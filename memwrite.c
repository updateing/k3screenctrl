#include <sys/mman.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int write_memory_byte(void* addr, unsigned char data) {
	int memfd = open("/dev/mem", O_RDWR);
	if (memfd < 0) {
		syslog(LOG_ERR, "Unable to access memory: %s", strerror(errno));
		return FAILURE;
	}

	long page_size = sysconf(_SC_PAGESIZE);
	void* map_start = addr & page_size;
	void* data_offset = addr - map_start;

	void* map_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, map_start);
	if (map_addr == -1) {
		syslog(LOG_ERR, "Unable to mmap: %s", strerror(errno));
		close(memfd);
		return FAILURE;
	}

	void* virt_addr = map_addr + data_offset;
	*((unsigned char*) virt_addr) = data;

	unsigned char read_result = *((unsigned char*) virt_addr);
	if (read_result != data) {
		syslog(LOG_INFO, "Written %p with %hhx but read %hhx back\n", addr, data, read_result);
	}

	munmap(map_addr, page_size);
	close(memfd);
	return SUCCESS;
}