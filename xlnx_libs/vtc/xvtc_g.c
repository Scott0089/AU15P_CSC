#include "xvtc.h"

XVtc_Config XVtc_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,v-tc-6.2", /* compatible */
		0x10000, /* reg */
		0xffff, /* interrupts */
		0xffff /* interrupt-parent */
	},
	 {
		 NULL
	}
};