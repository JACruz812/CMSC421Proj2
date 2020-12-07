#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_create_mbx_421 436
#define __NR_resize_mbx_421 437
#define __NR_delete_mbx_421 438
#define __NR_write_mbx_421 439
#define __NR_read_mbx_421 440
#define __NR_size_mbx_421 441
#define __NR_list_mbx_421 442
#define __NR_count_mbx_421 443

long create_mbx_421_syscall(const char* name, unsigned long size) {
    return syscall(__NR_create_mbx_421,name,size);
}

long resize_mbx_421_syscall(const char* name, unsigned long size) {
    return syscall(__NR_resize_mbx_421,name,size);
}

long delete_mbx_421_syscall(const char* name) {
    return syscall(__NR_delete_mbx_421,name);
}

long write_mbx_421_syscall(const char* name, const unsigned char* data, unsigned long offset, unsigned long n) {
    return syscall(__NR_write_mbx_421,name,data,offset,n);
}

long read_mbx_421_syscall(const char* name, unsigned char* data, unsigned long offset, unsigned long n) {
    return syscall(__NR_read_mbx_421,name,data,offset,n);
}

long size_mbx_421_syscall(const char* name) {
    return syscall(__NR_size_mbx_421,name);
}

long list_mbx_421_syscall(char** list, unsigned long max_entries) {
    return syscall(__NR_list_mbx_421,list,max_entries);
}

long count_mbx_421_syscall(void) {
    return syscall(__NR_count_mbx_421);
}

