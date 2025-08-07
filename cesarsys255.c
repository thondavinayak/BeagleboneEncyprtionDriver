#define CAESAR_DEV_NAME "caesar"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/fs.h>
#include <linux/uaccess.h> 
#include <linux/sysfs.h>   
#include <linux/kobject.h> 
#include <linux/string.h>  

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Caesar cipher module with sysfs interface");
MODULE_AUTHOR("vinayak koti");

static int key = 0;
static char encrypted_message[255] = {0};  
static struct kobject *caesar_kobj;

static ssize_t key_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t key_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
static ssize_t encrypted_message_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t encrypted_message_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobj_attribute key_attr = __ATTR(key, 0660, key_show, key_store);
static struct kobj_attribute encrypted_message_attr = __ATTR(encrypted_message, 0660, encrypted_message_show, encrypted_message_store);

static ssize_t key_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", key);
}

static ssize_t key_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    if (sscanf(buf, "%d", &key) == 1) {
        printk(KERN_ALERT "caesar: Key set to %d\n", key);
    }
    return count;
}

static ssize_t encrypted_message_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s\n", encrypted_message);
}

static ssize_t encrypted_message_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int i;

    for (i = 0; i < count && i < 255 && buf[i] != '\0'; i++) {
        encrypted_message[i] = encrypt_char(buf[i], key);
    }

    encrypted_message[i] = '\0'; 
    printk(KERN_ALERT "caesar: Encrypted message: %s\n", encrypted_message);
    return count;
}

static char encrypt_char(char c, int k) {
    if (c >= 32 && c <= 126) {  
        return 32 + ((c - 32 + k) % 95);  
    } else {
        return c;  
    }
}

static int __init caesar_init(void)
{
    int retval;

    printk(KERN_ALERT "caesar Loading ...\n");

    caesar_kobj = kobject_create_and_add(CAESAR_DEV_NAME, kernel_kobj);
    if (!caesar_kobj)
        return -ENOMEM;

    retval = sysfs_create_file(caesar_kobj, &key_attr.attr);
    if (retval)
        kobject_put(caesar_kobj);

    retval = sysfs_create_file(caesar_kobj, &encrypted_message_attr.attr);
    if (retval)
        kobject_put(caesar_kobj);

    return retval;
}

static void __exit caesar_exit(void)
{
    printk(KERN_ALERT "caesar Unloading ...\n");

    kobject_put(caesar_kobj);
}

module_init(caesar_init);
module_exit(caesar_exit);
