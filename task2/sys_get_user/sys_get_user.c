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

asmlinkage long sys_get_user(const char *last_name, unsigned int len, struct user_data *output) {
    int fd;
    char command[COMMAND_SIZE] = "get ";
    char result[COMMAND_SIZE];
    
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);
    
    fd = ksys_open(DEVICE_NAME, 0, 10); 

    strncat(command, last_name, len);

    ksys_write(fd, command, COMMAND_SIZE);
    ksys_read(fd, result, COMMAND_SIZE);

    sscanf(result, "%s %s %s",
            output->name,
            output->last_name,
            output->phnumber);

    ksys_close(fd);
    
    set_fs(old_fs);
    
    return 0;
}
