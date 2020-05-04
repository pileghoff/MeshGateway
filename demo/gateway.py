from paho.mqtt.client import *
import threading
import time
import db
import prov
import config

client = Client("")

INFO = '\033[92m'
WARNING = '\033[93m'
ERROR= '\033[91m'
ENDC = '\033[0m'

def log(client, userdata, message):
    print(ERROR + message.payload.decode(encoding='UTF-8') + ENDC)

callbacks = {
    "/device/new": prov.new_device,
    "/device/prov/callback": prov.callback,
    "/device/conf/callback": config.callback,
    "/device/log": log,
}

def publish(topic, payload):
    client.publish(topic, payload=payload, qos=2)

def on_connect(client, userdata, flags, rc):
    print("Connection returned result: "+connack_string(rc))

client.username_pw_set("umdzcgpe", "an4TX5LI-3Nm")
client.connect("farmer.cloudmqtt.com", port=16130)
client.on_connect = on_connect

for topic, cb in callbacks.items():
    client.subscribe(topic, qos=2)
    client.message_callback_add(topic, cb)
