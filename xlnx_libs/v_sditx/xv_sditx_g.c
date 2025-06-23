#include "xv_sditx.h"

XV_SdiTx_Config XV_SdiTx_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-smpte-uhdsdi-tx-1.0", /* compatible */
		0x0, /* reg */
		0x0, /* xlnx,include-edh */
		0x2, /* xlnx,sdiline-rate */
		0x0 /* xlnx,c_str_st352 */
	},
	 {
		 NULL
	}
};