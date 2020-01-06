from itertools import count, cycle
import itertools
import threading
import time
import sys
import tkinter as tk
from PIL import Image, ImageTk
from guizero import App, Text, TextBox, PushButton, Picture, ListBox, CheckBox
from time import sleep
import subprocess
from ReadCSV import importCSV ##changed the function so it didn't have the same name. Don't need .py. BBR
from DropBoxUpload_and_Delete import Upload_and_Delete
import os
import pyglet

#Function to start test. Called by Start Test Button
def animate():
	#put some animation stuff here
	#im = Image.open('drop.gif')
	#temp = im.copy()
	#temp = pics[0].convert('RGBA')
	#gif['frames'] = [ImageTk.PhotoImage(temp)]
	#im.seek(1)
	#temp.paste(im.copy())
	#temp.save('test.png')
	#root = tk.Tk()
	#root.geometry("300x300")
	#lbl = ImageLabel(root)
	#lbl.pack()
	#lbl.load('drop.gif')
	#root.mainloop()
	ag_file = "drop.gif"
	animation = pyglet.resource.animation(ag_file)
	sprite = pyglet.sprite.Sprite(animation)
	# create a window and set it to the image size
	win = pyglet.window.Window(width=300, height=300)
	# set window background color = r, g, b, alpha
	# each value goes from 0.0 to 1.0
	#green = 0, 1, 0, 
	backg = 0, 0, 0, 1
	pyglet.gl.glClearColor(*backg)
	@win.event
	def on_draw():
	    win.clear()
	    sprite.draw()
	pyglet.app.run()

def start_test():
	# numTestsStarted[0] = 1
	# numTestsFinished[0] = 1
	clear_start_screen()
	change_message("Test in progress...")
	#backButton.visible = True ## May want to rethink this one
	#t = threading.Thread(target=animate)
	#t.start()
	#loading_picture.visible = True
	app.update()
	#animate()
	f = open(filename, "w+");
	f.close()
	cmd_test()
	#t.terminate()
	get_results()
	
	
def get_results():
	print("Getting results from CSV")
	#filename = 'DPV_test1.csv'##figure out a way to not hardcode it BBR
	for i in range(len(sensors)):
		filename = "DPV_test" + str(sensors[i]) + ".csv"
		conc_t = importCSV(filename)
		print(conc_t)
		concs.append(conc_t)
	show_results()
	
def show_results():
	loading_picture.visible = False
	change_message("Test complete")
	backButton.visible = True ## Moved here to avoid confusion BBR
	uploadButton.visible = True
	
	for i in range(len(sensors)):
		print(i)
		conc_str_t = "{:.1f}".format(concs[i-1])
		text_t = "Sensor " + str(sensors[i-1]) + " Concentration: " + conc_str_t + " mM"
		result_messages[i-1].value = text_t
		result_messages[i-1].visible = True
	dpv_picture.value = "DPV_test" + str(sensors[0]) + ".png" #Plots first graph
	dpv_picture.visible  = True
	
def upload():
	uploadButton.visible = False
	backButton.text = "Uploading Files, Please Wait"
	for i in range(len(sensors)):
		filename = "DPV_test" + str(sensors[i]) + ".csv"
		Upload_and_Delete(filename, sensors[i])
		
	backButton.text = "Restart"
	
def cmd_test():
	# Create string of arguments for command line (numbers of selected sensors) 
	sensors_str = str(sensors[0])
	for i in range(1,len(sensors)):
		sensors_str = sensors_str + ' ' + str(sensors[i])
	# Run cpp executable with arguments
	subprocess.call('./SAM_SPI_Comm ' + sensors_str, shell=True) # Raspbian
        # subprocess.call('./SAM_SPI_Comm ' + sensors_str) # Windows
	
	
## Multi Test Function    
## Steps through tests to carry out
# def begin_test():
    # for i in range(len(sensors)):
        # if numTestsStarted[0] <= len(sensors):
            # test(sensors[i], testsToDo[i])
            # text_t = "Test " + testsToDo[numTestsStarted[0]-1] + " has started on sensor " + str(sensors[numTestsStarted[0]-1])
            # message_t = Text(app, text=text_t)
            # start_messages.append(message_t)
            # numTestsStarted[0] = numTestsStarted[0] + 1
    # test_stop()

## Multi Test Function
## Dummy function for testing	
# def test(sensor, test):
   # print("testing",sensor,test)

## Multi Test Function
def test_stop():
    for i in range(len(sensors)):
        if numTestsFinished[0] <= len(sensors):
            text_t = "Test " + testsToDo[numTestsFinished[0]-1] + " has finished on sensor " + str(sensors[numTestsFinished[0]-1])
            message_t = Text(app, text=text_t)
            done_messages.append(message_t)
            numTestsFinished[0] = numTestsFinished[0] + 1
    tests_done()

## Multi Test Function
def tests_done():
    change_message("All tests complete")
    for i in range(len(sensors)):
        start_messages[i].destroy()
        done_messages[i].destroy()
    results()

## Multi Test Function    
def results():
	#result_messages = []
	for i in range(len(sensors)):
		text_t = "Result " + str(i+1) + ": " + testsToDo[i] + " Concentration " + str(random.randint(0,9))
		message_t = Text(app, text=text_t)
		result_messages.append(message_t)

# Helper function to change the value (text) of the 'message' text
def change_message(value):
    message.value = value #function changes welcome message to value of my name (from text box)

# Function to make a CheckBox with text saying "Sensor num". Sets sensor list
def make_box(num):
	box = CheckBox(app)
	box.text="Sensor "+str(num)
	box.text_size = 11 # Resize the text
	box.resize(20,2) # Resize the check boxes
	box.update_command(set_sensors,[box, num])
	return box

## Multi Test Function
## Function to prepare screen with start test function. called by confirm tests button
def start_test_screen():
    startButton.visible=True
    test_list.visible = False
    confirmTestsButton.visible = False
    change_message("Start test when ready")
    
# Function to set sensors list. Updated when different boxes are checked or unchecked
def set_sensors(box,num):
    if box.value == 1:
        if sensors.count(num) == 0:
            sensors.append(num)
    else:
        if sensors.count(num) > 0:
            sensors.remove(num)
    sensors.sort()
  
# Checks to make sure at least one sensor has been selected. If so initializes test screen
def check_sensor_select():
	if one_box_checked():
		change_message("You must select at least one sensor to use")
	else:
		# test_select_init()	# for multi test
		start_test()			# single test - jump straight to test start
     
# Returns true if one or more boxes are checked    
def one_box_checked():
    one_box = ((not box1.value) and (not box2.value) and (not box3.value) and (not box4.value) and (not box5.value) and (not box6.value) and (not box7.value) and (not box8.value))
    return one_box

def clear_start_screen():
	box1.visible = False
	box2.visible = False
	box3.visible = False
	box4.visible = False
	box5.visible = False
	box6.visible = False
	box7.visible = False
	box8.visible = False
	startButton.visible = False
	
## Multi Test Function
## Intializes test select screen
def test_select_init():
        box1.visible = False
        box2.visible = False
        box3.visible = False
        box4.visible = False
        box5.visible = False
        box6.visible = False
        box7.visible = False
        box8.visible = False
        confirmSensorsButton.visible = False
        change_message("Which test do you want to use for sensor " +str(sensors[0]) )
        
        test_list.visible = True
        confirmTestsButton.visible = True
        numTests[0] = 1
    
##Multi Test Function
## Determines whether all selected sensors have tests assigned, if so goes to "start test screen"
def count_tests():
    if numTests[0] != len(sensors): #if some tests have not been assigned,update screen for next selection
        change_test_select() 
    else:
        start_test_screen() #if all tests have been assigned go to start test screen
        
    testsToDo.append(test_list.value) #keep track of tests that need to be completed

## Multi Test Function	
def change_test_select():
    change_message("Which test do you want to use for sensor " + str(sensors[numTests[0]]))
    numTests[0] = numTests[0] + 1

## Multi Test Function	
## Function to update tests to do with list selection
def set_tests(value):
    testToDo = value
	
def start_screen():
	change_message("Which sensors do you want to use?") #Display welcome message
	
	box1.visible = True
	box2.visible = True
	box3.visible = True
	box4.visible = True
	box5.visible = True
	box6.visible = True
	box7.visible = True
	box8.visible = True
	
	startButton.visible = True
	backButton.visible = False

def restart():
	#Get rid of the text of the results
	for i in range(len(sensors)):
		result_messages[i-1].visible = False
		filename = "DPV_test" + str(sensors[i]) + ".csv"
		if(os.path.exists(filename)):
			os.remove(filename)
		filename = "DPV_test" + str(sensors[i]) + ".png"
		if(os.path.exists(filename)):
			os.remove(filename)
	dpv_picture.visible = False
	uploadButton.visible = False
	#Go to start screen
	start_screen()
	
class ImageLabel(tk.Label):
    """
    A Label that displays images, and plays them if they are gifs

    :im: A PIL Image instance or a string filename
    """
    def load(self, im):
        if isinstance(im, str):
            im = Image.open(im)
        frames = []

        try:
            for i in count(1):
                frames.append(ImageTk.PhotoImage(im.copy()))
                im.seek(i)
        except EOFError:
            pass
        self.frames = cycle(frames)

        try:
            self.delay = im.info['duration']
        except:
            self.delay = 100

        if len(frames) == 1:
            self.config(image=next(self.frames))
        else:
            self.next_frame()

    def unload(self):
        self.config(image=None)
        self.frames = None

    def next_frame(self):
        if self.frames:
            self.config(image=next(self.frames))
            self.after(self.delay, self.next_frame)

sensors = []
tests = ["Practice Test 1"]
testsToDo = []
numTests = [1]
numTestsStarted = [0]
numTestsFinished = [0]
start_messages = []
done_messages = []
result_messages = []
concs = []
filename = 'DPV_test1.csv';

app = App(title="Microdrop Sensor Test")
app.set_full_screen()
app.bg = "#E9F1F4"

message = Text(app, text="", size=20, font="Arial", color ="black") 

box1 = make_box(1)
box2 = make_box(2)
box3 = make_box(3)
box4 = make_box(4)
box5 = make_box(5)
box6 = make_box(6)
box7 = make_box(7)
box8 = make_box(8)

for i in range(8):
		text_t = "Sensor " + str(i) + ": " + "Concentration " + "TEST" #make 8 result messages
		message_t = Text(app, text=text_t)
		message_t.visible = False
		result_messages.append(message_t)


dpv_picture = Picture(app)
dpv_picture.visible = False
dpv_picture.height = 300
dpv_picture.width = 300

loading_picture = Picture(app,image = 'drop.gif')
loading_picture.visible = False

startButton = PushButton(app, text="Start Test")
startButton.text_size = 11
startButton.font = "Arial"
startButton.update_command(check_sensor_select)

backButton = PushButton(app, text="Restart without saving")
backButton.text_size = 11
backButton.font = "Arial"
backButton.update_command(restart) ####### May need more sophisticated restart that interrupts the test. This just sends back to the first screen.

uploadButton = PushButton(app, text="Save files to DropBox");
uploadButton.font = "Arial"
uploadButton.text_size = 11
uploadButton.visible = False
uploadButton.update_command(upload);

start_screen()

################### MULTI TEST BUTTON SETUP ####################
# test_list = ListBox(app, items=tests, visible = False)
# test_list.update_command(set_tests)

# confirmSensorsButton = PushButton(app, text="Confirm sensors")
# confirmSensorsButton.update_command(check_sensor_select)

# confirmTestsButton = PushButton(app, text="Confirm tests", visible = False)
# confirmTestsButton.update_command(count_tests)

# startButton = PushButton(app, text="Start Test", visible = False)
# startButton.update_command(start_test)

app.display()
