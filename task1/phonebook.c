#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/slab.h>
#include <linux/list.h>

#define MODULE_NAME "phonebook"
#define DEVICE_NAME "phonebook"
#define BUFSIZE 256
#define RESULT_SIZE 256

MODULE_AUTHOR("Kudryavy <miroshnichenko@phystech.edu.>");
MODULE_DESCRIPTION("Kernel-mode phone book");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

struct phonebook_item
{
    char name[30];
    char last_name[30];
    char phnumber[15];
    struct list_head list;  // структура двусвязного списка
};

struct phonebook_item phonebook;

static char message[BUFSIZE] = {0};
static char result[RESULT_SIZE];
static int major_number;
static int device_open_cnt = 0;

/*
 * Prototypes for file operations
 */
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t , loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t , loff_t *);
static void findphb(void);
static void write_word(char *, int *, char *);
static void add_phonebook_item(void);
static void delete_phonebook_item(void);

/*
 * File operations associated with the device
 */
static void findphb(void)
{
    int num;
    char* mess = message + 5;
    char input_last_name[30];
    struct phonebook_item* current_phonebook;
    strcpy(input_last_name, mess);
    input_last_name[strlen(input_last_name) - 1] = '\0';
    
    num = 0;
    result[0] = '\0';
    list_for_each_entry(current_phonebook, &phonebook.list, list) 
    {
        if (!strcmp(current_phonebook->last_name, input_last_name)) 
        {
            ++num;
            strcat(result, current_phonebook->name);
            strcat(result, " ");
            strcat(result, current_phonebook->last_name);
            strcat(result, " ");
            strcat(result, current_phonebook->phnumber);
            strcat(result, "\n");
            printk(KERN_INFO "info record: name: %s, lastname: %s, phone: %s \n", 
                current_phonebook->name, current_phonebook->last_name, current_phonebook->phnumber);    
        }
    }
    if (num == 0){
        printk(KERN_INFO "not find %s", input_last_name);
        strcpy(result, "not find");
    }
}

static void write_word(char* source, int* start, char* word)
{
    int i = 0;
    while(source[*start] != ' ' && source[*start] != '\n' && source[*start] != '\0') 
    {
        word[i++] = source[(*start)++];
    }
    word[i] = '\0';
    (*start)++;
}

static void add_phonebook_item(void)
{
    char* mess = message + 4;
    struct phonebook_item *new_phonebook;
    int i = 0;
    new_phonebook = (struct phonebook_item*)kmalloc(sizeof(struct phonebook_item), GFP_KERNEL);
    write_word(mess, &i, new_phonebook->name);
    write_word(mess, &i, new_phonebook->last_name);
    write_word(mess, &i, new_phonebook->phnumber);
    
    INIT_LIST_HEAD(&new_phonebook->list);
    list_add_tail(&(new_phonebook->list), &(phonebook.list));
    printk(KERN_INFO "add phonebook_item: name: %s, lastname: %s, phone: %s \n", new_phonebook->name, new_phonebook->last_name, new_phonebook->phnumber);
    strcpy(result, "added");
}

static void delete_phonebook_item(void)
{
    char* mess = message + 7;
    int num;
    char input_last_name[30];
    struct phonebook_item* current_phonebook;
    struct phonebook_item* tmp;
    strcpy(input_last_name, mess);
    input_last_name[strlen(input_last_name) - 1] = '\0';

    num = 0;
    list_for_each_entry_safe(current_phonebook, tmp, &phonebook.list, list) {
        if (!strcmp(current_phonebook->last_name, input_last_name)) {
            ++num;
            printk(KERN_INFO "delete record: name: %s, lastname: %s, phone: %s \n", 
                current_phonebook->name, current_phonebook->last_name, current_phonebook->phnumber);
            list_del(&current_phonebook->list);
            kfree(current_phonebook);
            strcpy(result, "deleted");
        }
    }
    if (num == 0){
        printk(KERN_INFO "delete record: not such record: %s", input_last_name);
        strcpy(result, "not such record");
    }
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init phonebook_init(void)
{
    INIT_LIST_HEAD(&(phonebook.list));
    /* Allocating major number dynamically */
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) 
    {
        printk(KERN_ALERT "[%s]: failed to register char device\n",
                           MODULE_NAME);
        return major_number;
    }
    else 
    {
        printk(KERN_INFO "[%s]: char device registered successfully, major number: %d\n",
                          MODULE_NAME, major_number);
        return 0;
    }
}

static void __exit phonebook_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "[%s]: module uninitialized successfully\n", MODULE_NAME);
}

static int dev_open(struct inode *inode, struct file *file)
{
    if (device_open_cnt)
    {
        return -EBUSY;
    }
    device_open_cnt++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    device_open_cnt--;
    module_put(THIS_MODULE);
    return 0;
}


static ssize_t dev_write(struct file *filp, const char *buffer,
                         size_t len, loff_t *offset)
{
    size_t res = copy_from_user(message, buffer, min((size_t)BUFSIZE-1, len));
    if (res != 0) 
    {
        printk(KERN_ALERT "[%s]: copy_from_user did not succeed\n", MODULE_NAME);
    }
    if (!strncmp(message, "info", 4))
    {
        findphb();
        return 0;
    }
    if (!strncmp(message, "add", 3))
    {
            add_phonebook_item();
            return 0;
    }
    if (!strncmp(message, "delete", 6))
    {
        delete_phonebook_item();
        return 0;
    }
    strncpy(result, "invalid command", RESULT_SIZE);
    return 0; 
}

static ssize_t dev_read(struct file *filp, char *buffer,
                        size_t len, loff_t *offset)
{
        if (copy_to_user(buffer, result, RESULT_SIZE))
        {
                return -EFAULT;
        } else
        {
                return 1;
        }
}

module_init(phonebook_init);
module_exit(phonebook_exit);
