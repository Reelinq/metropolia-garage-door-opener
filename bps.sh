#! /usr/bin/bash

cmake -G Ninja -B build && cmake --build build
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program build/blink.elf verify reset exit"
