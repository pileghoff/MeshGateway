IMAGE=gateway_env
CONTAINER=gateway_build_env

args=${1-all}

docker_run=docker exec -it \
						$(CONTAINER) \
            bash -c

docker_start= docker run --rm -it \
						--name $(CONTAINER)  \
						--mount type=bind,source=${PWD}/src,target=/src \
						--privileged \
						-v /dev/bus/usb:/dev/bus/usb \
						-t -d \
            ${IMAGE}

#docker_init=source /esp/esp-idf/add_path.sh && . /esp/esp-idf/export.sh
docker_init=source /esp/esp-idf/add_path.sh
#docker_init=. /esp/esp-idf/export.sh

.PHONY: build
build:
	$(docker_run) '$(docker_init) && cd /src && idf.py build'


.PHONY: flash
flash:
	$(docker_run) '$(docker_init) && cd /src && idf.py flash -p /dev/ttyUSB0'


.PHONY: clean
clean:
	$(docker_run) '$(docker_init) && cd /src && idf.py erase_flash'


.PHONY: monitor
monitor:
	$(docker_run) '$(docker_init) && cd /src && idf.py monitor'

.PHONY: menuconfig
menuconfig:
	$(docker_run) '$(docker_init) && cd /src && idf.py menuconfig'

.PHONY: env
env:
	$(docker_run) '$(docker_init) && bash'

.PHONY: docker
docker:
	docker stop $(CONTAINER)
	docker build -t $(IMAGE) -f Dockerfile .
	$(docker_start)

.PHONY: test
test:
	$(docker_run) '$(docker_init) && cd /src/test && idf.py clean && idf.py build && idf.py monitor'
