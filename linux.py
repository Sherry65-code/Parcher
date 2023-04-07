from os import system
from time import sleep


def clear():
    system("clear")

def checkNet():
    if system("ping google.com -c 1 &> /dev/null") == 0:
        return True
    else:
        return False

def isUEFI():
    if system("ls /sys/firmware/efi/efivars &> /dev/null") == 0:
        return True
    else:
        return False

def verifyBootName(bootname):
    system("lsblk --nodeps -o NAME > disks.txt")
    f = open("disks.txt", "r")
    fdata = f.read()
    f.close()
    fdata = fdata.replace("NAME","")
    if bootname in fdata:
        return True
    else:
        return False


