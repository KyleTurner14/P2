//Robert Shannahan
//Kyle Turner
//John Millner

//libraries.
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "global.h"

//static vals
static int deviceNumber;
static int numOpen = 0;
static int numClose = 0;
static int numRead = 0;
static int error_count = 0;

int front;
int back;
int size;

struct mutex queueMutex;
char message[1024];
static struct class* group14ReadClass = NULL;
static struct device* group14ReadDevice = NULL;

//prototype functions
extern int init_group14Read(void);
extern void  cleanup_group14Read(void);
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

	extern int init_group14Read(void){
	//initialize
	printk(KERN_INFO "group 14 Read: Installing Module...\n");

	//make sure device number is > 0
	deviceNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(deviceNumber<0){
		printk(KERN_ALERT "group 14 Read: failed to register a positive number\n.");
		return deviceNumber;
	}
	printk(KERN_INFO "group 14 Read: registered correctly with number %d\n", deviceNumber);

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
	printk(KERN_INFO "group 14 Read: device class created correctly\n.");
	return 0;
}


extern void cleanup_Group14Read(void){
	device_destroy(group14ReadClass, MKDEV(deviceNumber, 0));
	class_unregister(group14ReadClass);
	class_destroy(group14ReadClass);
	unregister_chrdev(deviceNumber, DEVICE_NAME);
	printk(KERN_INFO "group 14 Read: Goodbye!\n");
}

static int dev_open(struct inode* inodep, struct file * filep){

	numOpen++;
	printk(KERN_INFO "group 14 Read: Device has been opened %d time(s)\n", numOpen);
	return 0;
}

static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t *offset){

	int i = 0;
	char sendBack[len];
	char printOut [len];


	//lock the mutex
	mutex_lock(&queueMutex);

	//report using printk each time it is written to 
	numRead++;
	printk(KERN_INFO "group 14 Read: Device has been read from %d time(s)\n", numRead);

	//read info
	error_count = 0;

	for(i = 0; i < len; i++){
		sendBack[i] = '0';
	}

	for(i = 0; i < len; i++){

		// check for no characters left
		if(size <= 0){
			printk(KERN_INFO "No more characters left.\n");
			break;
		}

		sendBack[i] = message[front % BUFF_LEN];
		message[front % BUFF_LEN] = '0';

		// increment
		front = (front + 1) % BUFF_LEN;
		size--;

	}

	// check for errors
	error_count = copy_to_user(buffer, sendBack, len);

	//unlock the mutex
	mutex_unlock(&queueMutex);

	//if error
	if(error_count != 0){
		printk(KERN_INFO "ERROR: Failed to send to back to user.\n");
		return -1;
	}

	for(i = 0; i < back; i++){
		if(sendBack[i] != '0')
		printOut[i] = sendBack[i];
	}

	printk(KERN_INFO "group 14 Read: Sent back %s.\n", printOut);

	printk(KERN_INFO "group 14 Read: The length is currently %d bytes\n", back);

	printk(KERN_INFO "group 14 Read: Sent %d characters to the user\n", back);

	return len;

}//end dev_read 

static int dev_release(struct inode *inodep, struct file *filep){
    numClose++;
    printk(KERN_INFO "group 14 Read: Device has been successfully closed %d time(s)\n", numClose);
    return 0;
}
