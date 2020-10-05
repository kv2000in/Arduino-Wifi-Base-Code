'''
	2/10/18 - TODO - 
	Limit one websocket connection at a time - done 2/17/18
	Pan Tilt returns current position - which is displayed on the screen - done 2/17/18
	Able to go to a position by entering angle/number
	Home button will set the servos to mid position (150,150)
	10 uSec steps - 150 (1500 uSec) = Mid position of servo 50 (500 uSec) = -90 deg to 250 (2500 uSec) +90 deg is the range.
	Python script on the server sets the maximum and minimum Pan and Tilt values - mostly decided by the hardware limitations (moving mechanism getting stuck/not smooth etc)
	Python script edited on macbook geany IDE
	'''
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket
from subprocess import call, PIPE
import subprocess
import time
import sys
import os
clients = []
pan_servo=6
tilt_servo=7
current_pan_val=150
current_tilt_val=150
max_pan_val=220
max_tilt_val=220
min_pan_val=60
min_tilt_val=60
sleep=0.05 # sleep increments for shell script
steps=1 #Number of step wise increments
bool_pan_L=False
bool_pan_R=False
bool_tilt_U=False
bool_tilt_D=False
bool_loop_pan=False
bool_loop_tilt=False
child_sleep=sleep*2
p_l=""#to be used for subprocess
p_r=""
t_u=""
t_d=""
l_p=""
l_t=""

# either echo "0=50%" >/dev/servoblaster or echo 0=150 >/dev/servoblaster will set middle position
# from 0% to 100% or from 50 to 250  (~ 500 us to 2500 us in 10 us steps) 
#move.sh args
#SERVO=$1 # Which servo is pan and which one is tilt (0 or 1)
#STEPS=$2 # determines the speed of movement
#SLEEP=$3
#DIR=$4 # Increase or Decrease as INC or DEC
#VAL=$5 #Max/min Pan/TIlt - With INC - give MAX Val, with DEC - give MIN Val

#######Functions taken from cat /usr/lib/python2.7/subprocess.py###############
###############################################################################
### Not being used##########
def _eintr_retry_call(func, *args):
	while True:
	try:
		return func(*args)
	except (OSError, IOError) as e:
		if e.errno == errno.EINTR:
		continue
			raise

def mywait(self):
	#print "mywait called"
	while self.returncode is None:
		try:
			print "try"
			pid, sts = _eintr_retry_call(os.waitpid, self.pid, 0)
			print sts
		except OSError as e:
			print "OSERROR"
			if e.errno != errno.ECHILD:
				raise
			pid = self.pid
			sts = 0
		if pid == self.pid:
			myhandle_exitstatus(self,sts)
	return self.returncode	


def myhandle_exitstatus(self, sts, _WIFSIGNALED=os.WIFSIGNALED,_WTERMSIG=os.WTERMSIG, _WIFEXITED=os.WIFEXITED,_WEXITSTATUS=os.WEXITSTATUS):
	if _WIFSIGNALED(sts):
		# This was resulting in -N (N=signal errors)###
		#The problem was - trap wasn't working if child process was getting kill signals before it has been "created"
		# In that scenario - child process was returning WTERMSIG status rather than the exit code set by the trap function
		# I am retrieving the current values of the servos based on the exit code
		# not an ideal way - to have a non - zero exit code for sucess
		#by convention - exit 0 - success and exit range is 0-255
		# Since my servo values are with in that range - it works.
		# Added time delay after calling the subprocess.Popen to allow for it to be created. Also added time delay after sending the
		#signal for proper exit code - might not be necessary
		# debug with strace ./testsigs.sh
		
		self.returncode = -_WTERMSIG(sts)
	#print "SIGNALED"
	#elif _WIFEXITED(sts):
	if _WIFEXITED(sts):
		self.returncode = _WEXITSTATUS(sts)
	else:
		raise RuntimeError("Unknown child exit status!")
###############################################################################		
#######Functions taken from cat /usr/lib/python2.7/subprocess.py###############

class SimpleChat(WebSocket):
	def handleMessage(self):
		#pan_tilt_servo(self.data)
		continuous_pan_tilt(self.data)
	#for client in clients:
	#	if client != self:
	#		client.sendMessage(self.address[0] + u' - ' + self.data)
	
	def handleConnected(self):
		#for client in clients:
		#client.sendMessage(self.address[0] + u' - connected')
		#2/17/18 - Check if there is a client already connected
		if len(clients)>0:
			#There is a client already connected - send the connecting client a message
			self.sendMessage(u'E-TRYAGAIN')
			#close the connecting client
			self.close()
			print "Attempted to connect"
		else:
			#No client connected already - so add this new client to the list
			clients.append(self)
			self.sendMessage(send_presets())
			self.sendMessage(u'D-'+str(current_pan_val)+','+str(current_tilt_val))
			print self.address, 'connected'
	def handleClose(self):
		#Remove a nonexistent client will give value error - hence try and catch
		try:
			clients.remove(self)
			print self.address, 'closed'
			for client in clients:
				client.sendMessage(self.address[0] + u' - disconnected')
		except ValueError:
			pass

#pan_tilt_servo - NOT in USE 2/10/18
def pan_tilt_servo(client_data):
	global current_pan_val
	global current_tilt_val
	global max_pan_val
	global max_tilt_val
	global min_pan_val
	global min_tilt_val
	if (client_data.split("-")[0]=="R"):
		if (current_pan_val<max_pan_val):
			current_pan_val+=1
			mystring="echo "+str(pan_servo)+"="+str(current_pan_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
	if (client_data.split("-")[0]=="RU"):
		if (current_pan_val<max_pan_val):
			current_pan_val+=1
			mystring="echo "+str(pan_servo)+"="+str(current_pan_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
		if (current_tilt_val<max_tilt_val):
			current_tilt_val+=1
			mystring="echo "+str(tilt_servo)+"="+str(current_tilt_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
if (client_data.split("-")[0]=="RD"):
	if (current_pan_val<max_pan_val):
		current_pan_val+=1
			mystring="echo "+str(pan_servo)+"="+str(current_pan_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
		if (current_tilt_val>min_tilt_val):
			current_tilt_val-=1
			mystring="echo "+str(tilt_servo)+"="+str(current_tilt_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
if (client_data.split("-")[0]=="U"):
	if (current_tilt_val<max_tilt_val):
		current_tilt_val+=1
			mystring="echo "+str(tilt_servo)+"="+str(current_tilt_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
	if (client_data.split("-")[0]=="L"):
		if (current_pan_val>min_pan_val):
			current_pan_val-=1
			mystring="echo "+str(pan_servo)+"="+str(current_pan_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
if (client_data.split("-")[0]=="LU"):
	if (current_tilt_val<max_tilt_val):
		current_tilt_val+=1
			mystring="echo "+str(tilt_servo)+"="+str(current_tilt_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
		if (current_pan_val>min_pan_val):
			current_pan_val-=1
			mystring="echo "+str(pan_servo)+"="+str(current_pan_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
if (client_data.split("-")[0]=="LD"):
	if (current_pan_val>min_pan_val):
		current_pan_val-=1
			mystring="echo "+str(pan_servo)+"="+str(current_pan_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
		if (current_tilt_val>min_tilt_val):
			current_tilt_val-=1
			mystring="echo "+str(tilt_servo)+"="+str(current_tilt_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
if (client_data.split("-")[0]=="D"):
	if (current_tilt_val>min_tilt_val):
		current_tilt_val-=1
			mystring="echo "+str(tilt_servo)+"="+str(current_tilt_val)+">/dev/servoblaster" 
			call(mystring,shell=True)
#continuous_pan_tilt - slowly pan and tilt - when either joystick or "pan"/"tilt" activated
def continuous_pan_tilt(client_data):
	global bool_pan_L
	global bool_pan_R
	global bool_tilt_U
	global bool_tilt_D
	global bool_loop_pan
	global bool_loop_tilt
	global current_pan_val
	global current_tilt_val
	
	#ToDO change Steps and Sleep based on client_data.split("-")[1]
	#If data.split <50 slow speed >100 - fastest speed
	if (client_data.split("-")[0]=="R"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_tilt_U:
			tilt_up("stop")
		if bool_tilt_D:
			tilt_down("stop")
		if bool_pan_L:
			pan_left("stop")
		if not (bool_pan_R):
			pan_right("start")
	if (client_data.split("-")[0]=="RU"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_pan_L:
			pan_left("stop")
		if not (bool_pan_R):
			pan_right("start")
		if bool_tilt_D:
			tilt_down("stop")
		if not (bool_tilt_U):
			tilt_up("start")
	if (client_data.split("-")[0]=="RD"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_pan_L:
			pan_left("stop")
		if not (bool_pan_R):
			pan_right("start")
		if bool_tilt_U:
			tilt_up("stop")
		if not (bool_tilt_D):
			tilt_down("start")
	if (client_data.split("-")[0]=="U"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_pan_L:
			pan_left("stop")
		if bool_pan_R:
			pan_right("stop")
		if bool_tilt_D:
			tilt_down("stop")
		if not (bool_tilt_U):
			tilt_up("start")
	if (client_data.split("-")[0]=="L"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_tilt_U:
			tilt_up("stop")
		if bool_tilt_D:
			tilt_down("stop")
		if bool_pan_R:
			pan_right("stop")
		if not (bool_pan_L):
			pan_left("start")
	if (client_data.split("-")[0]=="LU"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_pan_R:
			pan_right("stop")
		if not (bool_pan_L):
			pan_left("start")
		if bool_tilt_D:
			tilt_down("stop")
		if not (bool_tilt_U):
			tilt_up("start")
	if (client_data.split("-")[0]=="LD"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_pan_R:
			pan_right("stop")
		if not (bool_pan_L):
			pan_left("start")
		if bool_tilt_U:
			tilt_up("stop")
		if not (bool_tilt_D):
			tilt_down("start")
	if (client_data.split("-")[0]=="D"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_pan_L:
			pan_left("stop")
		if bool_pan_R:
			pan_right("stop")
		if bool_tilt_U:
			tilt_up("stop")
		if not (bool_tilt_D):
			tilt_down("start")
	if (client_data.split("-")[0]=="P"):
		if bool_pan_L:
			pan_left("stop")
		if bool_pan_R:
			pan_right("stop")
		if bool_tilt_D:
			tilt_down("stop")
		if bool_tilt_U:
			tilt_up("stop")
		if not (bool_loop_pan):
			loop_pan("start")
	if (client_data.split("-")[0]=="T"):
		if bool_pan_L:
			pan_left("stop")
		if bool_pan_R:
			pan_right("stop")
		if bool_tilt_D:
			tilt_down("stop")
		if bool_tilt_U:
			tilt_up("stop")
		if not (bool_loop_tilt):
			loop_tilt("start")
	if (client_data.split("-")[0]=="X"):
		if bool_loop_pan:
			loop_pan("stop")
		if bool_loop_tilt:
			loop_tilt("stop")
		if bool_tilt_U:
			tilt_up("stop")
		if bool_tilt_D:
			tilt_down("stop")
		if bool_pan_L:
			pan_left("stop")
		if bool_pan_R:
			pan_right("stop")
		#When Stopping Loop/Pan - send current position to clients
		for ws in clients:
			ws.sendMessage(u'D-'+str(current_pan_val)+','+str(current_tilt_val))
	if (client_data.split("-")[0]=="p"):
		if bool_loop_pan:
			loop_pan("stop")
			#When Stopping Loop/Pan - send current position to clients
			for ws in clients:
				ws.sendMessage(u'D-'+str(current_pan_val)+','+str(current_tilt_val))
	if (client_data.split("-")[0]=="t"):
		if bool_loop_tilt:
			loop_tilt("stop")
			#When Stopping Loop/Pan - send current position to clients
			for ws in clients:
				ws.sendMessage(u'D-'+str(current_pan_val)+','+str(current_tilt_val))
	#Go to preset	
if (client_data.split("-")[0]=="S"):
	go_to_preset(client_data.split("-")[1])
	#Save preset
	if (client_data.split("-")[0]=="s"):
		save_preset(client_data.split("-")[1])
#Delete preset
if (client_data.split("-")[0]=="d"):
	del_preset(client_data.split("-")[1])
	#List preset names to clients
	# Do it when client connects instead of  on request 
	if (client_data.split("-")[0]=="l"):
		for ws in clients:
			#		try:
			#ws._sendMessage(False,TEXT,message)
			ws.sendMessage(send_presets())
#			print "a"
#		except:
#			print sys.exc_info()[0]
#			raise


def pan_left(command):
	global bool_pan_L
	global current_pan_val
	global p_l
	if (command=="start"):
		if not (bool_pan_L):
			bool_pan_L=True
			#print "Pan Left Started"
			#print current_pan_val
			try:
				p_l = subprocess.Popen(['./move.sh',str(pan_servo),str(steps),str(sleep),'INC',str(max_pan_val),str(current_pan_val)])
				# checking boolean p_l._child_created is unreliable - it returns true even if the child process is still being created
				# in "fast on and off" situation - this leads to signals not being properly trapped by trap in the shell script
				#So - better to sleep for at least "sleep" time
				#print p_l._child_created
				time.sleep(child_sleep)
			except:
				print sys.exc_info()[0]
				raise
	#print p_l.pid
if (command=="stop"):
	#print "Pan left stop called"
	bool_pan_L=False
		try:
			p_l.send_signal(2) #Signal 2 = SIGINT check kill -l
			#p_l.poll()
			#print "sleeping for 0.10 seconds"
			time.sleep(child_sleep)
			p_l.wait()
			#mywait(p_l)
			#print "beyond mywait"
			#print p_l.stdout.readline()
			#p_l.communicate()
			#print p_l.returncode # move.sh returns current value as returncode
			#must use .wait() for returncode to be returned.
			current_pan_val=int(p_l.returncode)
	#print current_pan_val
	#print p_l.stdout.readline()
	#print p_l.communicate()[0]
	#after killing - read the current value from the file and store it as current position
	#p_l = open(str(pan_servo),'r')
	#if (p_l.readline()):
	#	p_l.seek(0)
	#	current_pan_val = int(p_l.readline())
	#else:
	#	current_pan_val=150
	#p_l.close()
	#print type(current_pan_val)
	#print current_pan_val
	except:
		print sys.exc_info()[0]
		raise

def pan_right(command):
	global bool_pan_R
	global current_pan_val
	global p_r
	if (command=="start"):
		if not (bool_pan_R):
			bool_pan_R=True
			#print "Pan right started"
			#print current_pan_val
			#q = subprocess.Popen(['./pan.sh','1','DEC'])
			p_r = subprocess.Popen(['./move.sh',str(pan_servo),str(steps),str(sleep),'DEC',str(min_pan_val),str(current_pan_val)])
			#print p_r._child_created
			time.sleep(child_sleep)
	if (command=="stop"):
		#print "Pan right stop called"
		bool_pan_R=False
		p_r.send_signal(2)
		time.sleep(child_sleep)
		p_r.wait()
		#mywait(p_r)
		current_pan_val=int(p_r.returncode)
#print current_pan_val
#p_r = open(str(pan_servo),'r')
#if (p_r.readline()):
#	p_r.seek(0)
#	current_pan_val = int(p_r.readline())
#else:
#	current_pan_val=150
#p_r.close()
def tilt_up(command):
	global bool_tilt_U
	global t_u
	global current_tilt_val
	if (command=="start"):
		if not (bool_tilt_U):
			#print "Tilt Up started"
			#print current_tilt_val
			bool_tilt_U=True
			#r = subprocess.Popen(['./tilt.sh','1','DEC'])
			t_u = subprocess.Popen(['./move.sh',str(tilt_servo),str(steps),str(sleep),'DEC',str(min_tilt_val),str(current_tilt_val)])
			#print t_u._child_created
			time.sleep(child_sleep)
	if (command=="stop"):
		#print "Tilt up stop called"
		bool_tilt_U=False
		t_u.send_signal(2)
		time.sleep(child_sleep)
		t_u.wait()
		#mywait(t_u)
		current_tilt_val = int(t_u.returncode)
#print current_tilt_val
#t_u = open(str(tilt_servo),'r')
#if (t_u.readline()):
#	t_u.seek(0)
#	current_tilt_val = int(t_u.readline())
#else:
#	current_tilt_val=150
#t_u.close()
def tilt_down(command):
	global bool_tilt_D
	global t_d
	global current_tilt_val
	
	if (command=="start"):
		if not (bool_tilt_D):
			bool_tilt_D=True
			#print "Tilt down started"
			#print current_tilt_val
			t_d = subprocess.Popen(['./move.sh',str(tilt_servo),str(steps),str(sleep),'INC',str(max_tilt_val),str(current_tilt_val)])
			#print t_d._child_created
			time.sleep(child_sleep)
	#test ="TILT"
#try:
#	s = subprocess.Popen(['./testargs.sh',str(max_pan_val),test])
#except Exception as e:
#	if hasattr(e,'message'):
#		print (e.message)
#	else:
#		print(e)
if (command=="stop"):
	#print "Tilt down stop called"
	bool_tilt_D=False
		t_d.send_signal(2)
		time.sleep(child_sleep)
		t_d.wait()
		#mywait(t_d)
		current_tilt_val = int(t_d.returncode)

#print current_tilt_val
#t_d = open(str(tilt_servo),'r')
#if (t_d.readline()):
#	t_d.seek(0)
#	current_tilt_val = int(t_d.readline())
#else:
#	current_tilt_val=150
#t_d.close()
def loop_pan(action):
	#action = start or stop
	#Determine curr position
	#Stop all active pan/tilt
	# set boolean loop_pan to True 
	#keep panning from min to max -> max to min
	# to do from_val to_val to pan/tilt for a smaller arc . Not from min to max
	global bool_loop_pan
	global l_p
	global current_pan_val
	if (action=="start"):
		if not (bool_loop_pan):
			#print "loop pan called"
			#print current_pan_val
			bool_loop_pan=True
			l_p = subprocess.Popen(['./loop.sh',str(pan_servo),str(steps),str(sleep),str(max_pan_val),str(min_pan_val),str(current_pan_val)])
			time.sleep(child_sleep)
	if (action=="stop"):
		bool_loop_pan=False
		l_p.send_signal(15)
		time.sleep(child_sleep)
		l_p.wait()
		current_pan_val = int(l_p.returncode)
#print "Loop pan ended"
#print current_pan_val
#l_p.kill()
#l_p = open(str(pan_servo),'r')
#if (l_p.readline()):
#	l_p.seek(0)
#	current_pan_val = int(l_p.readline())
#else:
#	current_pan_val=150
#l_p.close()

def loop_tilt(action):
	#keep tilting from min to max -> max to min
	global bool_loop_tilt
	global l_t
	global current_tilt_val
	if (action=="start"):
		if not (bool_loop_tilt):
			#print "loop tilt called"
			#print current_tilt_val
			bool_loop_tilt=True
			l_t = subprocess.Popen(['./loop.sh',str(tilt_servo),str(steps),str(sleep),str(max_tilt_val),str(min_tilt_val),str(current_tilt_val)])
			time.sleep(child_sleep)
	if (action=="stop"):
		bool_loop_tilt=False
		l_t.send_signal(15)
		time.sleep(child_sleep)
		l_t.wait()
		current_tilt_val = int(l_t.returncode)
#print "Loop tilt ended"
#print current_tilt_val
#l_t.kill()
#l_t = open(str(tilt_servo),'r')
#if (l_t.readline()):
#	l_t.seek(0)
#	current_tilt_val = int(l_t.readline())
#else:
#	current_tilt_val=150
#l_t.close()

def save_preset(name):
	#stop all pan tilt if ongoing - just a backup - since websocket will send X - on touch end - which basically stops all pan/tilt
	if bool_loop_pan:
		loop_pan("stop")
	if bool_loop_tilt:
		loop_tilt("stop")
	if bool_tilt_U:
		tilt_up("stop")
	if bool_tilt_D:
		tilt_down("stop")
	if bool_pan_L:
		pan_left("stop")
	if bool_pan_R:
		pan_right("stop")
	global current_pan_val
	global current_tilt_val
	#Read current position of servos
	#f = open(str(tilt_servo),'r')
	#if (f.readline()):
	#	f.seek(0)
	#	current_tilt_val = int(f.readline())
	#else:
	#	current_tilt_val=150
	#f.close()
	#f = open(str(pan_servo),'r')
	#if (f.readline()):
	#	f.seek(0)
	#	current_pan_val = int(f.readline())
	#else:
	#	current_pan_val=150
	#f.close()
	#Delete if a preset of the same name already exists
	# Validate on client if the user really wants to update the preset
	del_preset(name)
	f = open('presets','a')
	f.write(name)
	f.write(":")
	f.write(str(current_pan_val))
	f.write(",")
	f.write(str(current_tilt_val))
	f.write("\n")
	f.close()
	return True

def del_preset(name):
	f = open('presets','r')
	lines = f.readlines()
	f.close()
	f = open('presets','w')
	for line in lines:
		if line.split(":")[0] != name:
			f.write(line)
	f.close()
	return True

def go_to_preset(name):
	#stop all pan tilt if ongoing - just a backup - since websocket will send X - on touch end - which basically stops all pan/tilt
	if bool_loop_pan:
		loop_pan("stop")
	if bool_loop_tilt:
		loop_tilt("stop")
	if bool_tilt_U:
		tilt_up("stop")
	if bool_tilt_D:
		tilt_down("stop")
	if bool_pan_L:
		pan_left("stop")
	if bool_pan_R:
		pan_right("stop")
	global current_pan_val
	global current_tilt_val
	#If I do a slow move - then will have to wait for the move to return
	# If we wait for the return - and for some reason it never returns
	# code will hang
	# so - just open the presets file - read the values for  pan and tilt
	# echo servoblaster to those vals
	# update the current vals and the files for current vals
	f = open('presets','r')
	lines = f.readlines()
	f.close()
	for line in lines:
		if line.split(":")[0] == name:
			line=line.replace("\n","")
			preset_pan_val = line.split(":")[1].split(",")[0]
			preset_tilt_val = line.split(":")[1].split(",")[1]
			mytiltstring="echo "+str(tilt_servo)+"="+preset_tilt_val+">/dev/servoblaster" 
			call(mytiltstring,shell=True)
			mypanstring="echo "+str(pan_servo)+"="+preset_pan_val+">/dev/servoblaster" 
			call(mypanstring,shell=True)
			current_pan_val=int(preset_pan_val)
			current_tilt_val=int(preset_tilt_val)
			#f = open(str(pan_servo),'w')
			#f.write(preset_pan_val)
			#f.close()
			#f = open(str(tilt_servo),'w')
			#f.write(preset_tilt_val)
			#f.close()
			for ws in clients:
				ws.sendMessage(u'D-'+str(current_pan_val)+','+str(current_tilt_val))
def send_presets():
	myreturn="P-"
	f=open('presets','r')
	lines=f.readlines()
	f.close()
	mycounter=1
	for line in lines:
		if not (line.split(":")[0].replace("\n","") ==""):
			myreturn+=str(mycounter)
			myreturn+=":"
			myreturn+=line.split(":")[0].replace("\n","")
			mycounter=mycounter+1
			myreturn+=","
	return unicode(myreturn)


#On start - check the servo positions from the files and update the current values
server = SimpleWebSocketServer('', 8000, SimpleChat)
server.serveforever()

#p = subprocess.Popen("echo 'foo' && sleep 60 && echo 'bar'", shell=True)

