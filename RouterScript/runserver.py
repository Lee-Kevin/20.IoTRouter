"""
This script runs the printer application using a development server.
This is a demo by Jiankai.li
"""

from os import environ
import os
from printer import app
from threading import Thread

thread = None
updateThread = None
relayThread = None

if __name__ == '__main__':
    # os.system("python /root/printer/NetStatus.py &")
    # os.system("python /root/printer/relay.py &")
    # os.system("python /root/printer/UpdateSensor.py &")            
    app.run('0.0.0.0', 8000,debug=False)
