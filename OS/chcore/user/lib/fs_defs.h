#pragma once

#define TMPFS_MAP_BUF	1
#define TMPFS_SCAN	2
#define TMPFS_MKDIR	3
#define TMPFS_RMDIR	4
#define TMPFS_CREAT	5
#define TMPFS_UNLINK	6
#define TMPFS_OPEN	7
#define TMPFS_CLOSE	8
#define TMPFS_WRITE	9
#define TMPFS_READ	10

#define TMPFS_GET_SIZE	999

#define TMPFS_INFO_VADDR 0x200000
#define TMPFS_SCAN_BUF_VADDR 0x20000000
#define TMPFS_READ_BUF_VADDR 0x30000000

enum FS_REQ {
	FS_REQ_OPEN = 0,
	FS_REQ_CLOSE,

	FS_REQ_CREAT,
	FS_REQ_MKDIR,
	FS_REQ_RMDIR,
	FS_REQ_UNLINK,

	FS_REQ_SCAN,
	FS_REQ_READ,
	FS_REQ_WRITE,

	FS_REQ_GET_SIZE
};


#define FS_REQ_PATH_LEN (256)
struct fs_request {              // offset:
	enum FS_REQ req;             // 0

	char *buff;                  // 8
	int flags;                   // 16
	mode_t mode;                 // 20
	off_t offset;                // 24
	ssize_t count;               // 32

	char path[FS_REQ_PATH_LEN];  // 40

};

struct dirent {                                                   // offset:
	ino_t          d_ino;       /* Inode number */                // 0
	off_t          d_off;       /* Not an offset; see below */    // 8
	unsigned short d_reclen;    /* Length of this record */       // 16
	unsigned char  d_type;      /* Type of file; not supported    // 18
				       by all filesystem types */
	char           d_name[256]; /* Null-terminated filename */    // 19
};


