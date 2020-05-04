import gateway
from threading import Event
import db
import time
# Return codes
CONF_RET_OK = 0
CONF_RET_TIMEOUT = 1

lock = Event()

MODEL_ONOFF_SRV = 0x1000
MODEL_ONOFF_CLI = 0x1001

return_code = 0

def sub_del(addr, model_id):
    global return_code
    return_code = 1
    while return_code == 1:
        print("sub del")
        lock.clear()

        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        if type(model_id) is not bytes:
            model_id = model_id.to_bytes(2, byteorder="little")

        b.extend(addr)
        b.extend(model_id)

        gateway.publish("/device/conf/sub/del", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def sub_add(addr, sub_addr, model_id):
    global return_code
    return_code = 1
    while return_code == 1:
        print("sub add")
        lock.clear()
        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        if type(sub_addr) is not bytes:
            sub_addr = sub_addr.to_bytes(2, byteorder="little")

        if type(model_id) is not bytes:
            model_id = model_id.to_bytes(2, byteorder="little")

        b.extend(addr)
        b.extend(sub_addr)
        b.extend(model_id)

        gateway.publish("/device/conf/sub/add", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def pub_set(addr, pub_addr, model_id):
    global return_code
    return_code = 1
    while return_code == 1:

        print("pub add")
        lock.clear()
        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        if type(pub_addr) is not bytes:
            pub_addr = pub_addr.to_bytes(2, byteorder="little")

        if type(model_id) is not bytes:
            model_id = model_id.to_bytes(2, byteorder="little")

        b.extend(addr)
        b.extend(pub_addr)
        b.extend(model_id)

        gateway.publish("/device/conf/pub", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def reset(addr):
    global return_code
    return_code = 1
    while return_code == 1:

        db.reset_device(addr)
        lock.clear()
        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        b.extend(addr)

        gateway.publish("/device/conf/reset", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def comp_get(addr):
    global return_code
    return_code = 1
    while return_code == 1:
        print("comp get")
        lock.clear()
        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        b.extend(addr)

        gateway.publish("/device/conf/comp/get", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def is_alive(addr):
    global return_code
    b = bytearray()
    if type(addr) is not bytes:
        addr = addr.to_bytes(2, byteorder="little")

    b.extend(addr)

    for i in range(2):
        lock.clear()
        return_code = -1
        gateway.publish("/device/conf/comp/get", b)
        lock.wait(10)
        if return_code == 0:
            return True
        else:
            print("Timeout", i, "   return", return_code)
    return return_code == 0

def display_error(err):
    global return_code
    return_code = -1
    lock.clear()
    b = bytearray()
    if type(err) is not bytes:
        err = err.to_bytes(1, byteorder="little")

    b.extend(err)

    gateway.publish("/device/error", b)
    lock.wait(10)
    print("Display error:", return_code == 0)
    return return_code == 0

def key_add(addr):
    global return_code
    return_code = 1
    while return_code == 1:
        print("app key add")
        lock.clear()
        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        b.extend(addr)

        gateway.publish("/device/conf/key/add", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def key_bind(addr, model_id):
    global return_code
    return_code = 1
    while return_code == 1:
        print("app bind")
        lock.clear()
        b = bytearray()
        if type(addr) is not bytes:
            addr = addr.to_bytes(2, byteorder="little")

        if type(model_id) is not bytes:
            model_id = model_id.to_bytes(2, byteorder="little")

        b.extend(addr)
        b.extend(model_id)

        gateway.publish("/device/conf/key/bind", b)
        lock.wait(10)

    if return_code == -1:
        db.reset_device(addr)

def callback(client, userdata, message):
    global return_code
    return_code = int.from_bytes(message.payload, byteorder='little')
    print(f"Config callback: {return_code}")
    lock.set()
