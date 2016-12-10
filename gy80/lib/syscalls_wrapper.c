#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>

#include "syscalls_wrapper.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GY80 driver");
MODULE_VERSION("0.1");

static asmlinkage long (*local_sys_ioctl) (unsigned int, unsigned int, unsigned long);
static asmlinkage long (*local_sys_write) (unsigned int, const char __user*, size_t);
static asmlinkage long (*local_sys_open)  (const char __user*, int, umode_t);
static asmlinkage long (*local_sys_read)  (unsigned int, char __user*, size_t);
static asmlinkage long (*local_sys_close)  (unsigned int);

static void* lookup_symbol(const char *sym) {
	return (void*) kallsyms_lookup_name(sym);
}

int setup_syscalls_wrapper(void) {
	local_sys_ioctl  = lookup_symbol("sys_ioctl");
	local_sys_write  = lookup_symbol("sys_write");
	local_sys_open   = lookup_symbol("sys_open");
	local_sys_read   = lookup_symbol("sys_read");
	local_sys_close  = lookup_symbol("sys_close");

	if (local_sys_ioctl == NULL)
		return 1;

	if (local_sys_write == NULL)
		return 1;

	if (local_sys_open == NULL)
		return 1;

	if (local_sys_read == NULL)
		return 1;

	if (local_sys_close == NULL)
		return 1;

	return 0;
}

asmlinkage long sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg) {
	long result;
	
	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	result = local_sys_ioctl(fd, cmd, arg);

	set_fs(fs);

	return result;
}

asmlinkage long sys_write(unsigned int fd, const char __user *buf, size_t count) {
	long result;
	
	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	result = local_sys_write(fd, buf, count);

	set_fs(fs);

	return result;
}

asmlinkage long sys_open(const char __user *filename, int flags, umode_t mode) {
	long result;
	
	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	result = local_sys_open(filename, flags, mode);

	set_fs(fs);

	return result;
}

asmlinkage long sys_read(unsigned int fd, char __user *buf, size_t count) {
	long result;
	
	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	result = local_sys_read(fd, buf, count);

	set_fs(fs);

	return result;
}

asmlinkage long sys_close(unsigned int fd) {
	long result;
	
	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	result = local_sys_close(fd);

	set_fs(fs);

	return result;
}