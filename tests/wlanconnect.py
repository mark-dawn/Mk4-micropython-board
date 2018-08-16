print("Running Wlan Connect Test")
from network import WLAN
from time import sleep
print("Starting WLAN")
wlan = WLAN(0)
wlan.active(True)
print("Scanning")
print(wlan.scan())
sleep(1)
print(wlan.scan())
sleep(1)
print("Connecting...")
wlan.connect("MYAP", "password", autoconnect=False)
sleep(1)
print("connected status is ")
print(wlan.isconnected())
print("ifconfig status is ")
sleep(1)
print(wlan.ifconfig())
print("done")
