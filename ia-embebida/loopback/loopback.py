import serial
import time

PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200

serial_port = serial.Serial(PORT, BAUD_RATE, timeout=0.25)
serial_port.flushInput()

for i in range(10):
    serial_port.write(str.encode(f"Test {i + 1}\n"))
    time.sleep(1)

serial_port.close()
