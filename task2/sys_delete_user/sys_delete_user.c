#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/slab.h>
#include <linux/list.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>


#define  DEVICE_NAME "/dev/phonebook"
#define  COMMAND_SIZE 256

struct user_data {
    char name[30];
    char last_name[30];
    char phnumber[15];
};

asmlinkage long sys_del_user(const char *lat_name, unsigned int len) {
    int fd;
    char command[COMMAND_SIZE] = "delete ";
    
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);
    
    fd = ksys_open(DEVICE_NAME, 0, 10);

    strncat(command, last_name, len);

    ksys_write(fd, command, COMMAND_SIZE);
    ksys_close(fd);
    
    set_fs(old_fs);
    
    return 0;
}
