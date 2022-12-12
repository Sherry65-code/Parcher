from lib import *
from re import match
print("Welcome to PARCHER")
sleep(1)
print("Checking if internet is working...")
while True:
    if testNetwork():
        print("Network is working well")
        sleep(1)
        clear()
        break
    else:
        clear()
        print("Waiting for Network as network not found. Press Ctrl+C to exit the program as you wont be able to exit the program after installation has begun")
        sleep(5)

print("Scanning disks...")
sleep(1)
disks = sys("fdisk -l")
clear()
print("Disks avaliable are :")
print(disks)
while True:
    diskname = input("Type diskname excluding the number (example - sda):")
    if "1" in diskname  or "2" in diskname  or "3" in diskname  or "4" in diskname  or "5" in diskname  or "6" in diskname  or "7" in diskname  or "8" in diskname  or "9" in diskname :
        print("No partition numbers to be included")
    elif diskname in disks :
        print("Proceeding to Other Customizations...")
        sleep(1)
        break
    else:
        print("This disk name is not found :(")
clear()
while True:
    username = input("Type your username (Exculde any special characters):")
    username = username.lower()
    if bool(match('^[a-zA-Z0-9]*$',username))==True:
        break
    else:
        print("Invalid name :(")
clear()
print("Setting up Partition...")
sleep(1)
clear()
print("Partitioning...")
print("NOTICE: Now you cant exit your installation on your own will")
try:
    countdown(5)
    criticalexe(f"(echo d; echo d; echo d; echo d; echo d; echo d; echo w;) | sudo fdisk /dev/{diskname}")
    criticalexe(f"(echo g; echo n; echo 1; echo; echo '+550M'; echo y; echo n; echo 2; echo ; echo +2G; echo y; echo n; echo 3; echo ; echo ; echo y; echo t; echo 1; echo 1; echo y; echo t; echo 2; echo 19; echo y; echo w;) | sudo fdisk /dev/{diskname}")
    criticalexe(f"mkfs.fat -F32 /dev/{diskname}1")
    criticalexe(f"mkswap /dev/{diskname}2")
    criticalexe(f"swapon /dev/{diskname}2")
    criticalexe(f"mkfs.ext4 /dev/{diskname}3")
    print("Partition has been done!")
    sleep(1)
    print("Now starting installation of main system...")
    sys(f"mount /dev/{diskname}3 /mnt")
    clear()
    print("Choose linux kernel:\n1. Normal\n2. LTS")
    while True:
        kernel = input("(1 or 2)>>")
        if (kernel == "1"):
            criticalexe("pacstrap /mnt base base-devel linux linux-firmware")
            break
        elif (kernel == "2"):
            criticalexe("pacstrap /mnt base base-devel linux-lts linux-firmware")
            break
        else:
            print("Wrong Argument. Try Again")
            sleep(1)
    clear()
    print("Creating Installation Register...")
    sleep(1)
    criticalexe("genfstab -U /mnt >> /mnt/etc/fstab")
except Exception as e:
    pass
except KeyboardInterrupt():
    print("You cant exit this program now.")