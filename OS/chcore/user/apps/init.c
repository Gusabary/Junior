#include <print.h>
#include <syscall.h>
#include <launcher.h>
#include <defs.h>
#include <bug.h>
#include <fs_defs.h>
#include <ipc.h>
#include <string.h>
#include <proc.h>

#define SERVER_READY_FLAG(vaddr) (*(int *)(vaddr))
#define SERVER_EXIT_FLAG(vaddr)  (*(int *)((u64)vaddr+ 4))

extern ipc_struct_t *tmpfs_ipc_struct;
static ipc_struct_t ipc_struct;
static int tmpfs_scan_pmo_cap;

/* fs_server_cap in current process; can be copied to others */
int fs_server_cap;

// BUFLEN cannot be too large because filenames array's space is limited
#define BUFLEN	100 //4096
#define MAX_FILE_NUM 30

extern char getch();

static void find_complement(char filenames[MAX_FILE_NUM][BUFLEN], char *buf, 
	int complement_time, char *complement) 
{
	for (int i = 0; i < MAX_FILE_NUM; i++) {
		if (strlen(filenames[i]) < strlen(buf)) {
			continue;
		}
		bool match = true;
		for (int j = 0; j < strlen(buf); j++) {
			if (filenames[i][j] != buf[j]) {
				match = false;
				break;
			}
		}
		if (match) {
			complement_time--;
		}
		if (complement_time == 0) {
			strcpy(complement, filenames[i]);
			return;
		}
	}
}

// read a command from stdin leading by `prompt`
// put the commond in `buf` and return `buf`
// What you typed should be displayed on the screen
char *readline(const char *prompt)
{
  	static char buf[BUFLEN];

	int i = 0, j = 0;
	signed char c = 0;
	int ret = 0;
	char complement[BUFLEN];
	int complement_time = 0;

	if (prompt != NULL) {
		printf("%s", prompt);
	}

	char filenames[MAX_FILE_NUM][BUFLEN];
	ipc_msg_t *ipc_msg;
	ipc_msg = ipc_create_msg(tmpfs_ipc_struct, sizeof(struct fs_request), 1);

	struct fs_request fr = {
		.req = FS_REQ_SCAN,
		.buff = TMPFS_SCAN_BUF_VADDR,
		.offset = 0,
		.count = TMPFS_GET_SIZE,
	};
	memset(fr.path, 0, FS_REQ_PATH_LEN);
	fr.path[0] = '/';

	ipc_set_msg_data(ipc_msg, (char *)&fr, 0, sizeof(struct fs_request));
	ipc_set_msg_cap(ipc_msg, 0, tmpfs_scan_pmo_cap);
	int count = ipc_call(tmpfs_ipc_struct, ipc_msg);

	struct dirent *dirp = (struct dirent *)fr.buff;
	for (int i = 0; i < count; i++) {
		memset(filenames[i], 0, BUFLEN);
		strcpy(filenames[i], dirp->d_name);
		dirp = (struct dirent *)((void *)dirp + dirp->d_reclen);
	}

	ipc_destroy_msg(ipc_msg);

	memset(buf, 0, BUFLEN);
	while (1)
	{
		c = getch();
		if (c < 0)
			return NULL;
		// TODO(Lab5): your code here
		if (c == '\r' || c == '\n') {
			if (complement_time > 0) {
				strcpy(buf, complement);
			}
			printf("\n");
			break;
		}
		else if (c == '\t') {
			complement_time++;
			find_complement(filenames, buf, complement_time, complement);
		}
		else {
			buf[i++] = c;
		}
		usys_putc('\r');
		printf("%s", prompt);
		if (complement_time > 0) {
			printf("%s", complement);
		}
		else {
			printf("%s", buf);
		}
	}
	return buf;
}

static void handle_ls(int argc, char *argv[]) {
	ipc_msg_t *ipc_msg;
	ipc_msg = ipc_create_msg(tmpfs_ipc_struct, sizeof(struct fs_request), 1);

	struct fs_request fr = {
		.req = FS_REQ_SCAN,
		.buff = TMPFS_SCAN_BUF_VADDR,
		.offset = 0,
		.count = TMPFS_GET_SIZE,
	};
	memset(fr.path, 0, FS_REQ_PATH_LEN);
	fr.path[0] = '/';
	if (argc > 1) {
		BUG_ON(argc != 2);
		strcpy(fr.path + 1, argv[1]);  // XXX: hacking
	}

	ipc_set_msg_data(ipc_msg, (char *)&fr, 0, sizeof(struct fs_request));
	ipc_set_msg_cap(ipc_msg, 0, tmpfs_scan_pmo_cap);
	int count = ipc_call(tmpfs_ipc_struct, ipc_msg);

	struct dirent *dirp = (struct dirent *)fr.buff;
	for (int i = 0; i < count; i++) {
		printf("%s\n", dirp->d_name);
		dirp = (struct dirent *)((void *)dirp + dirp->d_reclen);
	}

	ipc_destroy_msg(ipc_msg);
}

static void handle_echo(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
}

static void handle_cat(int argc, char *argv[]) {
	BUG_ON(argc != 2);
	ipc_msg_t *ipc_msg;
	ipc_msg = ipc_create_msg(tmpfs_ipc_struct, sizeof(struct fs_request), 1);

	struct fs_request fr = {
		.req = FS_REQ_READ,
		.count = PAGE_SIZE,
	};
	memset(fr.path, 0, FS_REQ_PATH_LEN);
	fr.path[0] = '/';
	strcpy(fr.path + 1, argv[1]);  // XXX: hacking

	int loop = 0;
	while (1) {
		fr.offset = PAGE_SIZE * loop;
		fr.buff = TMPFS_SCAN_BUF_VADDR;
		
		ipc_set_msg_data(ipc_msg, (char *)&fr, 0, sizeof(struct fs_request));
		ipc_set_msg_cap(ipc_msg, 0, tmpfs_scan_pmo_cap);
		int count = ipc_call(tmpfs_ipc_struct, ipc_msg);

		for (int i = 0; i < count; i++) {
			printf("%c", fr.buff[i]);
		}

		loop++;
		if (count < fr.count) {
			break;
		}
	}

	ipc_destroy_msg(ipc_msg);
}

// run `ls`, `echo`, `cat`, `cd`, `top`
// return true if `cmdline` is a builtin command
int builtin_cmd(char *cmdline)
{
	// TODO(Lab5): your code here
	const int max_argc = 8;
	char *argv[max_argc];
	int argc = 0;
	struct fs_request fs_req;

	// parse cmdline to argc and argv
	char cmdline_copy[BUFLEN];
	char *cmdline_copy_p = cmdline_copy;

	strcpy(cmdline_copy, cmdline);
	int end = strlen(cmdline_copy);
	cmdline_copy[end] = ' ';
	cmdline_copy[end + 1] = 0;

	char *oldp = cmdline_copy_p;
	while (*cmdline_copy_p) {
		if (*cmdline_copy_p == ' ') {
			*cmdline_copy_p = 0;
			argv[argc++] = oldp;
			cmdline_copy_p++;
			while (*cmdline_copy_p == ' ') {
				cmdline_copy_p++;
			}
			oldp = cmdline_copy_p;
		}
		cmdline_copy_p++;
	}

	// run command
	if (!strcmp(argv[0], "ls")) {
		handle_ls(argc, argv);
		return 1;
	}
	if (!strcmp(argv[0], "echo")) {
		handle_echo(argc, argv);
		return 1;
	}
	if (!strcmp(argv[0], "cat")) {
		handle_cat(argc, argv);
		return 1;
	}
	if (!strcmp(argv[0], "cd")) {
		return 1;
	}
	if (!strcmp(argv[0], "top")) {
		usys_print_top();
		return 1;
	}
	return 0;
}

// run other command, such as execute an executable file
// return true if run sccessfully
int run_cmd(char *cmdline)
{
	struct user_elf user_elf;
	int ret;
	int info_pmo_cap;
	struct pmo_map_request pmo_map_requests[1];

	ret = readelf_from_fs(cmdline, &user_elf);
	if (ret < 0) {
		printf("[Shell] No such binary in current directory\n");
		return ret;
	}
	ret = launch_process_with_pmos_caps(&user_elf, NULL, NULL, 
		NULL, 0, NULL, 0, 0);
	usys_yield();
	return ret;
}

static int 
run_cmd_from_kernel_cpio(const char *filename, int *new_thread_cap,
			                   struct pmo_map_request *pmo_map_reqs,
			                   int nr_pmo_map_reqs)
{
	struct user_elf user_elf;
	int ret;

	ret = readelf_from_kernel_cpio(filename, &user_elf);
	if (ret < 0) {
		printf("[Shell] No such binary in kernel cpio\n");
		return ret;
	}
	return launch_process_with_pmos_caps(&user_elf, NULL, new_thread_cap,
					     pmo_map_reqs, nr_pmo_map_reqs,
					     NULL, 0, 0);
}

void boot_fs(void)
{
	int ret = 0;
	int info_pmo_cap;
	int tmpfs_main_thread_cap;
	struct pmo_map_request pmo_map_requests[1];

	/* create a new process */
	printf("Booting fs...\n");
	/* prepare the info_page (transfer init info) for the new process */
	info_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);
	fail_cond(info_pmo_cap < 0, "usys_create_ret ret %d\n", info_pmo_cap);

	ret = usys_map_pmo(SELF_CAP,
			   info_pmo_cap, TMPFS_INFO_VADDR, VM_READ | VM_WRITE);
	fail_cond(ret < 0, "usys_map_pmo ret %d\n", ret);

	SERVER_READY_FLAG(TMPFS_INFO_VADDR) = 0;
	SERVER_EXIT_FLAG(TMPFS_INFO_VADDR) = 0;

	/* We also pass the info page to the new process  */
	pmo_map_requests[0].pmo_cap = info_pmo_cap;
	pmo_map_requests[0].addr = TMPFS_INFO_VADDR;
	pmo_map_requests[0].perm = VM_READ | VM_WRITE;
	ret = run_cmd_from_kernel_cpio("/tmpfs.srv", &tmpfs_main_thread_cap,
				       pmo_map_requests, 1);
	fail_cond(ret != 0, "create_process returns %d\n", ret);

	fs_server_cap = tmpfs_main_thread_cap;

	while (SERVER_READY_FLAG(TMPFS_INFO_VADDR) != 1)
		usys_yield();

	/* register IPC client */
  tmpfs_ipc_struct = &ipc_struct;
	ret = ipc_register_client(tmpfs_main_thread_cap, tmpfs_ipc_struct);
	fail_cond(ret < 0, "ipc_register_client failed\n");

	tmpfs_scan_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);
	fail_cond(tmpfs_scan_pmo_cap < 0, "usys create_ret ret %d\n",
		  tmpfs_scan_pmo_cap);

	ret = usys_map_pmo(SELF_CAP,
			   tmpfs_scan_pmo_cap,
			   TMPFS_SCAN_BUF_VADDR, VM_READ | VM_WRITE);
	fail_cond(ret < 0, "usys_map_pmo ret %d\n", ret);

	printf("fs is UP.\n");
}

