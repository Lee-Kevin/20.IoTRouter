# This is a net speed determine program
# Using shell cmmand ifocnifg and awk

import subprocess
import time

class NetStatus:
    "This class is used for collect net status data and save it to /tmp "
    last_RX_Bytes = 0;
    cur_RX_Bytes = 0;

    cmd1 = "ifconfig eth0 | grep \"RX\" | awk '{if(NR==1)'print'}' | awk '{print $2}' | awk '{gsub(\"packets:\", \"\");print}'"
    cmd2 = "cat /proc/net/arp | grep : | grep ^192.168.100 | grep 0x2 | awk '{print NR}'"
    def __init__(self,speedpath,numberpath):
        self.speedpath = "/tmp/netSpeed"
        self.numberpath = "/tmp/wifi_user_number"
        
    def get_netSpeed(self):
        last_RX_Bytes = int(subprocess.Popen(self.cmd1, shell=True, stdout=subprocess.PIPE).stdout.read())
        time.sleep(3)
        cur_RX_Bytes = int(subprocess.Popen(self.cmd1, shell=True, stdout=subprocess.PIPE).stdout.read())
        netSpeed = (cur_RX_Bytes - last_RX_Bytes) / 3
#        print netSpeed, "kb/s" 
        return str(netSpeed)
        
    def get_wifi_user_number(self):
        data = subprocess.Popen(self.cmd2, shell=True, stdout=subprocess.PIPE).stdout.read()
        user_number = len(data)/2
#        print user_number
        return str(user_number)

    def saveFile(self,path, data):
        fs = open(path, "wb")
        fs.write(data)
        fs.close()        
    def UpdateNetSpeed(self):
        self.saveFile(self.speedpath, self.get_netSpeed())
        
    def UpdateUserNumber(self):
        self.saveFile(self.numberpath,self.get_wifi_user_number())
        
if __name__=="__main__":
    net = NetStatus("/tmp/netSpeed","/tmp/wifi_user_number")
    while True:
        net.UpdateNetSpeed()
        net.UpdateUserNumber()
      
    
    
    