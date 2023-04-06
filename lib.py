# This file handles all the lower level work required for installing arch linux

from time import time
from os import system


# Return false is internet is disconnected else returns true
def check_net():
    if system('ping google.com -c 1 &> /dev/null') == 0:
        return True
    else:
        return False

# Clear the user screen
def clear_screen():
    system("clear")

def sleep(sleeptime):
    st = time()
    while time()<st+sleeptime:
        pass
