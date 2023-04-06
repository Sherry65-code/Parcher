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
    disk = input("Type disk name (/dev/sda) >>")
    if partition(disk):
        break
    else:
        pass
