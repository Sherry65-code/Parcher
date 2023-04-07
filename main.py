# Main File

from linux import *
from sys import exit

# All Variables
username = ""
hostname = ""
kernel = ""
disk = ""
# All Variables End

clear()

# Checking for internet
while True:
    if checkNet():
        print("\rNetwork Found !")
        break
    else:
        print("\rWaiting for network...",end="")

# Then UEFI Check

if isUEFI():
    print("Continuing with UEFI")
else:
    print("We donot support BIOS yet")
    exit()

# Then Choose a Disk

print("CHOOSE A DISK FROM BELOW")
system("lsblk | grep sd")
system("lsblk | grep nvme")
system("lsblk | grep mmcblk")
while True:
    disk = input("Type Disk name:")
    if verifyBootName(disk):
        print("Continuing for Partition stage...")
        break
    else:
        print("Wrong Disk, Try Again")

system("parted /dev/%s --script mktable gpt \
mkpart primary fat32 1MiB 551MiB \
set 1 esp on \
mkpart primary linux-swap 551MiB 2551MiB \
mkpart primary ext4 2551MiB 100%%")
clear()
print("Formatting Drive...")
system(f"mkfs.fat -F32 /dev/{disk}1")
system(f"mkswap /dev/{disk}2")
system(f"swapon /dev/{disk}2")
system(f"mkfs.ext4 /dev/{disk}3")

print("Disk Formatted")
sleep(1)
clear()
username = input("Type your username:")
hostname = input("Type your hostname:")
print("WHICH LINUX KERNEL DO YOU WANT")
print("1. Linux\n2. Linux LTS\n3. Linux ZEN")
op = input(">>")
if op == "2":
    kernel = "linux-lts"
elif op == "3":
    kernel = "linux-zen"
else:
    kernel = "linux"
print("Installing for LINUX")
system("timedatectl list-timezones > time.txt")
f = open("time.txt", "r")
fdata = f.read()
f.close()
system(f"mount /dev/{disk}3 /mnt")
print("Installing Base System...")
system(f"pacstrap -K /mnt base base-devel {kernel} linux-firmware")
print("Creating fstab...")
system("genfstab -U /mnt >> /mnt/etc/fstab")
