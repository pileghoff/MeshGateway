#! python3
import gateway
import db
import config
import prov
import time

default_setup2 = [{"name":"3",
                   'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\xb6\x00\x00\x00\x00\x00\x00\x00\x00',
                   'oob': b'\xbc\xdd\xc2\xd1\xe3\xb6',
                   'subs':[],
                   "pub":{},
                   "status":0},
                  {"name":"4",
                   'uuid': b'\xdd\xdd\xbc\xdd\xc2\xd1\xe2\xa6\x00\x00\x00\x00\x00\x00\x00\x00',
                   'oob': b'\xbc\xdd\xc2\xd1\xe2\xa6',
                   'subs':[],
                   "pub":{},
                   "status":0},
                  {"name":"2",
                   "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xdc\xae\x00\x00\x00\x00\x00\x00\x00\x00',
                   "oob": b'\xbc\xdd\xc2\xd1\xdc\xae',
                   'subs':[],
                   "pub":{},
                   "status":0},
                  {"name":"1",
                   "uuid": b'\xdd\xdd\xbc\xdd\xc2\xd1\xe3\x8a\x00\x00\x00\x00\x00\x00\x00\x00',
                   "oob": b'\xbc\xdd\xc2\xd1\xe3\x8a',
                   'subs':[],
                   "pub":{},
                   "status":0}]

def menu(options):
    while True:
        print("Select an option")
        for i, option in enumerate(options):
            print(f"({i}) {option}")
        print()

        raw = input("")
        try:
            number = int(raw)
            if number < 0 or number > len(options):
                print("Number out of range")
            else:
                return int(raw)
        except ValueError:
            print("Option not a number")


def list_devices():
    devs = db.get_devices()
    for dev in devs:
        print(f"Device {dev['name']}")
        print(f"\t Status: {db.Status(dev['status']).name}")

def provision_device():
    devs = [dev for dev in db.get_devices() if dev['status'] == db.Status.Unprov]
    if len(devs) == 0:
        print("No devices ready to provision")
    else:
        print("Which device to provision?")
        device_names = ["Device " + str(dev['name']) for dev in devs]
        device_names.append("All")
        option = menu(device_names)
        if option == len(device_names)-1:
            for dev in devs:
                prov.prov_device(dev['uuid'], dev['oob'])
        else:
            prov.prov_device(devs[option]['uuid'], devs[option]['oob'])

def config():
    devs = [dev for dev in db.get_devices() if dev['status'] => db.Status.Prov]
    if len(devs) == 0:
        print("No devices ready to configure")
    else:
        print("Which device to configure?")
        dev_option = menu(devs)
        dev = devs[dev_option]
        conf_option = -1
        while conf_options != 2:
            conf_option = menu(["Set sub", "Set pub", "Done"])
            if conf_option == 0:
                subs_raw = input("Comma seperated subs:")
                subs = [int(s) for s in subs_raw]
                dev['subs'] = subs

            if conf_option == 1:
                pub_raw = input("pub:")
                dev['subs'] = int(pub_raw)

        db.device_change_config(dev)
        config.key_add(dev['unicast'])
        config.sub_del(dev['unicast'], sub[0]['model'])

        for sub in dev['subs']:
            config.key_bind(dev['unicast'], sub['model'])
            config.sub_add(dev['unicast'], sub['addr'], sub['model'])

        config.key_bind(dev['unicast'], ['model'])
        config.pub_set(dev['unicast'], pub['addr'], pub['model'])
        db.device_configured(dev['uuid'])


def main_menu():
    option = menu(["List devices", "Provision device", "Configure device"])
    if option == 0:
        list_devices()
    elif option == 1:
        provision_device()


if __name__ == "__main__":
    db.reset()
    db.init_network(default_setup2)
    gateway.client.loop_start()
    while True:
        main_menu()






