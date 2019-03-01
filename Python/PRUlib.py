
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

#turn on the selected PRU (0 = PRU0, 1 = PRU1)
def init (PRU) :
	if PRU == 0:
		state = os.open(REMOTEPROC_STATE0, os.O_RDWR)	#Open /sys/class/remoteproc/remoteproc0/state, in read/write mode
		pru_state = os.read(state, 7)	#Read the current state of selected PRU
		if b'running' in pru_state:		#Do nothing if already running
			print("PRU0 is already running")
		elif b'offline' in pru_state:	#Start selected PRU when offline
			prtin("PRU0 is offline, starting now")
			try:
				os.write(state, b"start")
				print("PRU0 has started")
			except:
				print("PRU0 has failed to start")
		os.close(state)		#close /sys/class/remoteproc/remoteproc0/state
	if PRU == 1:
		state = os.open(REMOTEPROC_STATE1, os.O_RDWR)	#Open /sys/class/remoteproc/remoteproc1/state, in read/write mode
		pru_state = os.read(state, 7)	#Read the current state of selected PRU
		if b'running' in pru_state:		#Do nothing if already running
			print("PRU1 is already running")
		elif b'offline' in pru_state:	#Start selected PRU when offline
			print("PRU1 is offline, starting now")
			try:
				os.write(state, b"start")
				print("PRU1 has started")
			except:
				print("PRU1 has failed to start")
		os.close(state)		#close /sys/class/remoteproc/remoteproc0/state

#turn off the selected PRU (0 = PRU0, 1 = PRU1)
def deinit (PRU) :
	if PRU == 0:
		state = os.open(REMOTEPROC_STATE0, os.O_RDWR)
		pru_state = state.read()
		if b'running' in pru_state:
			print("PRU0 is running, shutting down")
			try:
				os.write(state, b"stop")
				print("PRU0 is offline")
			except:
				print("Something went wrong while turning off PRU0")
			os.close(state)
	if PRU == 1:
		state = os.open(REMOTEPROC_STATE1, os.O_RDWR)
		pru_state = state.read()
		if b'running' in pru_state:
			print("PRU1 is running, shutting down")
			try:
				os.write(state, b"stop")
				print("PRU1 is offline")
			except:
				print("Something went wrong while turning off PRU1")
			os.close(state)

#read one payload from the selected PRU
def read (PRU):
	data = os.open(DATA, os.O_CREAT|os.O_APPEND|os.O_RDWR)
	if PRU ==0:
		state = os.open(REMOTEPROC_STATE0, os.O_RDWR)
		dev = os.open(CHAR_DEV0, os.O_RDWR)
		cur_state = os.read(state, 7)
		os.close(state)
		if b'running' in cur_state:
			os.write(dev, b"ready")
			print("PRU0 is running, character device will be read")
			for x in range(100):
				try:
					readBuf = os.read(dev, RPMSG_BUF_SIZE) #Read the characer device and save the value in char_dev
					print(readBuf) #print the just recorded value
					os.write(data, readBuf) #write char_dev in data.txt
				except KeyboardInterrupt:
					print("payload saved in " + DATA)
					break;
				except:
					print("Could not read the character device")
					break;
		else:
			print("Device is not running")
		os.close(dev)
		os.close(data)

	if PRU ==1:
		state = os.open(REMOTEPROC_STATE1, os.O_RDWR)
		dev = os.open(CHAR_DEV1, os.O_RDWR)
		cur_state = os.read(state, 7)
		os.close(state)
		if b'running' in cur_state:
			os.write(dev, b"ready")
			print("PRU1 is running, character device will be read")
			for x in range(100):
				try:
					readBuf = os.read(dev, RPMSG_BUF_SIZE) #Read the characer device and save the value in char_dev
					print(readBuf) #print the just recorded value
					os.write(data, readBuf) #write char_dev in data.txt
				except KeyboardInterrupt:
					print("payload saved in " + DATA)
					break;
				except:
					print("Could not read the character device")
					break;
		else:
			print("Device is not running")
		os.close(dev)
		os.close(data)

#returns the status of both PRUs
def status() :
	state0 = os.open(REMOTEPROC_STATE0, os.O_RDONLY)
	state1 = os.open(REMOTEPROC_STATE1, os.O_RDONLY)
	pru_state0 = os.read(state0, 7)
	pru_state1 = os.read(state1, 7)
	os.close(state0)
	os.close(state1)
	print("PRU0 is " + pru_state0.decode("utf-8")  + "\nPRU1 is " + pru_state1.decode("utf-8") )

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
