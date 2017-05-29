# covert_detection.py
# detects whether a file is being used as a covert channel
# Mano Wared 913398346
# Patrick Ong 997520004

import os
from stat import *

channel = "./testfile"
filename = "profile"
length = 10000000

def profile():
	thefile = open(filename, 'w')
	for i in range(length):
		poll = os.stat(channel)[ST_MODE]
		thefile.write(str(poll)+"\n")
	thefile.close()

def detect():
	thefile = open(filename, 'r')
	for line in thefile:
		poll = os.stat(channel)[ST_MODE]
		if line != str(poll) + "\n":
			thefile.close()
			return -1
	thefile.close()
	return 0


mode = raw_input("Mode: ")

if mode == "0":
	profile()
elif mode == "1":
	pid = os.fork()
	if pid == 0:
		status = detect()
		if status == -1:
			print("!!! WARNING: FILE PERMISSIONS CHANGED !!!")
		else:
			print("File untouched")
	else:
		exit()