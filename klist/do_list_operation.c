#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 

MODULE_AUTHOR("seantywork");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");



static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);

#define SUCCESS 0
#define DEVICE_NAME "do_list_operation_dev"
#define BUF_LEN 80

static int dev_major;
static int device_open_counter = 0;
static char msg[BUF_LEN];
static char *msg_buff_ptr;

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};


struct message_store {

    int id;
    int msg_len;
    char* msg;

};


static struct message_store msgstore;


static int __init do_list_operation_init(void)
{
    

    dev_major = register_chrdev(0, DEVICE_NAME, &fops);

    if(dev_major < 0){

        printk(KERN_ALERT "registering char device failed: %d\n", dev_major);

        return dev_major;

    }

    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, dev_major);



    printk(KERN_INFO "do_list_operation initiated\n");


	return 0; /* success */
}

static void __exit do_list_operation_exit(void)
{

    printk(KERN_INFO "do_list_operation exit\n");

}

module_init(do_list_operation_init);
module_exit(do_list_operation_exit);