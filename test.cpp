#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>

using namespace std;

string read_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: could not open file '" << filename << "'" << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
    file.close();
    return content;
}

void clear() {
    system("clear");
}

bool isInternetAvailable() {
    return system("ping google.com -c 1 &> /dev/null") == 0;
}

bool isUEFI() {
    return system("ls /sys/firmware/efi/efivars &> /dev/null") == 0;
}

bool verifyBootName(const string& bootname) {
    system("lsblk --nodeps -o NAME > disks.txt");
    string drivelist = read_file("disks.txt");
    if (drivelist.empty()) {
        cerr << "Error: could not read disk list from file" << endl;
        return false;
    }
    return drivelist.find(bootname) != string::npos;
}

struct UserInfo {
    string name;
    string hostname;
    string kernel;
    string disk;
    bool isSecureBoot;
};

void clearString(char* str, int size) {
    memset(str, 0, size);
}

int main() {
    // Initialize struct
    // I used struct to separate variable from main
    // All the variables declared in main are temporary and not to be used afterwards
    UserInfo user;

    char cmd[300];
    char cmd2[300];
    char cmd3[300];
    int kchoice;
    char op1;
    // Clear screen
    clear();
    // Test for internet connection
    // If connection not found then wait for it.
    cout << "\rChecking if Internet Connection persists...";
    while (!isInternetAvailable()) {
        cout << "\rWaiting for internet connection...            ";
        sleep(1);
    }
    cout << "\rInternet Connection Verified !                   " << endl;
    sleep(1);
    // Then check if the system is UEFI or BIOS
    if (isUEFI()) {
        cout << "Starting Installation for UEFI..." << endl;
    }
    else {
        cout << "BIOS is yet not supported for installing. Come Later. :(" << endl;
        return 0;
    }
    sleep(1);
    // First display disks
    cout << "CHOOSE A DISK FOR BELOW" << endl;
    system("lsblk | grep sd");
    system("lsblk | grep nvme");
    system("lsblk | grep mmcblk");
    while (true) {
        cout << "Type disk name (sda): ";
        cin >> user.disk;
        if (verifyBootName(user.disk)) {
            break;
        }
        else {
            cout << "Wrong Boot Drive. Try Again !" << endl;
        }
    }
    cout << "Partitioning " << user.disk << endl;
    sleep(1);
    // Partition Code
    clearString(cmd, sizeof(cmd));
    sprintf(cmd, "parted /dev/%s --script mktable gpt \
mkpart primary fat32 1MiB 551MiB \
set 1 esp on \
mkpart primary linux-swap 551MiB 2551MiB \
mkpart primary ext4 2551MiB 100%%", user.disk.c_str());
    // Apply Partition
    system(cmd);
    // Then format drive
    clearString(cmd, sizeof(cmd));
    sprintf(cmd, "mkfs.fat -F32 /dev/%s1", user.disk.c_str());
    system(cmd);
    clearString(cmd, sizeof(cmd));
    sprintf(cmd, "mkswap /dev/%s2", user.disk.c_str());
    system(cmd);
    clearString(cmd, sizeof(cmd));
    sprintf(cmd, "swapon /dev/%s2", user.disk.c_str());
    system(cmd);
    clearString(cmd, sizeof(cmd));
    sprintf(cmd, "mkfs.ext4 /dev/%s3", user.disk.c_str());
    system(cmd);
    clearString(cmd, sizeof(cmd));
    // After formatting Mount drive
    cout << "Mounting " << user.disk << endl;
    sprintf(cmd2, "mount /dev/%s3 /mnt", user.disk.c_str());
    system(cmd2);
    clearString(cmd, sizeof(cmd));
    cout << "Mounted " << user.disk << endl;
    sleep(1);
    clear();
    cout << "Select a Linux Kernel" << endl;
    cout << "1. Linux" << endl;
    cout << "2. Linux LTS" << endl;
    cout << "3. Linux Zen" << endl;
cin >> kchoice;
if (kchoice == 1) {
    user.kernel = "linux";
}
else if (kchoice == 2) {
    user.kernel = "linux-lts";
}
else if (kchoice == 3) {
    user.kernel = "linux-zen";
}
clear();
cout << "INSTALLING BASE SYSTEM" << endl;
clearString(cmd, sizeof(cmd));
sprintf(cmd, "pacstrap /mnt base %s linux-firmware base-devel", user.kernel.c_str());
sleep(1);
clearString(cmd, sizeof(cmd));
sprintf(cmd, "pacstrap -K /mnt base base-devel linux-firmware %s", user.kernel.c_str());
system(cmd);
cout << "Creating fstab..." << endl;
system("genfstab -U /mnt >> /mnt/etc/fstab");
clear();
cout << "Installed Base System" << endl;
sleep(1);
clear();
cout << "TIMEZONE" << endl;
string timelist = read_file("timelist.txt");
if (timelist.empty()) {
    cerr << "Error: could not read timezone list from file" << endl;
    return 0;
}
while (true) {
    cout << "Type s to search for timezone and e to enter timezone:";
    cin >> op1;
    if (op1 == 's') {
        // Search
        cout << "Type term to search:";
        cin >> cmd2;
        clearString(cmd, sizeof(cmd));
        sprintf(cmd, "cat timelist.txt | grep %s", cmd2);
        system(cmd);
    }
    else if (op1 == 'e') {
        // Directly Edit
        clearString(cmd, sizeof(cmd));
        cout << "Type timezone Continent/Region :";
        cin >> cmd2;
        sprintf(cmd, "arch-chroot /mnt ln -sf /usr/share/zoneinfo/%s /etc/localtime", cmd2);
        if (system(cmd) == 0) {
            break;
        }
        else {
            cout << "The Timezone you entered is incorrect. Try Again !" << endl;
        }
    }
}
cout << "Timezone set to " << cmd2 << " sucessfully" << endl;
sleep(1);
clear();
cout << "Setting Hardware Clock to current time..." << endl;
system("arch-chroot /mnt hwclock --systohc");
cout << "Hardware Clock set sucessfully" << endl;
sleep(1);
cout << "Setting locale to 'en_IN UTF-8'" << endl;
system("arch-chroot /mnt echo 'en_IN UTF-8' >> /etc/locale.gen");
system("arch-chroot /mnt locale-gen");
cout << "Locale Set" << endl;
cout << "Type the name of the machine you want to keep:";
cin >> user.hostname;
clearString(cmd2, sizeof(cmd2));
sprintf(cmd2, "arch-chroot /mnt echo '%s' > /etc/hostname", user.hostname.c_str());
system(cmd2);
cout << "Type password for root user:";
system("arch-chroot /mnt passwd");
clear();
cout << "Now setting up network services..." << endl;
system("arch-chroot /mnt echo '127.0.0.1    localhost' >> /etc/hosts");
system("arch-chroot /mnt echo '::1          localhost' >> /etc/hosts");
clearString(cmd, sizeof(cmd));
sprintf(cmd, "arch-chroot /mnt echo '127.0.1.1    %s.localdomain    %s' > /etc/hosts", user.hostname.c_str(), user.hostname.c_str());
system(cmd);
cout << "Type your username:";
cin >> user.name;
clearString(cmd, sizeof(cmd));
sprintf(cmd, "arch-chroot /mnt useradd -m %s", user.name.c_str());
system(cmd);
cout << "Now type password for " << user.name << ":";
clearString(cmd, sizeof(cmd));
sprintf(cmd, "arch-chroot /mnt passwd %s", user.name.c_str());
system(cmd);
cout << "Installing sudo..." << endl;
system("arch-chroot /mnt pacman -S sudo --noconfirm");
clear();
cout << "Adding " << user.name << " to wheel group..." << endl;
clearString(cmd, sizeof(cmd));
sprintf(cmd, "arch-chroot /mnt usermod -aG wheel,video,audio,optical,storage %s", user.name.c_str());
system(cmd);
cout << "Giving " << user.name << " user privelleges..." << endl;
system("arch-chroot /mnt echo '%wheel ALL=(ALL) ALL' | EDITOR='tee -a' visudo");
cout << "User privelleges set" << endl; sleep(1); clear(); cout << "Installing GRUB Bootloader…" << endl; cout << "Does your system support Secure Boot (y/N):"; cin >> op1;
user.isSecureBoot = (op1 == 'y');
if (user.isSecureBoot) {
clearString(cmd, sizeof(cmd));
sprintf(cmd, "arch-chroot /mnt grub-install --target=x86_64-efi --bootloader-id=grub_uefi --modules='tpm' --disable-shim-lock --recheck");
if (system(cmd) == 0) {
cout << "Generating Grub Configuration file…" << endl; } else { clearString(cmd, sizeof(cmd)); sprintf(cmd, "arch-chroot /mnt grub-install --target=x86_64-efi --bootloader-id=grub_uefi --recheck"); system(cmd); } } else { clearString(cmd, sizeof(cmd)); sprintf(cmd, "arch-chroot /mnt grub-install --target=x86_64-efi --bootloader-id=grub_uefi --recheck"); system(cmd); cout << "Generating Grub Configuration file…" << endl; } system("arch-chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg"); cout << "Bootloader Installed Sucessfully" << endl; sleep(1); clear(); cout << "Installing Network Manager…" << endl; system("arch-chroot /mnt pacman -S networkmanager --noconfirm"); system("arch-chroot /mnt systemctl enable NetworkManager"); cout << "Network Manager Installed" << endl; sleep(1); clear(); cout << "CHOOSE A DESKTOP ENVIROMENT TO INSTALL" << endl; cout << "1. GNOME" << endl; cout << "2. KDE Plasma" << endl; cout << "3. XFCE" << endl; cout << "4. OpenBox" << endl; cout << "5. None" << endl; while (true) { cout << ">>";
cin >> op1;
if (op1 == '1') {
// GNOME
system("arch-chroot /mnt pacman -S xorg xorg-server gnome gdm --noconfirm");
system("arch-chroot /mnt systemctl enable gdm");
break;
}
else if (op1 == '2') {
// KDE Plasma
system("arch-chroot /mnt pacman -S xorg plasma plasma-meta doplhin konsole sddm --noconfirm");
system("arch-chroot /mnt systemctl enable sddm");
break;
}
else if (op1 == '3') {
// XFCE
system("arch-chroot /mnt pacman -S install xfce4 xfce4-goodies lightdm xorg --noconfirm");
system("arch-chroot /mnt systemctl enable lightdm");
break;
}
else if (op1 == '4') {
// Openbox
system("arch-chroot /mnt pacman -S openbox lightdm xorg --noconfirm");
system("arch-chroot /mnt systemctl enable lightdm");
break;
}
else if (op1 == '5') {
// None
break;
}
else {
cout << "Wrong Choice, choice doesnt exist :(" << endl;
}
}
clear();
cout << "Installation Completed Sucessfully" << endl;
cout << "Happy Arch :)" << endl;
cout << "Rebooting in 5 seconds…" << endl;
system("umount -R /mnt");
sleep(5);
system("systemctl reboot");
return 0;
}