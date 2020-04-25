#pragma once

#define EPERM            1      /* Operation not permitted */
#define EAGAIN           2      /* Try again */
#define ENOMEM           3      /* Out of memory */
#define EACCES           4      /* Permission denied */
#define EINVAL           5      /* Invalid argument */
#define EFBIG            6      /* File too large */
#define ENOSPC           7      /* No space left on device */
#define ENOSYS           8      /* Function not implemented */
#define ENODATA          9      /* No data available */
#define ETIME           10      /* Timer expired */
#define ECAPBILITY      11      /* Invalid capability */
#define ESUPPORT        12      /* Not supported */
#define EBADSYSCALL     13      /* Bad syscall number */
#define ENOMAPPING      14      /* Bad syscall number */
#define ENOENT          15      /* Entry does not exist */
#define EEXIST          16      /* Entry already exists */
#define ENOTEMPTY       17      /* Dir is not empty */
#define ENOTDIR         18      /* Does not refer to a directory */

#define EMAX            19

#define IS_ERR(x) ((((long long)(x)) < 0) && ((long long)(x)) > -EMAX)
#define ERR_PTR(x) ((void *)(long long)(x))
#define PTR_ERR(x) ((long)(x))
