#!/bin/bash

echo "Unloading all GPIO modules..."
rmmod gpio_btn_chrdev
rmmod gpio_btn_platform
rmmod gpio_led_platform
echo "All modules unloaded."
 