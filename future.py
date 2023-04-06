# this contains all the installation code after arch-chroot /mnt

# this is a standalone file which will do everything itself so all the code for this file is present in it only.

from os import system
from time import sleep
# get all timezones

system("timedatectl list-timezones > timelist.txt")

# search functionality for this

while True:
    fun1 = input("Type s to search and e to directly enter timezone:")
    if fun1 == 's':
        # search now [Type Here O-]
        searchprompt = input("Type timezone to search for:")
        system(f"cat timelist.txt | grep {searchprompt}")
    elif fun1 == 'e':
        # try to use
        zone = input("Type Timezone:")
        if system(f"ln -sf /usr/share/zoneinfo/{zone} /etc/localtime") == 0:
            print(f"Timezone verified and timezone has been set to {zone}")
            break
        else:
            print("Timezone you entered is incorrect. Try Again.")
            sleep(1)

# Then we have to set the hardware clock
print("Setting Hardware clock...")
system("hwclock --systohc")
print("DONE setting hardware clock")
sleep(1)
# Setting locale-gen - {}{}{}{}{}{}This part needs development afterwards
print("Setting Locale to en_IN. You can change it afterwards by editting /etc/locale.gen and then run the command locale-gen")
if system("echo 'en_IN UTF-8' > /etc/locale.gen") == 0:
    pass
else:
    system("sudo (echo 'en_IN UTF-8' > /etc/locale.gen)")

# Now comes setting hostname

while True:
    hostname = input("Type you PC name you want to keep:")
    if hostname == '':
        print("Please enter you pc name you wanna keep")
    else:
        break
if system(f"echo '{hostname}' > /etc/hostname") == 0:
    print(f"Hostname set to {hostname}")
else:
    print("Hostname was not able to set, Unfortunately!")

# then set root password

print("Now type your root password you want to keep. Remember, Its not your username")
while True:
    if system("passwd") == 0:
        break
    else:
        print("Hostname was not setup\nPlease try again.")

# then create new user

print("Type your username now:",end="")
username = input()
opt1 = input("Do you want to create a home directory for your user? For Beginners it is recommended to create home directory (Y/n):")
if op1 == 'n':
    print("Continuing without Home Directory...")
    system(f"useradd {username}")
else:
    print("Creating Home Directory with user...")
    system(f"useradd -m {username}")

print("NOW SET PASSWORD FOR YOURSELF")
system("passwd {username}")

print("Continuing to sudo install...")
system("pacman -S sudo --noconfirm")

print("Adding user to wheel group")
system(f"usermod -aG wheel,video,audio,optical,storage {username}")

print("GIVING user sudo privelleges...")
system("echo 'root ALL=(ALL:ALL) ALL' > /etc/sudoers")
system("echo 'wheel ALL=(ALL:ALL) ALL' >> /etc/sudoers")
system("echo '@includedir /etc/sudoers.d' >> /etc/sudoers")

# Adding Network Configuration

print("ADDING NETWORK CONFIGURATION...")
system("echo '127.0.0.1    localhost' >> /etc/hosts")
system("echo '::1          localhost' >> /etc/hosts")
system(f"echo '127.0.1.1    {hostname}.localdomain    {hostname}' >> /etc/hosts")

# setup UEFI Partition
print("SETTING UP UEFI PARTITION...")
f = open("diskname.txt", "r")
fdata = f.read()
f.close()
fdata = fdata.replace(" ", "")
system("mount --mkdir /dev/{fdata} /boot/EFI")

# Now bootloader install
print("Installing GRUB...")
system("sudo pacman -S grub efibootmgr dosfstools mtools os-prober --noconfirm")
if system("""grub-install --target=x86_64-efi --bootloader-id=grub_uefi --modules="tpm" --disable-shim-lock --recheck""") == 0:
    pass
else:
    system("grub-install --target=x86_64-efi --bootloader-id=grub_uefi --recheck")

print("GENERATING BOOT CONFIGURATION FILE FOR GRUB...")
system("grub-mkconfig -o /boot/grub/grub.cfg")

# Now installing network manager
print("INSTALLING NETWORK MANAGER...")
system("sudo pacman -S networkmanager --noconfirm")

print("NOW SETTING UP NETWORK MANAGER...")
system("sudo systemctl enable NetworkManager")

# Now installing user desktop manager

system("clear")
print("NOW SPECIFY A DESKTOP ENVIROMENT")
print("1. GNOME\n2. KDE Plasma\n3. XFCE 4\n4. OpenBox\n5. None")
in1 = input(">>")
if in1 == '1':
    print("Installing GNOME...")
    system("sudo pacman -S xorg xorg-server gnome gdm --noconfirm")
    system("sudo systemctl enable gdm.service")
    print("DONE")
elif in1 == '2':
    print("Installing KDE...")
    opt2 = input("Which type of installation do you want?\n1.Minimal\n2.Full\n:::")
    if opt2 == '1':
        # Minimal install
        system("pacman -S xorg plasma plasma-meta dolphin sddm konsole --noconfirm")
    else:
        system("pacman -S xorg plasma plasma-meta kde-applications --noconfirm")
    system("sudo systemctl enable sddm.service")
elif in1 == '3':
    print("Installing XFCE 4...")
    system("sudo pacman -S xfce4 xfce4-goodies lightdm xorg --noconfirm")
    system("sudo systemctl enable sddm")
elif in1 == '4':
    print("Installing OpenBox...")
    system("sudo pacma -S openbox lightdm xorg --noconfirm")
else:
    print("Skipping Desktop Enviroment Install completely")
    
sleep(1)
