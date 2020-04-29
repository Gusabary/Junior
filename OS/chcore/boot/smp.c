/* MPIDR AFF0 => core in processor, AFF1 => processor */
#include <lib/machine.h>

unsigned long MPIDR[PLAT_CPU_NUM] = {
	0x0,			/* core 0 */
	0x1,			/* core 1 */
	0x2,			/* core 2 */
	0x3,			/* core 3 */
};