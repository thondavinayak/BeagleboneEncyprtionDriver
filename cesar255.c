/* Caesar cipher Kernel module with /dev interface
Before loading, need to create the /dev entry with:
sudo mknod /dev/caesar c 61 0
*/

#define CAESAR_DEV_NAME "caesar"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> // printk
#include <linux/fs.h>
#include <linux/uaccess.h> /* copy_from/to_user */

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Caesar cipher module with /dev interface");
MODULE_AUTHOR("vinayak koti");
MODULE_SUPPORTED_DEVICE(CAESAR_DEV_NAME);

static int major = 61;

static int key = 0;
static char encrypted_message[255] = {0};  

static ssize_t caesarRead(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t caesarWrite(struct file *file, const char *buf, size_t count, loff_t *ppos);
static int caesarOpen(struct inode *inode, struct file *file);
static int caesarRelease(struct inode *inode, struct file *file);

static struct file_operations caesar_ops = {
    .open = caesarOpen,
    .read = caesarRead,
    .write = caesarWrite,
    .release = caesarRelease,
};


static char encrypt_char(char c, int k) {
    if (c >= 32 && c <= 126) {  
        return 32 + ((c - 32 + k) % 95);  
    } else {
        return c;  
    }
}

static int __init caesar_init(void)
{
    printk(KERN_ALERT "caesar Loading ...\n");
    register_chrdev(major, CAESAR_DEV_NAME, &caesar_ops);
    printk(KERN_ALERT "MAJOR = %d\n", major);
    return 0;
}

static void __exit caesar_exit(void)
{
    printk(KERN_ALERT "caesar Unloading ...\n");
    unregister_chrdev(major, CAESAR_DEV_NAME);
}

module_init(caesar_init);
module_exit(caesar_exit);


static int caesarOpen(struct inode *inode, struct file *file)
{
    printk(KERN_ALERT "caesarOpen() - Device /dev/caesar opened\n");
    return 0;
}

static int caesarRelease(struct inode *inode, struct file *file)
{
    printk(KERN_ALERT "caesarRelease() - Device /dev/caesar closed\n");
    return 0;
}

static ssize_t caesarWrite(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    char kbuf[255];
    int i;

    if (count > 255)
        count = 255;

    if (copy_from_user(kbuf, buf, count)) {
        return -EFAULT;
    }

    for (i = 0; i < count; i++) {
        if (*ppos == 0 && i == 0 && kbuf[i] >= '0' && kbuf[i] <= '9') {
            key = kbuf[i] - '0';
            printk(KERN_ALERT "caesarWrite() - Key set to %d\n", key);
            continue;
        }

        encrypted_message[i] = encrypt_char(kbuf[i], key);
        printk(KERN_ALERT "caesarWrite() - Encrypted char: %c\n", encrypted_message[i]);
    }

    //if (i < 255)
        //encrypted_message[i] = '\0';

    return count;
}


static ssize_t caesarRead(struct file *file, char *buf, size_t count, loff_t *ppos) {
    char message[255];
    size_t len;

    if (*ppos > 0) {
        return 0;  
    }

    len = strlen(encrypted_message);

    memcpy(message, encrypted_message, len);

    //message[len] = '\n';
    message[len + 1] = '\0';

    if (copy_to_user(buf, message, len + 1)) {
        return -EFAULT;
    }

    *ppos += len + 1;
    printk(KERN_ALERT "caesarRead() - Read encrypted message\n");
    return len + 1;
}
