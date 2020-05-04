from pymongo import MongoClient
from paho.mqtt.client import *
from threading import Event
from enum import IntEnum

client = MongoClient("localhost", 27017)
db = client['db1']
device_db = db.device

class Status(IntEnum):
    NotFound=0
    Unprov=1
    Prov=2
    Configured=3
    Failed=4

def reset():
    device_db.delete_many({})

def init_network(setup):
    for dev in setup:
        device_db.insert_one(dev)

def device_change_config(dev):
    if type(dev) is list:
        l = []
        for d in dev:
            l.append(device_change_config(d))
        return l
    else:
        device_db.update_one({"uuid":dev["uuid"]},
                         {"$set":{"status": Status.Prov,
                                  "pub": dev["pub"],
                                  "subs": dev["subs"]}})
def reset_device(unicast):
    device_db.update_one({"unicast":unicast},
    {"$set":{"status":Status.NotFound}})

def remove_device(uuid):
    res = device_db.remove({"uuid":uuid})

def get_devices():
    return list(device_db.find())

def device_found(uuid):
    doc = device_db.find_one({"uuid":uuid})
    if doc:
        device_db.update_one({"uuid":uuid},
                         {"$set":{"status": Status.Unprov}})
    else:
        pass
        #print(f"Device not found: uuid {uuid} oob {uuid[2:8]}")

def device_failed(uuid):
    device_db.update_one({"uuid":uuid},
                         {"$set":{"status": Status.Failed}})

def device_prov(uuid, unicast):
    res = device_db.find_one({"uuid":uuid})
    res1 = device_db.update_one({"uuid":uuid},
                               {"$set": {"unicast":unicast}})
    res2 = device_db.update_one({"uuid":uuid},
                         {"$set": {"status": Status.Prov}})


def device_configured(uuid):
    device_db.update_one({"uuid":uuid},
                         {"$set":{"status": Status.Configured}})

def get_unicast(uuid):
    doc = device_db.find_one({"uuid":uuid})
    return doc['unicast']

def device_get_subs(uuid):
    doc = device_db.find_one({"uuid":uuid})
    return list(doc["subs"])

def device_get_pub(uuid):
    doc = device_db.find_one({"uuid":uuid})
    return doc["pub"]
