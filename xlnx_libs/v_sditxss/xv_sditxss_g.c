#include "xv_sditxss.h"

XV_SdiTxSs_Config XV_SdiTxSs_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-smpte-uhdsdi-tx-ss-2.0", /* compatible */
		0x400a0000, /* reg */
		0x2, /* xlnx,pixels-per-clock */
		0x2, /* xlnx,sdiline-rate */
		0x1, /* xlnx,Isstd_352 */
		0xa, /* xlnx,bpp */
		0x1, /* sditx-present */
		0x0, /* sditx-connected */
		0x1, /* sdivtc-present */
		0x10000, /* sdivtc-connected */
		0xffff, /* interrupts */
		0xffff /* interrupt-parent */
	},
	 {
		 NULL
	}
};