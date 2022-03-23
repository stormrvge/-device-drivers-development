#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ratushnyak E. and Svistukhin A.");
MODULE_DESCRIPTION("Lab module");

#include <linux/fs.h>
#include <linux/proc_fs.h>

#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include <linux/uaccess.h>

#include <linux/slab.h>

#include "list.h"

#define BUSY 1
#define NOT_BUSY 0

static const char* const default_msg = "No write operations were made since module were installed\n";

static char buffer[60];

static int state = NOT_BUSY;
static struct list data = { 0 };

static dev_t first;
static struct cdev c_dev;
static struct class* cl;
static struct proc_dir_entry* entry;

static int lab_open(struct inode* i, struct file* f)
{
    if (state == BUSY) return -EBUSY;

    printk(KERN_INFO "lab: opened\n");
    state = BUSY;
    return 0;
}

static int lab_close(struct inode* i, struct file* f)
{
    printk(KERN_INFO "lab: closed\n");
    state = NOT_BUSY;
    return 0;
}

static size_t put_user_msg(char __user **buf, size_t* len, const char* msg)
{
    size_t i;
    const size_t msg_len = strlen(msg);

    if (msg_len > *len) return -ENOMEM;

    for(i = 0; i != msg_len; i++)
    {
        put_user(msg[i], *buf + i);
    }

    *len -= msg_len;
    *buf += i;

    return msg_len;
}

static ssize_t lab_read_to_user(struct file* f, char __user * buf, size_t len, loff_t *off)
{
    size_t current_len, diff, sum;
    struct list_impl* it;

    if (*off != 0) return 0;

    if (get_list_size(&data) == 0) 
    {
        current_len = put_user_msg(&buf, &len, default_msg);

        if (current_len == -ENOMEM) return -ENOMEM;

        diff = current_len - *off;
        *off += current_len;

        return diff;
    }
    else
    {
        current_len = put_user_msg(&buf, &len, "Device log\n");

        if (current_len == -ENOMEM) return -ENOMEM;

        sum = current_len;

        for (it = data.head; it != NULL; it = it->next)
        {
            if (sprintf(buffer, "Text was written with letters = %ld", it->value) < 0) return -EAGAIN; 

            current_len = put_user_msg(&buf, &len, buffer);
            
            if (current_len == -1) return -ENOMEM;  

            sum += current_len;

            if (len == 0) return -ENOMEM;

            put_user('\n', buf);

            buf++;
            len--;
            sum++;
        }

        *off += sum;
        return sum;
    }
}

static ssize_t lab_read_to_dmesg(struct file* f, char __user * buf, size_t len, loff_t *off)
{
    size_t current_len, diff, sum;
    struct list_impl* it;

    if (*off != 0) return 0;

    printk(KERN_INFO "lab: read\n"); 

    if (get_list_size(&data) == 0) 
    {
        printk(KERN_INFO "lab: %s", default_msg);
        
        *off += strlen(default_msg);

        return diff;
    }
    else
    {
        printk(KERN_INFO "lab: device log\n");

        current_len = put_user_msg(&buf, &len, "Device log\n");

        if (current_len == -ENOMEM) return -ENOMEM;

        sum = current_len;

        for (it = data.head; it != NULL; it = it->next)
        {
            if (sprintf(buffer, "Text was written with letters = %ld", it->value) < 0) return -EAGAIN; 

            printk(KERN_INFO "%s\n", buffer);
            sum += strlen(buffer);
        }

        *off += sum;
        return sum;
    }
}

static ssize_t lab_write(struct file* f, const char __user *buf, size_t len, loff_t* off)
{
    char temp;
    size_t counter, i;
    printk(KERN_INFO "lab: write\n");

    counter = 0;

    for (i = 0; i != len; i++)
    {
        get_user(temp, buf + i);
    
        counter += ('A' <= temp && temp <= 'Z') || ('a' <= temp && temp <= 'z');
    }

    if (add_to_list(&data, counter) != 0) return -ENOMEM;
    
    *off = 0;
    return len;
}

static struct file_operations fops = 
{
    .owner = THIS_MODULE,
    .open = lab_open,
    .release = lab_close,
    .read = lab_read_to_dmesg,
    .write = lab_write
}; 

static struct proc_ops proc_ops = 
{
    .proc_open = lab_open,
    .proc_release = lab_close,
    .proc_read = lab_read_to_user
};

static int __init lab_init(void)
{
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0) return -EAGAIN;

    if ((cl = class_create(THIS_MODULE, "char_drive")) == NULL) 
    {
        unregister_chrdev_region(first, 1);
        return -EAGAIN;
    }

    if (device_create(cl, NULL, first, NULL, "var5") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -EAGAIN;
    }

    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -EAGAIN;
    }

    entry = proc_create("var5", 0444, NULL, &proc_ops);

    if (entry == NULL) 
    {
        cdev_del(&c_dev);
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -EAGAIN;
    }

    return 0;
}

static void __exit lab_exit(void) 
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    proc_remove(entry);

    destroy_list(&data);
}

module_init(lab_init);
module_exit(lab_exit);
