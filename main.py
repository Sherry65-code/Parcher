# this contains the main file 

from lib import *

# first work -> check if internet is avaliable

while True:
    print("\rCheck if internet connection persists...", end="")
    if check_net():
        print("Internet Connection Avaliablity is Verified !", end="")
    else:
        print("Waiting for connection...                          ", end="")
    sleep(1)
