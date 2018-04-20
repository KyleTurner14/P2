//Robert Shannahan
//Kyle Turner
//John Millner

//libraries.
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

//#include "index.h"


//static vals
static int deviceNumber;
static int numOpen = 0;
static int numClose = 0;
static int numRead = 0;
static int error_count = 0;

int front = 0;
int size = 0;
int back = 0;


char message[1024];
static DEFINE_MUTEX( queueMutex );

EXPORT_SYMBOL(back);
EXPORT_SYMBOL(size);
EXPORT_SYMBOL(front);
EXPORT_SYMBOL(message);
EXPORT_SYMBOL(queueMutex);


static struct class* group14ReadClass = NULL;
static struct device* group14ReadDevice = NULL;

//prototype functions
extern int init_module(void);
extern void cleanup_module(void);
static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file*, char *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);

//define
#define BUFF_LEN 1024
#define DEVICE_NAME "group14Read"
#define CLASS_NAME "gp14read"

//MODULE_INFO
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 14");
MODULE_DESCRIPTION("Linux Char Driver");
MODULE_VERSION("1.0");

static struct file_operations fops =
{
		.open = dev_open,
		.read = dev_read,
		.release = dev_release,
};

	int init_module(void){
	//initialize
	printk(KERN_INFO "group14Read: Installing Module...\n");

	//make sure device number is > 0
	deviceNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(deviceNumber<0){
		printk(KERN_ALERT "group14Read: failed to register a positive number\n.");
		return deviceNumber;
	}
	printk(KERN_INFO "group14Read: registered correctly with number %d\n", deviceNumber);

	group14ReadClass = class_create(THIS_MODULE,CLASS_NAME);
	if(IS_ERR(group14ReadClass)){
		unregister_chrdev(deviceNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(group14ReadClass);
	}

	group14ReadDevice = device_create(group14ReadClass, NULL, MKDEV(deviceNumber, 0), NULL, DEVICE_NAME );
	if(IS_ERR(group14ReadDevice)){
		class_destroy(group14ReadClass);
		unregister_chrdev(deviceNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(group14ReadDevice);
	}
	printk(KERN_INFO "group14Read: device class created correctly\n.");
	return 0;
}


	void cleanup_module(void){
	device_destroy(group14ReadClass, MKDEV(deviceNumber, 0));
	class_unregister(group14ReadClass);
	class_destroy(group14ReadClass);
	unregister_chrdev(deviceNumber, DEVICE_NAME);
	printk(KERN_INFO "group14Read: Goodbye!\n");
}

static int dev_open(struct inode* inodep, struct file * filep){

	numOpen++;
	printk(KERN_INFO "group14Read: Device has been opened %d time(s)\n", numOpen);
	return 0;
}

static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t *offset){

	int i = 0;
	char sendBack[700];
	int old_size = size;

	//lock the mutex
	mutex_lock(&queueMutex);

	//report using printk each time it is written to 
	numRead++;
	printk(KERN_INFO "group14Read: Device has been read from %d time(s)\n", numRead);

	//read info
	error_count = 0;

	while (size > 0) 
	{

		sendBack[i] = message[front];
		message[front] = 0;
		
		put_user( sendBack[i], buffer++ );
		printk ( KERN_INFO "%d\t%c\n", size, sendBack[i] );
		// increment
		front++;
		size--;

	}
	
	printk(KERN_INFO "No more characters left.\n");

	//unlock the mutex
	mutex_unlock(&queueMutex);

	printk(KERN_INFO "group14Read: The length is currently %d bytes\n", back);

	printk(KERN_INFO "group14Read: Sent %d characters to the user\n", back);

	return old_size;

}//end dev_read 

static int dev_release(struct inode *inodep, struct file *filep){
    numClose++;
    printk(KERN_INFO "group14Read: Device has been successfully closed %d time(s)\n", numClose);
    return 0;
}
