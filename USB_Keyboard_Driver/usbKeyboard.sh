#!/bin/bash
sudo echo -n "2-6:1.0" > /sys/bus/usb/drivers/usbhid/unbind
sudo echo -n "2-6:1.0" > /sys/bus/usb/drivers/usbkbd/bind
