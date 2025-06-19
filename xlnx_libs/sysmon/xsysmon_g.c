#include "xsysmon.h"

XSysMon_Config XSysMon_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,system-management-wiz-1.3", /* compatible */
		0x40020000, /* reg */
		0x1, /* xlnx,include-intr */
		0x1 /* xlnx,ip-type */
	},
	 {
		 NULL
	}
};