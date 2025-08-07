# BeagleboneEncyprtionDriver
Encryption Kernel Module – BeagleBone

This repository contains kernel modules that implement Caesar cipher encryption via different Linux kernel interfaces on the BeagleBone platform. The modules support reading and writing encrypted messages through /dev and sysfs.

🛠️ Features
Multiple Caesar cipher kernel module variants:

Single character encryption via /dev/caesar (cesar.c)

255-character message encryption via /dev/caesar (cesar255.c)

255-character encryption using /sys interface (cesarsys255.c)

Supports custom encryption keys set by user input

Demonstrates:

Character device handling

Sysfs integration

Secure user-kernel communication

Embedded Linux driver development and Makefile-based builds

📂 File Structure
File	Description
cesar.c	Minimal Caesar cipher kernel module; encrypts a single character via /dev/caesar.
cesar255.c	Enhanced version that encrypts up to 255 characters via /dev/caesar.
cesarsys255.c	Advanced version with sysfs attributes /sys/caesar/key and /sys/caesar/encrypted_message.
Makefile	Builds the appropriate kernel module for your BeagleBone Linux kernel.

⚙️ Build & Installation
🧰 Prerequisites
BeagleBone with Embedded Linux (e.g. Debian or Angstrom)

Kernel headers or build environment installed (linux-headers)

make and gcc installed on the host or target

🏗️ Build the Module
bash
Copy
Edit
make
This will compile all .c files into kernel object .ko modules.

📦 Load a Module
Example: load the single-character version

sudo insmod cesar.ko
sudo mknod /dev/caesar c 61 0
For the 255-character version:

sudo insmod cesar255.ko
sudo mknod /dev/caesar c 61 0
For the sysfs version:

sudo insmod cesarsys255.ko
🧪 Usage Examples
1. cesar.c (Single char via /dev/caesar)

echo -n 3 > /dev/caesar       # Set encryption key to 3
echo -n A > /dev/caesar       # Encrypt character 'A' with key 3
cat /dev/caesar               # Output: 'D'
2. cesar255.c (String encryption via /dev/caesar)

echo -n "3HelloWorld" > /dev/caesar   # First character '3' is key
cat /dev/caesar                       # Encrypted output
3. cesarsys255.c (String encryption via /sys)

echo 5 > /sys/caesar/key
echo "BeagleBoneLinux" > /sys/caesar/encrypted_message
cat /sys/caesar/encrypted_message     # View encrypted string
🔁 Variants & Differences
Variant	Interface	Max Msg Length	Key Input	Use Case
cesar.c	/dev	1 char	Written before message	Educational, simple example
cesar255.c	/dev	255 chars	First char in write buffer	More practical string use
cesarsys255.c	/sys	255 chars	/sys/caesar/key	Modern sysfs-based driver

🧹 Unload Module
sudo rmmod cesar         # or cesar255, cesarsys255
sudo rm /dev/caesar
