
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/version.h>
#include <linux/fs.h>

#include "character_dev.h"
#include <linux/if_tun.h>
#include <linux/if_macvlan.h>
#include <linux/signal.h>
#include <linux/pagemap.h>
#include <linux/page-flags.h>
#include <linux/mm_types.h>
#include "linux/mm.h"
#include <asm/pgtable.h>
#include <asm/page.h>

#include <asm/uaccess.h>
#include <linux/netdevice.h>
#include <linux/list.h>
#include <asm/siginfo.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_VERSION("1.9.17");
MODULE_AUTHOR("Drukhary");
MODULE_DESCRIPTION("OS LAB2");

static int lab_dev_open(struct inode *inode, struct file *file);
static int lab_dev_release(struct inode *inode, struct file *file);
static ssize_t lab_dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t lab_dev_write(struct file *filp, const char *buf, size_t len, loff_t *off);
static long lab_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static int lab_dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device File Opened...!!!\n");
    return 0;
}

static int lab_dev_release(struct inode *inode, struct file *file)
{

    printk(KERN_INFO "Device File Closed...!!!\n");
    return 0;
}

static ssize_t lab_dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Read Function\n");
    return 0;
}

static ssize_t lab_dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Write function\n");
    return len;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
static int device_ioctl(struct inode *inode,
                        struct file *file,
                        unsigned int ioctl_num,
                        unsigned long ioctl_param)
#else
static long lab_dev_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
#endif
{
    printk(KERN_INFO "lab_dev_ioctl(%p,%lu,%lu)", file, ioctl_num, ioctl_param);
    switch (ioctl_num)
    {
        case IOCTL_GET_INFO:
            struct lab_data *data = vmalloc(sizeof(struct lab_data));
            copy_from_user(data,(struct lab_data *)ioctl_param,sizeof(struct lab_data));
            struct task_struct *t = get_pid_task(find_get_pid(data->pid), PIDTYPE_PID);
            if (t == NULL)
            {
                printk(KERN_ERR "task_struct with pid=%d does not exist\n", data->pid);
                vfree(data);
                return 1;
            };
            data->result.tgid = t->tgid;
            data->result.task_flags = t->flags;
            data->result.sig_flags = t->signal->flags;
            data->result.sig_group_exit_code = t->signal->group_exit_code;
            data->result.sig_group_stop_count = t->signal->group_stop_count;

            copy_to_user((struct lab_response *)ioctl_param, data, sizeof(struct lab_data));
            vfree(data);
            break;
    }
    return 0;
}

struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = lab_dev_read,
    .write = lab_dev_write,
    .open = lab_dev_open,
    .release = lab_dev_release,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
    .ioctl = device_ioctl
#else
    .unlocked_ioctl = lab_dev_ioctl
#endif
};

int init_module()
{
    int ret_val;
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &file_ops);
    if (ret_val < 0)
    {
        printk(KERN_ALERT "%s failed with %d\n", "Sorry, registering the character device \n", ret_val);
        return ret_val;
    }

    printk(KERN_INFO "\nRegistration is a success The major device number is %d.\n", MAJOR_NUM);
    printk(KERN_INFO "If you want to talk to the device driver,\n");
    printk(KERN_INFO "you'll have to create a device file. \n");
    printk(KERN_INFO "I suggest you use:\n");
    printk(KERN_INFO "mknod %s c %d <MINOR-NUM>\n", DEVICE_NAME, MAJOR_NUM);
    printk(KERN_INFO "The device file name is important, because\n");
    printk(KERN_INFO "the ioctl program assumes that's the\n");
    printk(KERN_INFO "file you'll use.\n");

    return 0;
}

void cleanup_module()
{
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}