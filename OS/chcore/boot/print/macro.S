#include <common/asm.h>

BEGIN_FUNC(early_put32)
    str w1,[x0]
    ret
END_FUNC(early_put32)

BEGIN_FUNC(early_get32)
    ldr w0,[x0]
    ret
END_FUNC(early_get32)

BEGIN_FUNC(delay)
	subs x0, x0, #1
	bne delay
	ret
END_FUNC(delay)
