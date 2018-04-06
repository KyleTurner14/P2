//libraries.

//John Millner
//Robert Shannahan
//Kyle Turner

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

//static vals
static int deviceNumber;
static int numRead = 0;
static int numOpen = 0;
static int numClose = 0;
static int numWrite = 0;

char mutex stack_mutex;


int front = 0;
int back = 0;
int size = 0;

static int error_count = 0;
static char message[1024] = {0};
static struct class* group14Class = NULL;
static struct device* group14Device = NULL;

//prototype functions
extern int init group14Write_init(void);
extern void  group14Write_exit(void)
static int dev_open(struct inode *, struct file *);
static ssize_t dev_write(struct file*, const char *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);

//define
#define BUFF_LEN 1024
#define DEVICE_NAME "group14Write"
#define CLASS_NAME "gp14"

//MODULE_INFO
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 14");
MODULE_DESCRIPTION("Linux Char Driver");
MODULE_VERSION("1.0");

static struct file_operations fops =
{
		.open = dev_open,
		.write = dev_write,
		.release = dev_release,
};



extern int init group14Write_init(void){
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
	printk(KERN_INFO "group 14: device class created correctly\n.");
	mutex_init(&stack_mutex)
	return 0;
}


extern void group14_exit(void){
	mutex_destroy(@stack_mutex)
	device_destroy(group14Class, MKDEV(deviceNumber, 0));
	class_unregister(group14Class);
	class_destroy(group14Class);
	unregister_chrdev(deviceNumber, DEVICE_NAME);
	printk(KERN_INFO "group 14: Goodbye!\n");
}

static int dev_open(struct inode* inodep, struct file * filep){

	if(!mutex_trylock(&stack_mutex)){
	printk(KERN_ALERT "Group 14: Device in use by another process");
	return -EBUSY

	numOpen++;
	printk(KERN_INFO "group 14: Device has been opened %d time(s)\n", numOpen);
	return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

	int i = 0;    

	// report using printk each time it is written to.
	numWrite++;
	printk(KERN_INFO "group 14: Device has been written to %d time(s)\n", numWrite);

	// add new string to buffer
	for(i = 0; i < len; i++){

		// check for overflow
		if(size >= BUFF_LEN){
			printk(KERN_INFO "ERROR: Already Full.\n");
			return -1;
		}

		// add the new string to the end of the message
		message[back % BUFF_LEN] = buffer[i];

		// increment
		size++;
		back = (back+1)%BUFF_LEN;

	}// end for loop

	mutex_unlock(&stack_mutex)

	printk(KERN_INFO "group 14: The length is currently %d bytes\n", back);
	printk(KERN_INFO "group 14: Received %zu characters from the user\n", len);
	return len;
}





