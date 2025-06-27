#!/bin/bash

echo "Loading all GPIO modules..."
insmod gpio_led_platform.ko 
insmod gpio_btn_platform.ko
insmod gpio_led_chrdev.ko
insmod gpio_btn_chrdev.ko
echo "All modules loaded."