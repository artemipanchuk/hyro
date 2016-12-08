#ifndef SYSCALLS_WRAPPER_H
#define SYSCALLS_WRAPPER_H

int setup_syscalls_wrapper(void);

asmlinkage long sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);
asmlinkage long sys_write(unsigned int fd, const char __user *buf, size_t count);
asmlinkage long sys_open(const char __user *filename, int flags, umode_t mode);
asmlinkage long sys_read(unsigned int fd, char __user *buf, size_t count);

#endif