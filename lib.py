from time import time
from subprocess import run, PIPE
from os import system
def sleep(sleeptime):
    st = time()
    while (time()<st+sleeptime):
        pass

def testNetwork():
    nettest = run(["ping","-c","1", "google.com"], stdout=PIPE)
    if "1 packets transmitted" in nettest.stdout.decode():
        return True
    return False

def sys(argument):
    mainarg = argument.split(" ")
    outer = run(mainarg, stdout=PIPE, stderr=PIPE)
    if "denied" in outer.stderr.decode():
        argument = f"sudo {argument}"
        mainarg = argument.split(" ")
        outer = run(mainarg, stdout=PIPE)
    return outer.stdout.decode()

def clear():
    run(["clear"])

def criticalexe(cmd):
    system(cmd)

def countdown(howmuch):
    x = howmuch
    while x!=0:
        print(f"{x}")
        x-=1
