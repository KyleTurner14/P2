#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>



//static vals
static int deviceNumber;
static int numOpen = 0;
static int numClose = 0;
static struct class* group14Class = NULL;
static struct device* group14Device = NULL;

//prototype functions
static int dev_open(struct inode *, struct file *);
//static ssize_t dev_write(struct file*, const char *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);
//define
#define BUFF_LEN 1024
#define DEVICE_NAME "group14"
#define CLASS_NAME "gp14"

//MODULE_INFO
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 14");
MODULE_DESCRIPTION("Linux Char Driver");
MODULE_VERSION("1.0");

static struct file_operations fops =
{
    .open = dev_open,
    //.write = dev_write,
    .release = dev_release,
};

   

    static int __init group14_init(void){
    //initialize
    printk(KERN_INFO "group 14: Installing Module...\n");
   
    //make sure device number is > 0
    deviceNumber = register_chrdev(0, DEVICE_NAME, &fops);
        if(deviceNumber<0){
        printk(KERN_ALERT "group 14 failed to register a positive number\n.");
        return deviceNumber;
        }
    printk(KERN_INFO "group 14: registered correctly with number %d\n", deviceNumber);

    group14Class = class_create(THIS_MODULE,CLASS_NAME);
        if(IS_ERR(group14Class)){
        unregister_chrdev(deviceNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(group14Class);
        }
   
    group14Device = device_create(group14Class, NULL, MKDEV(deviceNumber, 0), NULL, DEVICE_NAME );
        if(IS_ERR(group14Device)){
        class_destroy(group14Class);
        unregister_chrdev(deviceNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(group14Device);
        }
    printk(KERN_INFO "group14: device class created correctly\n.");
    return 0;
    }

    static void __exit group14_exit(void){
    device_destroy(group14Class, MKDEV(deviceNumber, 0));
    class_unregister(group14Class);
    class_destroy(group14Class);
    unregister_chrdev(deviceNumber, DEVICE_NAME);
    printk(KERN_INFO "group 14: Goodbye!\n");
    }
       
    static int dev_open(struct inode* inodep, struct file * filep){
        numOpen++;
    printk(KERN_INFO "group 14: Device has been opened %d time(s)\n", numOpen);
    return 0;
    }

    //static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    //report using printk each time it is written to.
    //printk(KERN_INFO "Device group 14 is being written to...");
    //store up to a buffer of 1kb
    //if message is long then only store 1kb
    //}

    static int dev_release(struct inode *inodep, struct file *filep){
    numClose++;
    printk(KERN_INFO "group14: Device has been successfully closed %d time(s)\n", numClose);
    return 0;
    }

    module_init(group14_init);
    module_exit(group14_exit);
