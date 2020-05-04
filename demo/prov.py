import gateway
import db
from threading import Event

# Return codes
PROV_RET_OK = 0
PROV_RET_FAIL_1 = 1
PROV_RET_FAIL_2 = 2

lock = Event()
lock.clear()
current_uuid = 0
return_code = 0

def new_device(client, userdata, message):
    uuid = message.payload

    docs = db.get_devices()
    if len([d for d in docs if d['uuid'] == uuid]):
        db.device_found(uuid)
    #else:
        #db.add_device(uuid, uuid[2:8])

def prov_device(uuid, oob):
    global current_uuid

    global return_code
    return_code = -1

    lock.clear()
    current_uuid = uuid
    b = bytearray()
    b.extend(uuid)
    b.extend(oob)
    gateway.publish("/device/prov", b)
    lock.wait(60)

    return return_code


def callback(client, userdata, message):
    global current_uuid
    global return_code
    return_code = message.payload[0]
    if return_code == PROV_RET_OK:
        unicast = int.from_bytes(message.payload[1:], byteorder='little')
        db.device_prov(current_uuid, unicast)
    else:
        print(f"Prov failed: {return_code}")
    lock.set()

