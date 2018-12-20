#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>


#define  DEVICE_NAME "/dev/phonebook"
#define  COMMAND_SIZE 256

struct user_data {
    char name[30];
    char last_name[30];
    char phnumber[15];
};

asmlinkage long sys_add_user(struct user_data *user) {
    int fd;
    char command[COMMAND_SIZE];
    
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = ksys_open(DEVICE_NAME, 0, 10);

    sprintf(command, "add %s %s %s",
            user->name,
            user->last_name,
            user->phnumber);
    
    ksys_write(fd, command, COMMAND_SIZE);
    ksys_close(fd);
    
    set_fs(old_fs);
    
    return 0;
}
