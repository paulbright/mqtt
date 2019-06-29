
import paho.mqtt.publish as publish
import sys

if len(sys.argv) < 4:
	print("usage: publish.py <topic> <msg> <mqttbroker>")
	exit(0)

publish.single(sys.argv[1], sys.argv[2], hostname=sys.argv[3])

