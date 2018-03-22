// Group 14:	Kyle Turner
//		Robert Shannahan
//		John Millner
// Program 2: 	Kernel Character Device

// Includes
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

// Defines
#define  DEVICE_NAME "CHAR_READER"	// The device will appear at /dev/CHAR_READER using this value
#define  CLASS_NAME  "READER"		// The device class

// Global Variables
static int    majorNumber;              // Stores the device number -- determined automatically
static char   message[256] = {0};       // Memory for the string that is passed from userspace
static short  size_of_message;          // Used to remember the size of the string stored
static int    numberOpens = 0;          // Counts the number of times the device is opened
static int    numberCloses = 0;		// Counts the number of times the device is closed
static struct class*charClass  = NULL; 	// The device-driver class struct pointer
static struct device*charDevice = NULL; // The device-driver device struct pointer

// Structs
static struct file_operations fops =
{
	.open = dev_open,
	.close = dev_close,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

// Prototypes
static int     dev_closes(struct inode *, struct file *);
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// Function to initialize the device
static int __init char_init(void){
	printk(KERN_INFO "Loading the device.\n");
	
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

	// Check for valid major number
	if(majorNumber < 0){
		printk(KERN_INFO "ERROR: Could not assign major number.");

		return majorNumber;
	}

	// Print success message
	printk(KERN_INFO "The device has been loaded successfully! The device number is: %d.\n", majorNumber);

	// Register the device
	charDevice = deviceClass(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_Name);

	// Clean up if there is an error
	if(IS_ERR(charDevice)){
		//
		class_destroy(charClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_INFO "ERROR: Device could not be created\n");

		return PTR_ERR(charDevice);
	}

	// Print success message
	printk(KERN_INFO "The device class has been created successfully!\n");

	return 0;
}

// Function to track how many times the device has been opened.
static int dev_open(struct inode *inodep, struct file *filep){
	numberOpens++;
	printk(KERN_INFO "The device has been opened %d time(s)\n", numberOpens);
	
	return 0;
}

// Function to track how many times the device has been closed.
static int dev_close(struct inode *inodep, struct file *filep){
	numberCloses++;
	printk(KERN_INFO "EBBChar: Device has been closed %d time(s)\n", numberCloses);
	
	return 0;
}

// Function to close the device.
static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "The device has been closed.\n");

	return 0;
}

// Function to exit.
static int __exit chat_exit(void){
	device_destroy(charClass, MKDEV(majorNumber, 0));
   	class_unregister(charClass);
   	class_destroy(charClass);
   	unregister_chrdev(majorNumber, DEVICE_NAME);
   	printk(KERN_INFO "The device says goodbye!\n");
}

module_init(char_init);
module_exit(char_exit);
