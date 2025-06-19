#include "xv_csc.h"

XV_csc_Config XV_csc_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-csc-1.1", /* compatible */
		0x0, /* reg */
		0x2, /* xlnx,samples-per-clock */
		0xf00, /* xlnx,v-csc-max-width */
		0x870, /* xlnx,v-csc-max-height */
		0xa, /* xlnx,max-data-width */
		0x1, /* xlnx,enable-422 */
		0x1, /* xlnx,enable-420 */
		0x1 /* xlnx,enable-window */
	},
	 {
		 NULL
	}
};