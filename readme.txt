setup

RPI -> installed mosquitto, node-red, ThingStream RPI C SDK 
mosquitto on localhost:1883 
node-red on localhost:1880


repositories in github -> pubtots, mqtt

open node-red in RPI and import  node_red_flow_to_TS_using_template.txt

this flow listens on local mqtt topic paul/rpi and uses a template node to create parameters for pubtots program 
and executes it using exec node.


parameters created by template node ->  test/paul "{{payload}}" /dev/ttyUSB0 
(test/paul is an MQTT topic in TS)
(payload is the msg send to local mqtt topic paul/rpi) 


this get passed to exec node
exec node executes pubtots 
pubtots command line
 
pubtots test/paul "{{payload}}" /dev/ttyUSB0


here pubtots forwards  payload sent to local mqtt topic paul/rpi to TS topic test/paul using the 
GSM hat attached to /dev/ttyUSB0

you will get segmentation fault error if the gsm hat is not available in the given port

 
below is the flow

at local RPI run
mosquitto_pub -t paul/rpi -m "your msg to be send to TS" -> 
	node-red-flow -> pass msg from paul/rpi to template node ->
		template node adds parameters and passes to exec node ->
			exec node runs pubtots with parameters passed to it ->
				pubtots sends msg to test/paul topic in ThingStream  


compiling pubtots

clone github repo pubtots
cd to pubtots folder
run $make
source code of pubtots is in pubtots/src
executable file is in pubtots/out
executable file name is pubtots which is in pubtots/out
chmod +x pubtots


to auto power on gsm hat
install gsm hat on gpio
to switch on gsm hat execute 

python GSM_PWRKEY.py 7
port will be on /dev/ttyS0 or similar 

