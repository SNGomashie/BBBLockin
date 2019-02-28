
import keyboard
import os

#Character devices for PRU0&PRU1
CHAR_DEV0 = "/dev/rpmsg_pru30"
CHAR_DEV1 = "/dev/rpmsg_pru31"

#Sysfs interface for PRU0&PRU1
REMOTEPROC_STATE0 = "/sys/class/remoteproc/remoteproc1/state"
REMOTEPROC_FIRM0 = "/sys/class/remoteproc/remoteproc1/firmware"
REMOTEPROC_STATE1 = "/sys/class/remoteproc/remoteproc2/state"
REMOTEPROC_FIRM1 = "/sys/class/remoteproc/remoteproc2/firmware"

#Firmware name for PRU0&PRU1
FIRMWARE0 = "am335x-pru0-fw"
FIRMWARE1 = "am335x-pru1-fw"

#read data location
DATA = "data.txt"

#RPMsg buffer size
RPMSG_BUF_SIZE = 512
readBuf = "\0" * 512

def rpmsginit (PRU) :
	dev = os.open(CHAR_DEV1, os.O_RDWR)
	os.write(dev, b"ready")
	os.close(dev)

#turn on the selected PRU (0 = PRU0, 1 = PRU1)
def init (PRU) :
	if PRU == 0:
		state0 = open(REMOTEPROC_STATE0, 'r') #Open /sys/class/remoteproc/remoteprocx/state, in read mode
		pru_state = state0.read() #Read the current state of selected PRU
		if 'running' in pru_state: #Do nothing if already running
			print("PRU0 is already running")
		else:	#Start selected PRU when offline
			print("PRU0 is offline, starting now")
			state0 = open(REMOTEPROC_STATE0, 'w')
			state0.write("start")
			print("PRU0 started")
		state0.close() #close /sys/class/remoteproc/remoteprocx/state
	if PRU == 1:
		state1 = open(REMOTEPROC_STATE1, 'r') #Open /sys/class/remoteproc/remoteprocx/state, in read mode
		pru_state = state1.read() #Read the current state of selected PRU
		if 'running' in pru_state: #Do nothing if already running
			print("PRU1 is already running")
		else:	#Start selected PRU when offline
			print("PRU1 is offline, starting now")
			state1 = open(REMOTEPROC_STATE1, 'w')
			state1.write("start")
			print("PRU1 started")
		state1.close() #close /sys/class/remoteproc/remoteprocx/state

#turn off the selected PRU (0 = PRU0, 1 = PRU1)
def deinit (PRU) :
	if PRU == 0:
		state = open(REMOTEPROC_STATE0, 'r') #Open /sys/class/remoteproc/remoteprocx/state, in read mode
		pru_state = state.read() #Read the current state of selected PRU
		state = open(REMOTEPROC_STATE0, 'w') #Open /sys/class/remoteproc/remoteprocx/state, in write mode
		if 'running' in pru_state: #Do nothing if already running
			print("PRU0 is running, shuting down")
			state.write("stop")
			print("PRU0 is offline")
		else:
			print("PRU0 is already offline")
		state.close()
	if PRU == 1:
		state = open(REMOTEPROC_STATE1, 'r') #Open /sys/class/remoteproc/remoteprocx/state, in read mode
		pru_state = state.read() #Read the current state of selected PRU
		state = open(REMOTEPROC_STATE1, 'w') #Open /sys/class/remoteproc/remoteprocx/state, in write mode
		if 'running' in pru_state: #Do nothing if already running
			print("PRU1 is running, shuting down")
			state.write("stop")
			print("PRU1 is offline")
		else:
			print("PRU1 is already offline")
		state.close()

#read one payload from the selected PRU
def read (PRU):
	data = os.open(DATA, os.O_CREAT|os.O_APPEND|os.O_RDWR)
	if PRU == 0:
		state = os.open(REMOTEPROC_STATE0, os.O_RDWR)
		dev = os.open(CHAR_DEV0, os.O_RDWR)
		cur_state = os.read(state, 7)
		os.close(state)
		if b'running' in cur_state:
			print("Device is running, character device will be read")
			readBuf = os.read(dev, RPMSG_BUF_SIZE) #Read the characer device and save the value in char_dev
			print(str(readBuf, 'utf-8')) #print the just recorded value
			print("payload saved in " + DATA)
			os.write(data, readBuf) #write char_dev in data.txt
			os.close(dev)
			os.close(data)
		else:
			print("Device is not running")
			os.close(dev)
			os.close(data)
	elif PRU == 1:
		state = os.open(REMOTEPROC_STATE1, os.O_RDWR)
		dev = os.open(CHAR_DEV1, os.O_RDWR)
		cur_state = os.read(state, 7)
		os.close(state)
		if b'running' in cur_state:
			os.write(dev, b"ready")
			print("Device is running, character device will be read")
			while 1:
				try:
					readBuf = os.read(dev, RPMSG_BUF_SIZE) #Read the characer device and save the value in char_dev
					print(readBuf) #print the just recorded value
					os.write(data, readBuf) #write char_dev in data.txt
				except:
					print("payload saved in " + DATA)
					os.close(dev)
					os.close(data)
					break;
		else:
			print("Device is not running")
			os.close(dev)
			os.close(data)

#returns the status of both PRUs
def status() :
	state0 = open(REMOTEPROC_STATE0, 'r')
	state1 = open(REMOTEPROC_STATE1, 'r')
	pru_state0 = state0.read()
	pru_state1 = state1.read()
	state0.close()
	state1.close()
	print("PRU0 is " + pru_state0 + "PRU1 is " + pru_state1)

#kills both PRUs
def kill() :
	state0 = os.open(REMOTEPROC_STATE0, os.O_RDWR)
	state1 = os.open(REMOTEPROC_STATE1, os.O_RDWR)
	cur_state0 = os.read(state0, 7)
	cur_state1 = os.read(state1, 7)
	if b"running" in cur_state0:
		os.write(state0,b"stop")
		print("PRU0 is offline")
	elif b"offline" in cur_state0:
		print("pru0 is already dead")
	if b"running" in cur_state1:
		os.write(state1,b"stop")
		print("PRU1 is offline")
	elif b"offline" in cur_state1:
		print("pru1 is already dead")
	os.close(state0)
	os.close(state1)

#Wakes both PRUs
def goodmorning() :
	state0 = os.open(REMOTEPROC_STATE0, os.O_RDWR)
	state1 = os.open(REMOTEPROC_STATE1, os.O_RDWR)
	cur_state0 = os.read(state0, 7)
	cur_state1 = os.read(state1, 7)
	if b"running" in cur_state0:
		print("PRU0 is already awake")
	elif b"offline" in cur_state0:
		os.write(state0,b"start")
		print("PRU0 is running")
	if b"running" in cur_state1:
		print("PRU1 is already awake")
	elif b"offline" in cur_state1:
		os.write(state1,b"start")
		print("PRU1 is running")
	os.close(state0)
	os.close(state1)

#Restarts both PRUs
def restart(PRU) :
	if PRU == 0:
		state0 = os.open(REMOTEPROC_STATE0, os.O_RDWR)
		cur_state0 = os.read(state0, 7)
		if b"running" in cur_state0:
			os.write(state0,b"stop")
			os.write(state0,b"start")
			print("PRU0 has been reset")
		elif b"offline" in cur_state0:
			os.write(state0,b"start")
			print("PRU0 has been reset")
		os.close(state0)
	elif PRU == 1:
		state1 = os.open(REMOTEPROC_STATE1, os.O_RDWR)
		cur_state1 = os.read(state1, 7)
		if b"running" in cur_state1:
			os.write(state1,b"stop")
			os.write(state1,b"start")
			print("PRU1 has been reset")
		elif b"offline" in cur_state1:
			os.write(state1,b"start")
			print("PRU1 has been reset")
		os.close(state1)
