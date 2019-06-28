import paho.mqtt.client as mqtt

client = mqtt.Client("myclient")
client.connect('localhost')
client.publish("paul/temp","45C")