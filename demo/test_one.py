#! python3
import gateway
import db
import config
import prov
import time
import datetime



# default_setup = [{"name":"1",
#                    "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\x8a\x00\x00\x00\x00\x00\x00\x00\x00',
#                    "oob": b'\xbc\xdd\xc2\xd1\xe3\x8a',
#                    'subs':[{'addr':0xC000, 'model':0x1000}, {'addr':0xC002, 'model':0x1000}],
#                    "pub":{'addr':0xC000, 'model':0x1001},
#                    "status":0}]

default_setup = [{"name":"3",
                   'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\xb6\x00\x00\x00\x00\x00\x00\x00\x00',
                   'oob': b'\xbc\xdd\xc2\xd1\xe3\xb6',
                   'subs':[{'addr':0xC000, 'model':0x1000}],
                   "pub":{'addr':0xC000, 'model':0x1001},
                   "status":0}]

def setup_network_loop():
    devs = db.get_devices()
    for dev in devs:
        print(f"Dev{dev['name']} {db.Status(dev['status']).name}")
        if dev["status"] == db.Status.Unprov:
            prov.prov_device(dev['uuid'], dev['oob'])

        if dev["status"] == db.Status.Prov:
            print(f"Dev subs: {str(db.device_get_subs(dev['uuid']))}")
            #config.comp_get(dev['unicast'])
            config.key_add(dev['unicast'])
            #config.sub_del(dev['unicast'], dev['sub']sub[0]['model'])

            for sub in db.device_get_subs(dev['uuid']):
                config.key_bind(dev['unicast'], sub['model'])
                config.sub_add(dev['unicast'], sub['addr'], sub['model'])


            print(f"Dev pub: {str(db.device_get_pub(dev['uuid']))}")
            pub = db.device_get_pub(dev['uuid'])
            config.key_bind(dev['unicast'], pub['model'])
            config.pub_set(dev['unicast'], pub['addr'], pub['model'])
            db.device_configured(dev['uuid'])

def check_network():
    devs = db.get_devices()
    for dev in devs:
        if dev['status'] >= db.Status.Prov and config.is_alive(dev['unicast']):
            print(f"Device {dev['name']} is alive")
        else:
            print(f"Device {dev['name']} is NOT alive")



def setup_network():
    while not all([dev['status'] == db.Status.Configured for dev in db.get_devices()]):
        setup_network_loop()
        time.sleep(1)

def reset_network():
    devs = db.get_devices()
    for dev in devs:
        config.reset(dev['unicast'])


if __name__ == "__main__":
    while True:
        db.reset()
        db.init_network(default_setup)
        gateway.client.loop_start()
        setup_network()
        reset_network()
