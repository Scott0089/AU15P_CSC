#include "xv_sdirxss.h"

XV_SdiRxSs_Config XV_SdiRxSs_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-smpte-uhdsdi-rx-ss-2.0", /* compatible */
		0x400c0000, /* reg */
		0x2, /* xlnx,pixels-per-clock */
		0x2, /* xlnx,sdiline-rate */
		0xa, /* xlnx,bpp */
		0x1, /* sdirx-present */
		0x0, /* sdirx-connected */
		0xffff, /* interrupts */
		0xffff /* interrupt-parent */
	},
	{
		"xlnx,v-smpte-uhdsdi-rx-1.0", /* compatible */
		0x0, /* reg */
		0x0, /* xlnx,pixels-per-clock */
		0x2, /* xlnx,sdiline-rate */
		0xa, /* xlnx,bpp */
		0x0, /* sdirx-present */
		0x0, /* sdirx-connected */
		0xffff, /* interrupts */
		0xffff /* interrupt-parent */
	},
	 {
		 NULL
	}
};