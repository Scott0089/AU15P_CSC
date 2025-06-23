#include "xv_sdirx.h"

XV_SdiRx_Config XV_SdiRx_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-smpte-uhdsdi-rx-1.0", /* compatible */
		0x0, /* reg */
		0x1, /* xlnx,include-edh */
		0x2 /* xlnx,sdiline-rate */
	},
	{
		"xlnx,v-smpte-uhdsdi-rx-ss-2.0", /* compatible */
		0x400c0000, /* reg */
		0x1, /* xlnx,include-edh */
		0x2 /* xlnx,sdiline-rate */
	},
	 {
		 NULL
	}
};