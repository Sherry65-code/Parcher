#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int isInternetAvaliable();
int isUEFI();
void clear();
int verifyBootName(char *bootname);
char* read_file(const char *filename);
void clearString(char str[], int size);

char* read_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return NULL;
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    // Allocate memory for buffer
    char* buffer = (char*) malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error: could not allocate memory for file '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    // Read file into buffer
    size_t bytes_read = fread(buffer, sizeof(char), file_size, fp);
    if (bytes_read < file_size) {
        fprintf(stderr, "Error: could not read file '%s'\n", filename);
        fclose(fp);
        free(buffer);
        return NULL;
    }
    buffer[bytes_read] = '\0';

    // Close file and return buffer
    fclose(fp);
    return buffer;
}

void clear() {
	system("clear");
}

int isInternetAvaliable() {
	if (system("ping google.com -c 1 &> /dev/null") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int isUEFI() {
	if (system("ls /sys/firmware/efi/efivars &> /dev/null") == 0) {
		return 1;
	}
	else {
		return 0;
	}
}
int verifyBootName(char *bootname) {
	system("lsblk --nodeps -o NAME > disks.txt");
	char *drivelist = read_file("disks.txt");
	if (drivelist == NULL) {
		printf("Error: could not read disk list from file\n");
		return 0;
	}
	if (strstr(drivelist, bootname) != NULL) {
		free(drivelist);
		return 1;
	}
	else {
		free(drivelist);
		return 0;
	}
}	
struct UserInfo {
	char *name;
	char *hostname;
	char **kernel;
	char *disk;
	char isSecureBoot;
};

void clearString(char str[], int size) {
	for (int i = 0;i<size;i++) {
		str[i] = '\0';
	}
}
int main(int argc, char *argv) {
	// Initialize struct
	// I used struct to seperate variable from main
	// All the variables declared in main are temporary and not to be used afterwards
	struct UserInfo user;

	char cmd[300];
	char cmd2[300];
	char cmd3[300];
	int kchoice;
	char op1;	
	// Clear screen
	clear();
	// Test for internet connection
	// If connection not found then wait for it.
	printf("\rChecking if Internet Connection persists...");
	while (1) {
   		if (isInternetAvaliable()) {
			printf("\rInternet Connection Verified !                   \n");
			break;
		}
		else {
			printf("\rWaiting for internet connection...            ");
		}	
	}
	sleep(1);
	// Then check if the system is UEFI or BIOS
	if (isUEFI()) {
		printf("Starting Installation for UEFI...\n");
	}
	else {
		printf("BIOS is yet not supported for installing. Come Later. :(\n");
	}
	sleep(1);
	// First display disks
	printf("CHOOSE A DISK FOR BELOW\n");
	system("lsblk | grep sd");
	system("lsblk | grep nvme");
	system("lsblk | grep mmcblk");
	while (1) {
		printf("Type disk name (sda):");
		scanf("%s", &user.disk);
		if (user.disk == NULL) {
		printf("Error: disk name not set\n");
			return 1;
		}

		if (verifyBootName(&(*(user.disk)))) {
			break;
		}
		else {
			printf("Wrong Boot Drive. Try Again !\n");
		}
	}
	printf("Partitioning %s\n", *(user.disk));
	sleep(1);
	// Partition Code
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "parted /dev/%s --script mktable gpt \
mkpart primary fat32 1MiB 551MiB \
set 1 esp on \
mkpart primary linux-swap 551MiB 2551MiB \
mkpart primary ext4 2551MiB 100%%", *(user.disk));
    // Apply Partition
	system(cmd);
	// Then format drive
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "mkfs.fat -F32 /dev/%s1", *(user.disk));
	system(cmd);
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "mkswap /dev/%s2", *(user.disk));
	system(cmd);
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "swapon /dev/%s2", *(user.disk));
	system(cmd);
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "mkfs.ext4 /dev/%s3", *(user.disk));
	system(cmd);
	clearString(cmd, sizeof(cmd));
	// After formatting Mount drive
	printf("Mounting %s", *(user.disk));
	sprintf(cmd2, "mount /dev/%s3 /mnt", *(user.disk));
	system(cmd2);
	clearString(cmd, sizeof(cmd));
	printf("Mounted %s", *(user.disk));
	sleep(1);
	clear();
	printf("Select a Linux Kernel\n1. Linux\n2. Linux LTS\n3.Linux Zen\n(1)>>");
	scanf("%d", &kchoice);
	if (kchoice == 1) {
   		*(user.kernel) = "linux";
	}
	else if (kchoice == 2) {
   		*(user.kernel) = "linux-lts";
	}
	else if (kchoice == 3) {
   		*(user.kernel) = "linux-zen";
	}

	clear();
	printf("INSTALLING BASE SYSTEM\n");
	clearString(cmd, sizeof(cmd));
	sprintf(cmd3, "pacstrap /mnt base %s linux-firmware base-devel", user.kernel);
	sleep(1);
	clearString(cmd, sizeof(cmd));
	sprintf(cmd3, "pacstrap -K /mnt base base-devel linux-firmware %s", user.kernel);
   	system(cmd3);
	printf("Creating fstab...\n");
	system("genfstab -U /mnt >> /mnt/etc/fstab");
	clear();
	printf("Installed Base System");	
	sleep(1);
	clear();
	printf("TIMEZONE\n");
	system("timedatectl list-timezones > timelist.txt");
	while (1) {
		printf("Type s to search for timezone and e to enter timezone:");
		scanf(" %c", &op1);
		if (op1 == 's') {
			// Search
			printf("Type term to search:");
			clearString(cmd2, sizeof(cmd2));
			scanf("%s", cmd2);
			clearString(cmd, sizeof(cmd));
			sprintf(cmd, "cat timelist.txt | grep %s", cmd2);
			system(cmd);
		}
		else if (op1 == 'e') {
			// Directly Edit
			clearString(cmd, sizeof(cmd));
			printf("Type timezone Continent/Region :");
			clearString(cmd2, sizeof(cmd2));
			scanf("%s", cmd2);
			sprintf(cmd, "arch-chroot /mnt ln -sf /usr/share/zoneinfo/%s /etc/localtime", cmd2);
			if (system(cmd) == 0) {
				break;
			}
			else {
				printf("The Timezone you entered is incorrect. Try Again !\n");
			}

		}	
	}
	printf("Timezone set to %s sucessfully\n", cmd);
	sleep(1);
	clear();
	printf("Setting Hardware Clock to current time...\n");
	system("arch-chroot /mnt hwclock --systohc");
	printf("Hardware Clock set sucessfully\n");
	sleep(1);
	printf("Setting locale to 'en_IN UTF-8'\n");
	system("arch-chroot /mnt echo 'en_IN UTF-8' >> /etc/locale.gen");
	system("arch-chroot /mnt locale-gen");	
	printf("Locale Set\n");
	printf("Type the name of the machine you want to keep:");
	scanf("%s", user.hostname);
	clearString(cmd2, sizeof(cmd2));
	sprintf(cmd2, "arch-chroot /mnt echo '%s' > /etc/hostname", user.hostname);
	system(cmd2);
	printf("Type password for root user:");
	system("arch-chroot /mnt passwd");
	clear();
	printf("Now setting up network services...\n");
	system("arch-chroot /mnt echo '127.0.0.1    localhost' >> /etc/hosts");
	system("arch-chroot /mnt echo '::1          localhost' >> /etc/hosts");
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "arch-chroot /mnt echo '127.0.1.1    %s.localdomain    %s' > /etc/hosts", user.hostname, user.hostname);
	system(cmd);
	printf("Type your username:");
	scanf("%s", &user.name);
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "arch-chroot /mnt useradd -m %s", *(user.name));
	system(cmd);
	printf("Now type password for %s:", *(user.name));
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "arch-chroot /mnt passwd %s", *(user.name));
	system(cmd);
	printf("Installing sudo...\n");
	system("arch-chroot /mnt pacman -S sudo --noconfirm");
	clear();
	printf("Adding %s to wheel group...\n", *(user.name));
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "arch-chroot /mnt usermod -aG wheel,video,audio,optical,storage %s", *(user.name));
	system(cmd);
	printf("Giving %s user privelleges...\n", *(user.name));
	system("arch-chroot /mnt echo 'root ALL=(ALL:ALL) ALL' > /etc/sudoers");
	system("arch-chroot /mnt echo 'wheel ALL=(ALL:ALL) ALL' >> /etc/sudoers");
	system("arch-chroot /mnt echo '@includedir /etc/sudoers.d' >> /etc/sudoers");
	printf("Setting UEFI Partition...\n");
	clearString(cmd, sizeof(cmd));
	sprintf(cmd, "arch-chroot /mnt mount --mkdir /dev/%s1 /boot/EFI", *(user.disk));
	system(cmd);
	printf("Installing GRUB...\n");
	system("arch-chroot /mnt pacman -S grub efibootmgr dosfstools mtools os-prober --noconfirm");
	printf("Does your system support Secure Boot (y/N):");
	scanf(" %c", &user.isSecureBoot);
	if (user.isSecureBoot == 'y') {
		if (system("arch-chroot /mnt grub-install --target=x86_64-efi --bootloader-id=grub_uefi --modules='tpm' --disable-shim-lock --recheck") == 0) {
			printf("Generating Grub Configuration file...");
		}
		else {
			system("arch-chroot /mnt grub-install --target=x86_64-efi --bootloader-id=grub_uefi --recheck");
		}
	}
	else {
		system("arch-chroot /mnt grub-install --target=x86_64-efi --bootloader-id=grub_uefi --recheck");
		printf("Generating Grub Configuration file...");
	}

	// Now generating grub configuration file
	system("arch-chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg");
	printf("\nBootloader Installed Sucessfully\n");

	// Now install network manager
	printf("Installing Network Manager...\n");
	system("arch-chroot /mnt pacman -S networkmanager --noconfirm");
	system("arch-chroot /mnt sudo systemctl enable NetworkManager");


	// Now ask user for choosing a desktop enviroment or any window manager
	// Then after taking input install the desktop enviroment in the user pc
	
	clear();
	printf("CHOOSE A DESKTOP ENVIROMENT TO INSTALL\n");
	printf("1. GNOME\n2. KDE Plasma\n3. XFCE\n4. OpenBox\n5. None\n");
	char ch;
	while (1) {
		printf(">>");
		scanf(" %c", &ch);
		if (ch == '1') {
			// GNOME
			system("arch-chroot /mnt pacman -S xorg xorg-server gnome gdm --noconfirm");
			system("arch-chroot /mnt systemctl enable gdm");
			break;
		}
		else if (ch == '2') {
			// KDE Plasma
			system("arch-chroot /mnt pacman -S xorg plasma plasma-meta doplhin konsole sddm --noconfirm");
			system("arch-chroot /mnt systemctl enable sddm");
			break;
		}
		else if (ch == '3') {
			// XFCE
			system("arch-chroot /mnt pacman -S install xfce4 xfce4-goodies lightdm xorg --noconfirm");
			system("arch-chroot /mnt systemctl enable lightdm");
			break;
		}
		else if (ch == '4') {
			// Openbox
			system("arch-chroot /mnt pacman -S openbox lightdm xorg --noconfirm");
			system("arch-chroot /mnt systemctl enable lightdm");
			break;
		}
		else if (kchoice == '5') {
			// None
			break;
		}
		else {
			printf("Wrong Choice, choice doesnt exist :(\n");
		}
	}	
	clear();
	printf("Installation Completed Sucessfully\n");
	printf("Happy Arch :)\n");
	printf("Rebooting in 5 seconds...");
	system("umount -R /mnt");
	sleep(5);
	system("systemctl reboot");
	return 0;
}
