from guizero import App, Text, TextBox, PushButton, Picture, ListBox, CheckBox
from time import sleep
#from random import randint
import random
import subprocess

#Function to start test. Called by Start Test Button
def start_test():
    change_message("Test is running")
    startButton.visible = False
    numTestsStarted[0] = 1
    numTestsFinished[0] = 1
    begin_test()
    
def begin_test():
    for i in range(len(sensors)):
        if numTestsStarted[0] <= len(sensors):
            test(sensors[i], testsToDo[i])
            text_t = "Test " + testsToDo[numTestsStarted[0]-1] + " has started on sensor " + str(sensors[numTestsStarted[0]-1])
            message_t = Text(app, text=text_t)
            start_messages.append(message_t)
            numTestsStarted[0] = numTestsStarted[0] + 1
    test_stop()

# Run cpp executable
def test(sensor, test):
#    print("testing",sensor,test)
	subprocess.call('./test_args.exe ' + str(sensor) + ' ' + str(test))
    
def test_stop():
    for i in range(len(sensors)):
        if numTestsFinished[0] <= len(sensors):
            text_t = "Test " + testsToDo[numTestsFinished[0]-1] + " has finished on sensor " + str(sensors[numTestsFinished[0]-1])
            message_t = Text(app, text=text_t)
            done_messages.append(message_t)
            numTestsFinished[0] = numTestsFinished[0] + 1
    tests_done()
            
def tests_done():
    change_message("All tests complete")
    for i in range(len(sensors)):
        start_messages[i].destroy()
        done_messages[i].destroy()
    results()
    
def results():
    for i in range(len(sensors)):
        text_t = "Result " + str(i+1) + ": " + testsToDo[i] + " Concetration " + str(random.randint(0,9))
        message_t = Text(app, text=text_t)
        result_messages.append(message_t)
#Helper function to change the value (text) of the 'message' text
def change_message(value):
    message.value = value #function changes welcome message to value of my name (from text box)

#Function to make a CheckBox with text saying "Sensor num". Sets sensor list
def make_box(num):
    box = CheckBox(app)
    box.text="Sensor "+str(num)
    box.update_command(set_sensors,[box, num])
    return box

#function to prepare screen with start test function. called by confirm tests button
def start_test_screen():
    startButton.visible=True
    test_list.visible = False
    confirmTestsButton.visible = False
    change_message("Start test when ready")
    
#Function to set sensors list. Updated when different boxes are checked or unchecked
def set_sensors(box,num):
    if box.value == 1:
        if sensors.count(num) == 0:
            sensors.append(num)
    else:
        if sensors.count(num) > 0:
            sensors.remove(num)
    sensors.sort()
  
#checks to make sure at least one sensor has been selected. If so initializes test select screen
def check_sensor_select():
    if one_box_checked():
        change_message("You must select at least one sensor to use")
    else:
        test_select_init()
     
#returns true if one or more boxes are checked    
def one_box_checked():
    one_box = ((not box1.value) and (not box2.value) and (not box3.value) and (not box4.value) and (not box5.value) and (not box6.value) and (not box7.value) and (not box8.value))
    return one_box

#intializes test select screen
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
    
#determines whether all selected sensors have tests assigned, if so goes to "start test screen"
def count_tests():
    if numTests[0] != len(sensors): #if some tests have not been assigned,update screen for next selection
        change_test_select() 
    else:
        start_test_screen() #if all tests have been assigned go to start test screen
        
    testsToDo.append(test_list.value) #keep track of tests that need to be completed
        
def change_test_select():
    change_message("Which test do you want to use for sensor " + str(sensors[numTests[0]]))
    numTests[0] = numTests[0] + 1
   
#Function to update tests to do with list selection
def set_tests(value):
    testToDo = value
	
sensors = []
tests = ["Practice Test 1", "Practice Test 2", "Practice Test 3"]
testsToDo = []
numTests = [1]
numTestsStarted = [0]
numTestsFinished = [0]
start_messages = []
done_messages = []
result_messages = []

app = App(title="Microdrop Sensor Test")

message = Text(app, text="Which sensors do you want to use?", size=20, font="Times New Roman", color ="black") #Display welcome message

box1 = make_box(1)
box2 = make_box(2)
box3 = make_box(3)
box4 = make_box(4)
box5 = make_box(5)
box6 = make_box(6)
box7 = make_box(7)
box8 = make_box(8)

test_list = ListBox(app, items=tests, visible = False)
test_list.update_command(set_tests)

confirmSensorsButton = PushButton(app, text="Confirm sensors")
confirmSensorsButton.update_command(check_sensor_select)

confirmTestsButton = PushButton(app, text="Confirm tests", visible = False)
confirmTestsButton.update_command(count_tests)

startButton = PushButton(app, text="Start Test", visible = False)
startButton.update_command(start_test)

app.display()