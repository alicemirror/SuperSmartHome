'''
    Plays sentences accoringly with the sentences.json file
    
    Uses the trans bash script to spek audio sentences and optionally
    also provide the textual format of the desired sentence.
    This call access to the sentences json file for the strings
    content.
'''
import time
import subprocess
import json

# List with the messages for voice comments
text_messages = ['']
# Number of text messages
num_messages = 0

class Speak():
    '''
    Class to speak voice comments
    '''
    def __init__(self):
        '''
        Initialization function.
        Load the sentences list from json file
        '''
        global num_messages
        global text_messages
        # Better put the sentences file full path
        sentences_file = "/media/pi/PiDesktop/SuperSmartHome/control_center/classes/sentences.json"
        # Loads the message tracks
        with open(sentences_file) as file:
            dictionary = json.load(file)
        num_messages = dictionary['phrases']
        text_messages = dictionary['list']
    
    def announce(self, message_id):
        '''
        Speak a message from the standard sentences list
        '''
        # Announce the initial message
        txt = text_messages[message_id]
        self.speak(txt)

    def speak(self, message):
        '''
        Speak a generic message
        '''
        # Text-to-speech command and parameters
        # Parameters: -sp = speak, -n = narrator voice (not used)
        TTS = [ '/usr/bin/trans', '-sp' ]
        self.runCmd([TTS[0], TTS[1], message])

    def runCmd(self, cmd):
        '''
        Execute a subprocess command ignoring the return value.
        :param cmd: The bash command with the parameters
        '''
        proc = subprocess.Popen(cmd,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                )
        stdout, stderr = proc.communicate()

    def getString(self, message_id):
        '''
        Return the corresponding sting in text format
        '''
        return text_messages[message_id]