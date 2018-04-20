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
//#include "index.h"

//static vals
static int deviceNumber;
static int numOpen = 0;
static int numClose = 0;
static int numWrite = 0;

extern int front;
extern int back;
extern int size;

extern struct mutex queueMutex;
extern char message[1024];

static struct class* group14WriteClass = NULL;
static struct device* group14WriteDevice = NULL;

//prototype functions
extern int init_module(void);
extern void cleanup_module(void);
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

	extern int init_module(void){
	//initialize
	printk(KERN_INFO "group14Write: Installing Module...\n");

	//make sure device number is > 0
	deviceNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(deviceNumber<0){
		printk(KERN_ALERT "group14Write: failed to register a positive number\n.");
		return deviceNumber;
	}
	printk(KERN_INFO "group14Write: registered correctly with number %d\n", deviceNumber);

	group14WriteClass = class_create(THIS_MODULE,CLASS_NAME);
	if(IS_ERR(group14WriteClass)){
		unregister_chrdev(deviceNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(group14WriteClass);
	}

	group14WriteDevice = device_create(group14WriteClass, NULL, MKDEV(deviceNumber, 0), NULL, DEVICE_NAME );
	if(IS_ERR(group14WriteDevice)){
		class_destroy(group14WriteClass);
		unregister_chrdev(deviceNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(group14WriteDevice);
	}
	printk(KERN_INFO "group14Write: device class created correctly\n.");
	mutex_init(&queueMutex);
	return 0;
}


extern void cleanup_module(void){
	device_destroy(group14WriteClass, MKDEV(deviceNumber, 0));
	class_unregister(group14WriteClass);
	class_destroy(group14WriteClass);
	unregister_chrdev(deviceNumber, DEVICE_NAME);
	printk(KERN_INFO "group14Write: Goodbye!\n");
}

static int dev_open(struct inode* inodep, struct file * filep){

	numOpen++;
	printk(KERN_INFO "group14Write: Device has been opened %d time(s)\n", numOpen);
	return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

	int i = 0;    
	bool U = false;
	bool C = false;

	mutex_lock(&queueMutex);

	// report using printk each time it is written to.
	printk(KERN_INFO "group14Write: Device has been written to %d time(s)\n", ++numWrite);
	
	// add new string to buffer
	for(i = 0; i < len; i++)
	{
		//implement crappy state machine
		char c = buffer[i];
		
		// check for overflow
		if(size >= BUFF_LEN){
			mutex_unlock(&queueMutex);
			printk(KERN_INFO "ERROR: Already Full.\n");
			return -1;
		}
		
		// look for initial state of U
		if( c == 'U' )
		{
			U = true;
			C = false;
		}
		else if ( c == 'C' && U )
		{
			U = true;
			C = true;
		}
		else if ( c == 'F' && U && C )
		{
			//the string UCF has been written
			//now write in "Undefeated 2018 National Champions UCF"
			//we must first replace U and C
			
			char replacement[36] = "efeated 2018 National Champions UCF";
			int j = 0;
			
			message[back-2] = 'U';
			message[back-1] = 'n';
			message[back] = 'd';
			
			size += 35;
			
			// check for overflow
			if(size >= BUFF_LEN){
				mutex_unlock(&queueMutex);
				printk(KERN_INFO "ERROR: Already Full.\n");
				return -1;
			}
			
			//now that we know we are clear, add replacement
			
			for( j = 0; j <= 35; j++)
			{
				message[++back] = replacement[j];
			}
			
			U = false;
			C = false;
		}
		else
		{		
			// add the new string to the end of the message
			message[back] = c;

			// increment
			size++;
			back++;
		}

	}// end for loop
	mutex_unlock(&queueMutex);


	printk(KERN_INFO "group14Write: Received %zu characters from the user\n", len);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
    numClose++;
    printk(KERN_INFO "group14Write: Device has been successfully closed %d time(s)\n", numClose);
    return 0;
}



