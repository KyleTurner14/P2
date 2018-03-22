#include <linux/module.h>
#include <linux/kernel.h>

//static vals

//prototype functions
static ssize_t dev_write(struct file*, const char *, size_t, loff_t *);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .close = dev_close,
};


    int init_module(void){
    printk(KERN_INFO "Installing Module...\n");
    return 0;
    }

    void cleanup_module(void){
    printk(KERN_INFO "Closing Kernel!\n");
    }
