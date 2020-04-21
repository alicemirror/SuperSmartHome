'''
Manages the network URL of the nodes and calls the API
specific for every node.

The URL parameters for the connection are defined in the
nodes json file.

The URL access to the nodes is secured through the HTTPS
access with OpenSSL certificate and a double check data
exchange through the https GET and an OTP password generated
by the system.
'''
import subprocess
import json

num_nodes = 0
nodes_dic = ['']

class NodesAPI():
    '''
    Manages the API calls to the nodes end the connection status
    '''
    def __init__(self):
        '''
        Initialization function.
        Load the configuration from json file
        '''
        global num_nodes
        global nodes_dic
        
        # Better put the sentences file full path
        nodes_file = "/media/pi/PiDesktop/SuperSmartHome/control_center/classes/nodes.json"
        # Loads the nodes info
        with open(nodes_file) as file:
            dictionary = json.load(file)
        
        num_nodes = dictionary['nodes']
        nodes_dic = dictionary['nodes_info']
    
    def get_url(self, node_id):
        '''
        Get the URL of the requested node
        '''
        global nodes_dic
        
        node_content = nodes_dic[node_id]
        return node_content['URL']
    
    def get_status(self, node_id):
        '''
        Get the current status of the requested node.
        If the status is 0 return False as it is not
        enabled, else return true
        '''
        global nodes_dic
        
        node_content = nodes_dic[node_id]
        
        if(node_content['active'] == 1):
            return True
        else:
            return False
        
    def get_nodename(self, node_id):
        '''
        Get the name of the requested node.
        '''
        global nodes_dic
        
        node_content = nodes_dic[node_id]
        return node_content['name']
    
    def get_nnodes(self):
        '''
        Get the number of nodes in the json file
        '''
        return num_nodes
    
    def url_get(self, url):
        '''
        Executes a get to the node with error checking.
        
        Note that the URL should be the complete requrest
        including the API name and parameters, if any
        
        Return the URL response (stdout) of False if the
        remote server is unreachable.
        '''
        # Fixed command name and parameters
        CURL = [ 'curl', '--insecure']
        # Compose the command
        cmd = [CURL[0], CURL[1], url]
        # Executes the call to the subprocess
        proc = subprocess.Popen(cmd,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                )
        stdout, stderr = proc.communicate()

        # Check if the request as been completed
        if(proc.returncode is 0):
            return stdout
        else:
            return 'No'

    def url_check(self, url):
        '''
        Check for the requested server url. if there is not response
        (timeout) returns False else returns True.
        '''
        # Fixed command name and parameters
        CURL = [ 'curl',
                '--insecure']
        # Compose the command
        cmd = [CURL[0], CURL[1], url]

        proc = subprocess.Popen(cmd,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                )
        stdout, stderr = proc.communicate()

        if(proc.returncode is 0):
            return True
        else:
            return False

