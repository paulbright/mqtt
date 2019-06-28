
import paho.mqtt.publish as publish

publish.single("paul/temp", "50C", hostname="localhost")