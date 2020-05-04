#! python3
import gateway
import db
import config
import prov
import time
import datetime
import threading

setup1 = [{"name":"1",
           "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\x8a\x00\x00\x00\x00\x00\x00\x00\x00',
           "oob": b'\xbc\xdd\xc2\xd1\xe3\x8a',
           'subs':[{'addr':0xC001, 'model':0x1000}],
           "pub":{'addr':0xC001, 'model':0x1001},
           "status":0},
          {"name":"2",
           "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xdc\xae\x00\x00\x00\x00\x00\x00\x00\x00',
           "oob": b'\xbc\xdd\xc2\xd1\xdc\xae',
           'subs':[{'addr':0xC002, 'model':0x1000}],
           "pub":{'addr':0xC002, 'model':0x1001},
           "status":0},
          {"name":"3",
           'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\xb6\x00\x00\x00\x00\x00\x00\x00\x00',
           'oob': b'\xbc\xdd\xc2\xd1\xe3\xb6',
           'subs':[{'addr':0xC003, 'model':0x1000}],
           "pub":{'addr':0xC003, 'model':0x1001},
           "status":0},
          {"name":"4",
           'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe2\xa6\x00\x00\x00\x00\x00\x00\x00\x00',
           'oob': b'\xbc\xdd\xc2\xd1\xe2\xa6',
           'subs': [{'addr':0xC004, 'model':0x1000}],
           "pub":{'addr':0xC004, 'model':0x1001},
           "status":0}]

setup2 = [{"name":"1",
           "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\x8a\x00\x00\x00\x00\x00\x00\x00\x00',
           "oob": b'\xbc\xdd\xc2\xd1\xe3\x8a',
           'subs':[{'addr':0xC001, 'model':0x1000}, {'addr':0xC002, 'model':0x1000}, {'addr':0xC003, 'model':0x1000}],
           "pub":{'addr':0xC001, 'model':0x1001},
           "status":0},
          {"name":"2",
           "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xdc\xae\x00\x00\x00\x00\x00\x00\x00\x00',
           "oob": b'\xbc\xdd\xc2\xd1\xdc\xae',
           'subs':[{'addr':0xC002, 'model':0x1000}, {'addr':0xC003, 'model':0x1000}, {'addr':0xC004, 'model':0x1000}],
           "pub":{'addr':0xC002, 'model':0x1001},
           "status":0},
          {"name":"3",
           'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\xb6\x00\x00\x00\x00\x00\x00\x00\x00',
           'oob': b'\xbc\xdd\xc2\xd1\xe3\xb6',
           'subs':[{'addr':0xC003, 'model':0x1000}, {'addr':0xC004, 'model':0x1000}, {'addr':0xC001, 'model':0x1000}],
           "pub":{'addr':0xC003, 'model':0x1001},
           "status":0},
          {"name":"4",
           'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe2\xa6\x00\x00\x00\x00\x00\x00\x00\x00',
           'oob': b'\xbc\xdd\xc2\xd1\xe2\xa6',
           'subs': [{'addr':0xC004, 'model':0x1000}, {'addr':0xC001, 'model':0x1000}, {'addr':0xC002, 'model':0x1000}],
           "pub":{'addr':0xC004, 'model':0x1001},
           "status":0}]

def setup_network_loop():
    devs = db.get_devices()
    for dev in [d for d in devs if d['status'] < db.Status.Configured]:
        print(f"Dev{dev['name']} {db.Status(dev['status']).name}")
        if dev["status"] == db.Status.Unprov:
            prov.prov_device(dev['uuid'], dev['oob'])
        elif dev["status"] == db.Status.Prov:
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

def check_network(check):
    while check.is_set():
        devs = db.get_devices()
        error = False
        for dev in devs:
            if dev['status'] >= db.Status.Prov and not config.is_alive(dev['unicast']):
                db.reset_device(dev['unicast'])
                print(f"Device {dev['name']} is NOT alive")
                error = True
                config.display_error(error)
            if not dev['status'] >= db.Status.Prov:
                error = True
                config.display_error(error)

        config.display_error(error)

        if not all([dev['status'] == db.Status.Configured for
                       dev in db.get_devices()]):
            setup_network()




def setup_network():
    while not all([dev['status'] == db.Status.Configured
                   for dev in db.get_devices()]):
        setup_network_loop()
        time.sleep(1)

def reset_network():
    devs = db.get_devices()
    for dev in devs:
        config.reset(dev['unicast'])


if __name__ == "__main__":
    db.reset()
    db.init_network(setup1)
    gateway.client.loop_start()
    config.display_error(False)
    setup_network()

    input("Network setup done.. Press enter to reconfigure")
    db.device_change_config(setup2)
    setup_network()


    input("Reconfig done.. Press enter to monitor network")
    check_run = threading.Event()
    check_run.set()
    threading.Thread(target=check_network, args=(check_run,)).start()
    input("Monitor done.. Press enter to remove node 1 from network")
    check_run.clear()
    setup_network()


    config.reset(db.get_unicast(setup1[0]['uuid']))
    db.remove_device(setup1[0]['uuid'])

    check_run = threading.Event()
    check_run.set()
    threading.Thread(target=check_network, args=(check_run,)).start()
    input("Press enter to stop monitoring")
    check_run.clear()
