//libraries.
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

//static vals
static int deviceNumber;
static int numRead = 0;
static int numOpen = 0;
static int numClose = 0;
static int numWrite = 0;

int front = 0;
int back = 0;
int size = 0;

static int error_count = 0;
static char message[1024] = {0};
static struct class* group14Class = NULL;
static struct device* group14Device = NULL;

//prototype functions

static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file*, const char *, size_t, loff_t *);
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
				.read = dev_read,
				.write = dev_write,
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
	printk(KERN_INFO "group 14: device class created correctly\n.");
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

	printk(KERN_INFO "group 14: The length is currently %d bytes\n", back);

	printk(KERN_INFO "group 14: Received %zu characters from the user\n", len);
	return len;
}

static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t *offset){

	int i = 0;
	char sendBack[len];
	
	char printOut[len];

	//report using printk each time it is written to 
	numRead++;
	printk(KERN_INFO "group 14: Device has been read from %d time(s)\n", numRead);

	//read info
	//error_count = 0;

	//for(i = 0; i < len; i++){
	//	sendBack[i] = '0';
	//}

	for(i = 0; i < len; i++){

		// check for no characters left
		if(size <= 0){
			printk(KERN_INFO "No more characters left.\n");
			break;
		}

		sendBack[i] = message[front];
		message[i] = 0;

		// increment
		front = (front + 1) % BUFF_LEN;
		back = (back - 1) % BUFF_LEN;
		size--;

	}

	// check for errors
	error_count = copy_to_user(buffer, sendBack, len);

	//if error
	if(error_count != 0){
		printk(KERN_INFO "ERROR: Failed to send to back to user.\n");
		return -1;
	}

	for(i = 0; i < len; i++){
		if(sendBack[i] != 0)
		printOut[i] = sendBack[i];
	}

	printk(KERN_INFO "group 14: Sent back %s.\n", printOut);

	printk(KERN_INFO "group 14: The length is currently %d bytes\n", back);

	printk(KERN_INFO "group 14: Sent %d characters to the user\n", back);
	return len;

}//end dev_read 

static int dev_release(struct inode *inodep, struct file *filep){
	numClose++;
	printk(KERN_INFO "group 14: Device has been successfully closed %d time(s)\n", numClose);
	return 0;
}

module_init(group14_init);
module_exit(group14_exit);

