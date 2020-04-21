'''
SuperSmartHome control center main application

Version 0.1.6 Alpha

    --- Changelog 20.04.2020 (build 6)
    [] Disabed the Timeloop nodes check and implemented a tool button
    [] Set disabling buttons while a executing a method that should not be
    interrupted.
    [] ISSUE: PyQt does not support calls from an external thread. Solved disabling
    the Timeloop jobs that impact on this behavior.
    [] Added the Reset Log and About tool buttons.

Date April 2020
Author Enrico Miglino balearicdynamics@gmail.com

Open Source License LGPL 3.0
'''

import sys
import subprocess
from PySide.QtGui import *
from PySide.QtCore import *
import string
import threading
import classes.secrets as secrets
from classes.voice import Speak
from classes.nodes import NodesAPI
import strings # Application strings
from datetime import datetime
from datetime import timedelta
from timeloop import Timeloop
from mainwindow import Ui_MainWindow

'''
GLOBAL PARAMETERS --------------------------------------------
'''

# Temporized recursive jobs based on Timeloop
TIME_TICK = 1 # Clock step (sec)
OTP_TICK = 60 # OTP Generation (sec)
NODES_RETRY = 1 # Number of session retries before raising an error
NODES_TIMEOUT = 3 # Number of seconds waiting for a node response

OTP_LEN = 4 # Numeric OTP pin characters

# Create the Timeloop instance
tl = Timeloop()

otp = "0000" # Initial OTP immediately changed
time_start = strings.S_UPTIME + datetime.now().strftime("%a, %d %B %Y at %H:%M:%S")

# Create the speaking class instance
narrator = Speak()

# Create the nodes class instance
nodes_api = NodesAPI()

'''
USER INTERFACE ------------------------------------------------
'''

class MainWindow(QMainWindow, Ui_MainWindow):
    '''
    Main window class. Manages the Qt environment and the Pyside converted
    UI design.
    '''
    def __init__(self):
        '''
        Initialization function.
        Setup the user interface and show it.
        '''
        global time_start
        global narrator
        global nodes_api

        super(MainWindow, self).__init__()
        self.setupUi(self)
        self.labelUpTime.setText(time_start)
        self.labelAlertDoorOpener.setText(strings.S_NODE_OK)
        self.assign_widgets()
        self.show()
        # Notify the program has started
        self.update_message_from_id(0)
        self.t_play_sentence(0)
        # And initialize the log
        self.log_clean()

    def assign_widgets(self):
        '''
        Define the interactive elements and the associated
        methods
        '''
        self.toolButtonOpenDoor.clicked.connect(self.button_open_door)
        self.toolButtonNodesHealth.clicked.connect(self.nodes_health)
        self.toolButtonResetLog.clicked.connect(self.log_clean)
        self.toolButtonAbout.clicked.connect(self.about)

    def t_play_sentence(self, sentence_id):
        '''
        Speak a sentence via a separate thread calling
        the narrator class method announce.
        '''
        global narrator
        
        # Create a thread from a function with arguments
        th = threading.Thread(target=self.play_sentence, args=(str(sentence_id)))
        # Start the thread
        th.start()

    def play_sentence(self, sentence_id):
        '''
        Function associated by the threaded call t_play_sentence()
        '''
        global narrator
        
        narrator.announce(int(sentence_id))
        
    def time_otp_set(self):
        '''
        Update the clock timer and last OTP password
        '''
        global otp
        
        timeLabel = datetime.now().strftime("%H:%M:%S")
        timeLabel += strings.S_OTP + otp
        
        self.labelTime.setText(timeLabel)
        
    def log_update(self, log_string):
        '''
        Update the log list with a new sentence with timestamp
        '''
        timestamp = "[" + datetime.now().strftime("%a, %d %b %Y - %H:%M:%S") + "] "
        timestamp += log_string
        # Write the log to the screen
        self.textBrowserEventsLog.append(timestamp)
        
    def log_clean(self):
        '''
        Clean the log list with a new sentence with timestamp
        '''
        global narrator
        
        timestamp = "[" + datetime.now().strftime("%a, %d %b %Y - %H:%M:%S") + "] "
        timestamp += narrator.getString(4)
        # Write the log to the screen
        self.textBrowserEventsLog.setText(timestamp)
        
    def update_message(self, message, ts = True):
        '''
        Update the realtime message with the specified string
        prepended with the timestamp.
        If ts is False the message is shown without
        timestamp.
        '''
        if ts:
            timestamp = datetime.now().strftime("%H:%M:%S - ")
        else:
            timestamp = ""
            
        timestamp += message
        self.labelRealTimeMessage.setText(timestamp)
        
    def update_message_from_id(self, string_id, ts = True):
        '''
        Call the method update_message after retrieving one of
        the prebuilt strings from the sentences json file
        '''
        global narrator
        
        self.update_message(narrator.getString(string_id), ts)
    
    def button_open_door(self):
        '''
        Execute the procedure to open the door on button call
        '''
        global nodesAPI
        global narrator
        global otp
        
        # Temporary disable the buttons until the action is not completed
        self.toolButtonOpenDoor.setEnabled(False)
        self.toolButtonNodesHealth.setEnabled(False)

        # Saves locally the current otp password to be reusable during
        # the two calls to avoid an unwanted error if the global otp
        # automatically changes between the two calls.0
        door_otp = otp
        # prepare the first HTTP GET api call
        door_get = nodes_api.get_url(1) + "/otp?pass=" + str(door_otp)
        
        ###### For testing only!!! Comment to enable the opener
        # door_otp=0
        
        # Executes the call
        api_return = nodes_api.url_get(door_get)
        # Check the return code
        if(api_return is 'No'):
            # The node is offline or not connected
            self.open_door_alert()
        else:
            # Prepare the second HTTP_GET api call
            door_get = nodes_api.get_url(1) + "/opendoor?pass=" + str(door_otp)
            # Executes the call
            api_return = nodes_api.url_get(door_get)
            # Check the return code
            print(api_return)
            if(api_return is 'No'):
                # The node is offline or not connected
                self.open_door_alert()
            else:
                if('esp8266' in str(api_return)):
                    self.open_door_opened()
                else:
                    self.open_door_not_open(str(api_return))

        # Enable the buttons
        self.toolButtonOpenDoor.setEnabled(True)
        self.toolButtonNodesHealth.setEnabled(True)

    def open_door_opened(self):
        '''
        Signals to the control center window
        that the door has been opened
        '''
        global nodes_api
        global narrator
        
        # Create the log message
        msg = narrator.getString(8)
        self.log_update(msg)
        # Remove the alert status if it was set before
        self.labelAlertDoorOpener.setText(strings.S_NODE_OK)
        self.t_play_sentence(8)
        self.update_message(msg)

    def open_door_alert(self):
        '''
        Create the alerts on the control center window
        when there is a problem opening the door
        '''
        global nodes_api
        global narrator
        
        # Create the log message
        msg = nodes_api.get_nodename(1) + " " + narrator.getString(2)
        self.log_update(msg)
        # Set the alert message beside the node list
        self.labelAlertDoorOpener.setText(narrator.getString(1))
        self.t_play_sentence(1)
        self.update_message(msg)

    def open_door_not_open(self, api_return):
        '''
        The server is up but the door is not open
        '''
        global nodes_api
        global narrator
        
        # Create the log message
        msg = narrator.getString(9)
        self.log_update(msg)
        self.log_update(api_return)
        # Set the alert message beside the node list
        self.t_play_sentence(9)
        self.update_message(msg)

    def nodes_health(self):
        '''
        Check the LAN nodes health status
        and manage alarms if some node is
        unreachable
        '''
        global nodes_api
        global narrator
        # Temporary disable the buttons until the action is not completed
        self.toolButtonOpenDoor.setEnabled(False)
        self.toolButtonNodesHealth.setEnabled(False)
        # Update the log
        self.log_update(narrator.getString(5))
        # Loop on all the nodes
        n = 0
        while n < nodes_api.get_nnodes():
            # Check if the node is active, else ignore the node
            if(nodes_api.get_status(n)):
                url = nodes_api.get_url(n)
                if(nodes_api.url_check(url) is False):
                    # The node is offline or not connected
                    # Create the log message
                    msg = nodes_api.get_nodename(n) + " " + narrator.getString(2)
                    self.log_update(msg)
                    # Set the alert message beside the node list
                    self.labelAlertDoorOpener.setText(narrator.getString(1))
                    self.t_play_sentence(1)
                else:
                    # Create the log message
                    msg = nodes_api.get_nodename(n) + " " + narrator.getString(7)
                    self.log_update(msg)
                    # Set the alert message beside the node list
                    self.labelAlertDoorOpener.setText("")
            
            n += 1

        # Enable the buttons
        self.toolButtonOpenDoor.setEnabled(True)
        self.toolButtonNodesHealth.setEnabled(True)

    def about(self):
        '''
        Shows the current application version info
        '''
        self.update_message_from_id(10)

'''
TIMELOOP JOBS DEFINITION --------------------------------------
'''

@tl.job(interval = timedelta(seconds = TIME_TICK))
def time_update():
    '''
    Update the clock label on the UI
    '''
    mainWin.time_otp_set()

@tl.job(interval = timedelta(seconds = OTP_TICK))
def OTP_update():
    '''
    Update the otp pin
    '''
    global otp
    otp_create()

'''
FUNCTIONS ------------------------------------------------------
'''

def otp_create():
    '''
    Generate a new random otp pin.
    
    The otp pin is OTP_LEN characters and is refreshed by
    a Timelopp job.
    '''
    global otp
    
    alphabet = string.digits
    otp = ''.join(secrets.choice(alphabet) for i in range(OTP_LEN))

'''
MAIN ------------------------------------------------------------
'''

if __name__ == '__main__':
    '''
    Main function
    Launch the UI loop accordingly with the QApplication directives
    and exit leaving the control to the MainWindow class
    Note: it is important that the Timeloop threaded jobs are started
    before the execution of the UI loop.
    '''
    # Start the UI
    app = QApplication(sys.argv)
    mainWin = MainWindow()
    # Define the startup otp pin
    otp_create()
    # Start the Timeloop timer
    tl.start()
    # Complete the UI loop and exits
    ret = app.exec_()
    sys.exit( ret )
    





