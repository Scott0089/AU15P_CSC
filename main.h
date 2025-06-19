#ifndef MAIN_H
#define MAIN_H

/* System includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

/* Project includes */
#include "xstatus.h"
#include "sc_io.h"

#include "xgpio.h"
#include "xsysmon.h"
#include "xtmrctr.h"
#include "xv_tpg.h"
#include "xvidc.h"
#include "xvprocss.h"

/* Device configuration */
#define TARGET_PHYS_ADDR 0x40000000

/* Global variables */
extern int fd;

#endif /* MAIN_H */
