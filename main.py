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
