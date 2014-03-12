#ifndef FTRACE_MCOUNT_H
#define FTRACE_MCOUNT_H

#include <stdint.h>
#include <libelf.h>

#define likely(x)  __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define MCOUNT_RSTACK_MAX  128
#define MCOUNT_NOTRACE_IDX 0x10000
#define MCOUNT_INVALID_DYNIDX 0xffff

struct mcount_ret_stack {
	unsigned long parent_ip;
	unsigned long child_ip;
	/* time in nsec (CLOCK_MONOTONIC) */
	uint64_t start_time;
	uint64_t end_time;
	uint64_t child_time;
	int tid;
	unsigned char depth;
	unsigned char unused;
	unsigned short dyn_idx;
};

extern __thread int mcount_rstack_idx;
extern __thread struct mcount_ret_stack *mcount_rstack;

int mcount_entry(unsigned long parent, unsigned long child);
unsigned long mcount_exit(void);
void __monstartup(unsigned long low, unsigned long high);
void _mcleanup(void);


#define FTRACE_MAGIC_LEN  8
#define FTRACE_MAGIC_STR  "Ftrace!"
#define FTRACE_FILE_VERSION  2
#define FTRACE_FILE_NAME  "ftrace.data"

struct ftrace_file_header {
	char magic[FTRACE_MAGIC_LEN];
	uint32_t version;
	uint16_t header_size;
	uint8_t  endian;
	uint8_t  class;
	uint64_t length; /* file size including header size */
	uint64_t info_mask;
	uint32_t nr_maps;
	uint32_t unused;
};

enum ftrace_info_bits {
	EXE_NAME,
	EXE_BUILD_ID,
	EXIT_STATUS,
	CMDLINE,
	CPUINFO,
	MEMINFO,
	OSINFO,
};

struct ftrace_info {
	char *exename;
	unsigned char build_id[20];
	int exit_status;
	char *cmdline;
	int nr_cpus_online;
	int nr_cpus_possible;
	char *cpudesc;
	char *meminfo;
	char *kernel;
	char *hostname;
	char *distro;
};

struct ftrace_file_handle {
	FILE *fp;
	struct ftrace_file_header hdr;
	struct ftrace_info info;
};

void fill_ftrace_info(uint64_t *info_mask, int fd, char *exename, Elf *elf,
		      int status);
int read_ftrace_info(uint64_t info_mask, struct ftrace_file_handle *handle);
void clear_ftrace_info(struct ftrace_info *info);

int arch_fill_cpuinfo_model(int fd);

#endif /* FTRACE_MCOUNT_H */
