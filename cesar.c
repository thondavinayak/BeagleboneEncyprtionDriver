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
static char encrypted_message = '\0';

static ssize_t caesarRead(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t caesarWrite(struct file *file, const char *buf, size_t count, loff_t *ppos);
static int caesarOpen(struct inode *inode, struct file *file);
static int caesarRelease(struct inode *inode, struct file *file);

/* File operations */
static struct file_operations caesar_ops = {
    .open = caesarOpen,
    .read = caesarRead,
    .write = caesarWrite,
    .release = caesarRelease,
};

/* Caesar cipher function (handles both upper and lower case letters) */
static char encrypt_char(char c, int k)
{
    if (c >= 'A' && c <= 'Z') {
        return 'A' + ( (c - 'A' + k) % 26 );
    } else if (c >= 'a' && c <= 'z') {
        return 'a' + ( (c - 'a' + k) % 26 );
    } else {
        return c;  // non alpha characters are skipped
    }
}

/* Module load/unload functions */
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

/* File operation handlers */

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
    char kbuf;

    if (copy_from_user(&kbuf, buf, 1)) {
        return -EFAULT;
    }

    printk(KERN_ALERT "caesarWrite() - Received char: %c\n", kbuf);

    if (kbuf >= '0' && kbuf <= '9') {
        key = kbuf - '0';
        printk(KERN_ALERT "caesarWrite() - Key set to %d\n", key);
    } else if ((kbuf >= 'A' && kbuf <= 'Z') || (kbuf >= 'a' && kbuf <= 'z')) {
        encrypted_message = encrypt_char(kbuf, key);
        printk(KERN_ALERT "caesarWrite() - Encrypted message Write: %c\n", encrypted_message);
    } else {
        printk(KERN_ALERT "caesarWrite() - Invalid input, ignored\n");
    }

    return count;
}

static ssize_t caesarRead(struct file *file, char *buf, size_t count, loff_t *ppos) {
    if (*ppos > 0) {
        return 0;
    }

    char message[2] = { encrypted_message, '\n' };

    //if (copy_to_user(buf, &encrypted_message, 1)) {
    if (copy_to_user(buf, &message, 2)) {
        return -EFAULT;
    }

    *ppos += 2;
    printk(KERN_ALERT "caesarRead() - Read encrypted message: %c\n", encrypted_message);
    return 2;
}
