FROM ubuntu:18.10
RUN rm /bin/sh && ln -s /bin/bash /bin/sh

RUN apt-get update && apt-get install -yq git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache clang-tidy

RUN mkdir /esp && mkdir /esp/tools && cd /esp && git clone --recursive -b v4.0-beta2 https://github.com/espressif/esp-idf.git

RUN cd /esp &&  wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz && tar -xzf /esp/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz 

ENV PATH="/esp/xtensa-esp32-elf/bin:${PATH}"

ENV IDF_PATH="/esp/esp-idf"

RUN python -m pip install --user -r $IDF_PATH/requirements.txt
