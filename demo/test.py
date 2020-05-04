#! python3
import gateway
import db
import config
import prov
import time
import serial
import queue
import threading

class TestError(Exception):
    pass

q = queue.Queue()

do_serial= True

def serial_thread():
    ser = serial.Serial("/dev/ttyUSB0", 115200)
    print("Serial thread started")
    while do_serial:
        line = ser.readline().decode('utf-8')
        if line.startswith("$"):
            q.put(line[1:].strip())



default_setup = [{"name":"3",
                  'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe2\xae\x00\x00\x00\x00\x00\x00\x00\x00',
                  'oob': b'\xbc\xdd\xc2\xd1\xe2\xae',
                  'subs':[{'addr':0xC004, 'model':0x1000}],
                  "pub":{'addr':0xC003, 'model':0x1001},
                  "status":0}]

def expect(expected):
    if do_serial:
        actual = q.get()
        if not actual == expected:
            raise TestError(f"Expected {expected}, got {actual}")

    print(expected)

if __name__ == "__main__":
    threading.Thread(target=serial_thread).start()

    db.reset()
    db.init_network(default_setup)
    gateway.client.loop_start()

    dev = db.get_devices()[0]
    while db.Status(dev['status']) == db.Status.NotFound:
        print("Not found")
        time.sleep(2)
        dev = db.get_devices()[0]

    print("Found")

    prov.prov_device(dev['uuid'], dev['oob'])
    expect("link open")
    expect("prov complete")
    expect("link close")

    dev = db.get_devices()[0]

    config.reset(dev['unicast'])
    expect("prov reset")
    wrong_oob = b'\xbc\xdd\xc2\xd1\xe2\x00'

    time.sleep(2)
    db.reset_device(dev['unicast'])
    dev = db.get_devices()[0]
    while db.Status(dev['status']) == db.Status.NotFound:
        print("Not found")
        time.sleep(2)
        dev = db.get_devices()[0]

    print("Found")

    prov.prov_device(dev['uuid'], wrong_oob)
    expect("link open")
    if not q.empty():
        raise TestError(f"Expected no message, got {q.get()}")

    print("Test complete");
