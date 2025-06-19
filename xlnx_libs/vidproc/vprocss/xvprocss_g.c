#include "xvprocss.h"

XVprocSs_Config XVprocSs_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-proc-ss-2.3", /* compatible */
		0x40050000, /* reg */
		0x4005ffff, /* xlnx,highaddr */
		0x3, /* xlnx,topology */
		0x2, /* xlnx,samples-per-clk */
		0xa, /* xlnx,max-data-width */
		0x3, /* xlnx,num-video-components */
		0xf00, /* xlnx,max-cols */
		0x870, /* xlnx,max-rows */
		0x1, /* xlnx,deint-motion-adaptive */
		0x0, /* rstaximm-present */
		0x0, /* rstaximm-connected */
		0x0, /* rstaxis-present */
		0x0, /* rstaxis-connected */
		0x0, /* vdma-present */
		0x0, /* vdma-connected */
		0x0, /* router-present */
		0x0, /* router-connected */
		0x1, /* csc-present */
		0x0, /* csc-connected */
		0x0, /* deint-present */
		0x0, /* deint-connected */
		0x0, /* hcrsmplr-present */
		0x0, /* hcrsmplr-connected */
		0x0, /* hscale-present */
		0x0, /* hscale-connected */
		0x0, /* lbox-present */
		0x0, /* lbox-connected */
		0x0, /* vcrsmplrin-present */
		0x0, /* vcrsmplrin-connected */
		0x0, /* vcrsmplrout-present */
		0x0, /* vcrsmplrout-connected */
		0x0, /* vscale-present */
		0x0 /* vscale-connected */
	},
	 {
		 NULL
	}
};