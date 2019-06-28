import paho.mqtt.client as mqtt #import the client1
import time
############
def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)
########################################
topic = 'paul/temp'
broker_address="localhost"
print("creating new instance")
client = mqtt.Client("PaulClient") #create new instance
client.on_message=on_message #attach function to callback
print("connecting to broker")
client.connect(broker_address) #connect to broker
client.loop_start() #start the loop
print("Subscribing to topic", topic)
client.subscribe(topic)
#print("Publishing message to topic",topic)
#client.publish(topic,"50C")
time.sleep(10) # wait
client.loop_stop() #stop the loop