# this contains the main file 

from lib import *

# first work -> check if internet is avaliable

clear()

while True:
    print("\rCheck if internet connection persists...", end="")
    if check_net():
        print("\rInternet Connection Avaliablity is Verified !")
        break
    else:
        print("\rWaiting for connection...                          ", end="")
    sleep(1)

# second -> print all the drives and the the user to choose any one
# They can be of 3 types -> /dev/sda, /dev/nvme0n1 or /dev/mmcblk0

print("========CHOOSE A HARD DISK TO INSTALL SYSTEM ON=========")
displaydisks()
while True:
    disk = input("Type disk name (sda) >>")
    if partition(disk):
        # Continue Partition
        disk = disk.replace("/","").replace("\\", "").replace("dev","")
        # then check for system boot type (UEFI or BIOS)
        if bootname() == "EFI":
            # UEFI Stuff here
            command = f"parted /dev/{disk} --script mktable gpt \
mkpart primary fat32 1MiB 551MiB \
set 1 esp on \
mkpart primary linux-swap 551MiB 2551MiB \
mkpart primary ext4 2551MiB 100%"
            system(command)
            # Then format the disks
            system(f"mkfs.fat -F32 /dev/{disk}1")
            system(f"mkswap /dev/{disk}2")
            system(f"swapon /dev/{disk}2")
            system(f"mkfs.ext4 /dev/{disk}3")
            # Mounting
            cmds = f"mount /dev/{disk}3 /mnt"
            system(cmds)
            # After Partiotin COntinue
        else:
            #BIOS Stuff here
            command = f"parted /dev/{disk} --script mktable msdos \
mkpart primary ext4 1MiB 551MiB \
set 1 boot on \
mkpart primary linux-swap 551MiB 2551MiB \
mkpart primary ext4 2551MiB 100%"
            system(command)
            system(f"mkfs.fat -F32 /dev/{disk}1")
            system(f"mkswap /dev/{disk}2")
            system(f"mkfs.ext4 /dev/{disk}3")
            cmds = f"mount /dev/{disk}3 /mnt"
            system(cmds)
        # Then Continue to common part
        print("Partition Completed Sucessfully :)") 

        # Then exit out of the Partiton Loop
        break
    else:
        pass

# Now prompt user for choosing a linux kernel

clear()

print("==========BASE=SYSTEM=INSTALL==============")
print("OPTIONS FOR LINUX KERNEL:")
print("1. Linux\n2. Linux LTS\n3. Linux ZEN")
while True:
    kernel = input("Choose any one kernel:")
    if kernel == "1" or kernel.lower().replace(" ", "") == "linux":
        # Chose Linux
        kerneltype = "linux"
        break
    elif kernel == "2" or kernel.lower().replace(" ", "").replace("-","") == "linuxlts":
        # Chose Linux LTS
        kerneltype = "linux-lts"
        break
    elif kernel == "3" or kernel.lower().replace(" ", "").replace("-","") == "linuxzen":
        # Chose Linux Zen
        kerneltype = "linux-zen"
        break
    elif kernel.replace(" ","") == "":
        # default user case - linux
        kerneltype = "linux"
        break
    else:
        # wrong input
        print("Wrong Option, try again!")

sleep(1)
clear()
print("Installing Base Packages...")
print(f"PACKAGES LIST\n{kerneltype}\nlinux-firmware\nbase\nbase-devel")
if system(f"pacstrap -K /mnt base base-devel {kerneltype} linux-firmware") == 0:
    clear()
    print("Installed Base system")
    sleep(1)
    print("Creating genfstab")
    system("genfstab -U /mnt >> /mnt/etc/fstab")
    print("Created genfstab")
    sleep(1)
    clear()
    print("Preparing to CHROOT into /mnt...")
    # Now need to transfer all the load after chrooting to arch-chroot /mnt
    # First transfer future.py file to /mnt
    f = open("bootdrive.txt", "w")
    f.write(f"{disk}")
    f.close()
    system("cp ./bootdrive.txt /mnt/")
    if system("cp ./future.py /mnt/") == 0:
        print("File Transfer sucessful to /mnt")
        sleep(1)
        print("Now transfering control to /mnt/future.py")
        # Then go chroot and run this command in chroot
        system("arch-chroot /mnt | python3 /mnt/future.py")
        # Getting Control back
        # Now first unmount all disks and prompt userthat the system will reboot
        system("umount -R /mnt")
        print("Your system is going down for reboot in 5s. Press Ctrl+C to cancel reboot")
        sleep(5)
        clear()
        print("HAPPY ARCH :)")
        sleep(2)
        system("reboot")
else:
    print("Kernel Installation Failed.\nTry mounting /mnt by typing mount /dev/sda3\nThen if its sucessful you can type this command to continue -> ~/run.sh\n Please troubleshoot the issue or post the issue on github at Sherry65-code/Parcher")

