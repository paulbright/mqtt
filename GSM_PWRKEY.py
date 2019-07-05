import RPi.GPIO as GPIO
import time
import sys

print(sys.argv[1]) 
pin = int(sys.argv[1])

GPIO.setmode(GPIO.BOARD)
GPIO.setup(pin, GPIO.OUT)
while True:
	GPIO.output(pin, GPIO.LOW)
	time.sleep(4)
	GPIO.output(pin, GPIO.HIGH)
	break
GPIO.cleanup()
