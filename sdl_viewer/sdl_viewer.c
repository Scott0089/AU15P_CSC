#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>

#define WIDTH 1920
#define HEIGHT 1080
#define HEADER_SIZE 4
#define FRAME_SIZE_BYTES ((WIDTH * HEIGHT / 2) * 8)
#define TOTAL_SIZE (HEADER_SIZE + FRAME_SIZE_BYTES)

// Convert 10-bit YUV to 8-bit RGB
static void yuv2rgb(uint16_t y, uint16_t u, uint16_t v, uint8_t* r, uint8_t* g, uint8_t* b) {
    y = y * 255 / 1023;
    u = u * 255 / 1023;
    v = v * 255 / 1023;
    int c = y - 16;
    int d = u - 128;
    int e = v - 128;
    int rr = (298 * c + 409 * e + 128) >> 8;
    int gg = (298 * c - 100 * d - 208 * e + 128) >> 8;
    int bb = (298 * c + 516 * d + 128) >> 8;
    *r = rr < 0 ? 0 : rr > 255 ? 255 : rr;
    *g = gg < 0 ? 0 : gg > 255 ? 255 : gg;
    *b = bb < 0 ? 0 : bb > 255 ? 255 : bb;
}

int main(int argc, char **argv) {
    struct stat st;
    ino_t last_inode = 0;
    bool rgb_mode = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--rgb") == 0) {
            rgb_mode = true;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            fprintf(stderr, "Usage: %s [--rgb]\n", argv[0]);
            return 1;
        }
    }

    int fd = open("/dev/shm/xdma_buffer", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    off_t filesize = lseek(fd, 0, SEEK_END);
    printf("File size: %ld, Expected: %d\n", filesize, TOTAL_SIZE);
    if (filesize < TOTAL_SIZE) {
        fprintf(stderr, "File is too small!\n");
        close(fd);
        return 1;
    }
    lseek(fd, 0, SEEK_SET);
    uint8_t* yuv = mmap(NULL, TOTAL_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (yuv == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // SDL2 setup
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        munmap(yuv, TOTAL_SIZE);
        close(fd);
        return 1;
    }
    SDL_Window* win = SDL_CreateWindow("YUV422 10-bit Video", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        munmap(yuv, TOTAL_SIZE);
        close(fd);
        return 1;
    }
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    uint8_t* rgb_frame = malloc(WIDTH * HEIGHT * 3);
    uint8_t* local_frame = malloc(FRAME_SIZE_BYTES);

    printf("mmap and SDL setup complete\n");
    printf("rgb_frame: %p, local_frame: %p\n", rgb_frame, local_frame);

    bool quit = false;
    SDL_Event e;
    // FPS calculation variables
    int frame_count = 0;
    double fps = 0.0;
    time_t last_time = time(NULL);
    char window_title[128];
    while (!quit) {
        uint32_t counter_before, counter_after;
        do {
            counter_before = *(volatile uint32_t *)yuv;
            if (counter_before % 2 != 0) continue; // Producer is writing
            memcpy(local_frame, yuv + HEADER_SIZE, FRAME_SIZE_BYTES);
            counter_after = *(volatile uint32_t *)yuv;
        } while (counter_before != counter_after || counter_before % 2 != 0);

        if (rgb_mode) {
            // Convert 10-bit RGB packed (2 pixels per 64 bits, 4 padding bits at end) to 8-bit RGB
            int idx = 0;
            for (int y = 0; y < HEIGHT; ++y) {
                for (int x = 0; x < WIDTH; x += 2) {
                    uint64_t val;
                    memcpy(&val, local_frame + idx, 8);
                    uint16_t R0 = (val >> 0) & 0x3FF;
                    uint16_t G0 = (val >> 10) & 0x3FF;
                    uint16_t B0 = (val >> 20) & 0x3FF;
                    uint16_t R1 = (val >> 30) & 0x3FF;
                    uint16_t G1 = (val >> 40) & 0x3FF;
                    uint16_t B1 = (val >> 50) & 0x3FF;
                    // 4 bits padding at bits 60-63
                    int pix_idx = (y * WIDTH + x) * 3;
                    rgb_frame[pix_idx + 0] = (R0 * 255) / 1023;
                    rgb_frame[pix_idx + 1] = (G0 * 255) / 1023;
                    rgb_frame[pix_idx + 2] = (B0 * 255) / 1023;
                    rgb_frame[pix_idx + 3] = (R1 * 255) / 1023;
                    rgb_frame[pix_idx + 4] = (G1 * 255) / 1023;
                    rgb_frame[pix_idx + 5] = (B1 * 255) / 1023;
                    idx += 8;
                }
            }
        } else {
            // Convert YUV422 10-bit packed to RGB using local_frame
            int idx = 0;
            for (int y = 0; y < HEIGHT; ++y) {
                for (int x = 0; x < WIDTH; x += 2) {
                    uint64_t val;
                    memcpy(&val, local_frame + idx, 8);
                    uint16_t Y0 = (val >> 0) & 0x3FF;
                    uint16_t U0 = (val >> 10) & 0x3FF;
                    uint16_t Y1 = (val >> 20) & 0x3FF;
                    uint16_t V0 = (val >> 30) & 0x3FF;
                    uint8_t r, g, b;
                    yuv2rgb(Y0, U0, V0, &r, &g, &b);
                    int pix_idx = (y * WIDTH + x) * 3;
                    rgb_frame[pix_idx + 0] = r;
                    rgb_frame[pix_idx + 1] = g;
                    rgb_frame[pix_idx + 2] = b;
                    yuv2rgb(Y1, U0, V0, &r, &g, &b);
                    rgb_frame[pix_idx + 3] = r;
                    rgb_frame[pix_idx + 4] = g;
                    rgb_frame[pix_idx + 5] = b;
                    idx += 8;
                }
            }
        }

        SDL_UpdateTexture(tex, NULL, rgb_frame, WIDTH * 3);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        // FPS calculation
        frame_count++;
        time_t now = time(NULL);
        if (now != last_time) {
            fps = frame_count / difftime(now, last_time);
            snprintf(window_title, sizeof(window_title), "YUV422 10-bit Video - FPS: %.1f", fps);
            SDL_SetWindowTitle(win, window_title);
            frame_count = 0;
            last_time = now;
        }

        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) quit = true;
        }
        //SDL_Delay(10); // ~100 FPS cap, adjust as needed
    }

    free(rgb_frame);
    free(local_frame);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    munmap(yuv, TOTAL_SIZE);
    close(fd);
    return 0;
}
