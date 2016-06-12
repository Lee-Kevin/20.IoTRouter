import requests
import time
import logging
logging.basicConfig(level='INFO')
class UpdateSensor:
    url = "https://api.mediatek.com/mcs/v2/devices/DBPY02ol/datapoints.csv"

    payload = ""
    headers = {
        'devicekey': "Gi7LGQpVbqzVYItK",
        'cache-control': "no-cache",
        'content-type': "text/csv",
        'connection': "close",
        # 'postman-token': "258188c6-e125-ff88-1af1-b6e46e03898d"
        }
    def __init__(self):
        pass
    def update(self,RouterTemp,RouterHumi):
        self.payload = "RouterTemperature,," + RouterTemp
        response = requests.request("POST", self.url, data=self.payload, headers=self.headers) 
        self.payload = "RouterHumidity,," + RouterHumi
        response = requests.request("POST", self.url, data=self.payload, headers=self.headers)  
if __name__ == '__main__':
    sensor = UpdateSensor()
    while True:
        try:
            f = open("/tmp/Sensor/temp")
            temp = f.read()
            logging.info("The temp :%s" % str(temp))
            f.close()
            f = open("/tmp/Sensor/humi")
            humi = f.read()
            logging.info("The humi :%s" % str(humi))
            f.close()
            sensor.update(str(temp),str(humi))
        except:
            logging.info("There is something wrong")        
        time.sleep(2)

        