#! python3
import gateway
import db
import config
import prov
import time
import datetime



default_setup = [{"name":"3",
                   'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\xb6\x00\x00\x00\x00\x00\x00\x00\x00',
                   'oob': b'\xbc\xdd\xc2\xd1\xe3\xb6',
                   'subs':[{'addr':0xC000, 'model':0x1000}],
                   "pub":{'addr':0xC000, 'model':0x1001},
                   "status":0},
                  {"name":"4",
                   'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe2\xa6\x00\x00\x00\x00\x00\x00\x00\x00',
                   'oob': b'\xbc\xdd\xc2\xd1\xe2\xa6',
                   'subs': [{'addr':0xC000, 'model':0x1000}, {'addr':0xC003, 'model':0x1000}],
                   "pub":{'addr':0xC000, 'model':0x1001},
                   "status":0},
                  {"name":"2",
                   "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xdc\xae\x00\x00\x00\x00\x00\x00\x00\x00',
                   "oob": b'\xbc\xdd\xc2\xd1\xdc\xae',
                   'subs':[{'addr':0xC000, 'model':0x1000}, {'addr':0xC003, 'model':0x1000}],
                   "pub":{'addr':0xC002, 'model':0x1001},
                   "status":0},
                  {"name":"1",
                   "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\x8a\x00\x00\x00\x00\x00\x00\x00\x00',
                   "oob": b'\xbc\xdd\xc2\xd1\xe3\x8a',
                   'subs':[{'addr':0xC000, 'model':0x1000}, {'addr':0xC002, 'model':0x1000}],
                   "pub":{'addr':0xC000, 'model':0x1001},
                   "status":0}]

def setup_network_loop():
    devs = db.get_devices()
    for dev in devs:

        time.sleep(2)
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
    testname = input("Testname?")
    with open("testlog.txt", "a") as f:
        f.write("\n\n")
        f.write("##################\n")
        f.write(testname)
        f.write("\n##################\n")
        f.flush()
        try:
            for i in range(20):
                start_time = datetime.datetime.now()
                db.reset()
                db.init_network(default_setup)
                gateway.client.loop_start()
                setup_network()

                new_setup = default_setup
                new_setup[0]['pub']['addr'] = 0xC001
                new_setup[1]['pub']['addr'] = 0xC001
                new_setup[2]['pub']['addr'] = 0xC001
                new_setup[3]['pub']['addr'] = 0xC001


                new_setup[0]['subs'] = [{"addr":0xC001, 'model':0x1000}]
                new_setup[1]['subs'] = [{"addr":0xC001, 'model':0x1000}]
                new_setup[2]['subs'] = [{"addr":0xC001, 'model':0x1000}]
                new_setup[3]['subs'] = [{"addr":0xC001, 'model':0x1000}]

                for dev in new_setup:
                    config.sub_del(db.get_unicast(dev['uuid']), 0x1000)

                db.device_change_config(new_setup)
                setup_network()

                total_time = datetime.datetime.now() - start_time
                f.write(str(total_time.total_seconds()))
                f.write("\n")
                f.flush()
                reset_network()
        except Exception:
            reset_network()
