#include <linux/module.h>
#include <linux/fs.h> //open, write,alloc_chdev_region, fops,...
#include <linux/uaccess.h> //copy_from...
#include <linux/cdev.h> //khoi tao, xoa device
#include <linux/device.h> //tao class, device
#include <linux/of.h> //doc thong tin tu DT
#include <linux/of_gpio.h>  //gpio_is_valid, request, free,drirection_output, set_value
#include <linux/gpio.h> //request, free,drirection_output, set_value
#include <linux/platform_device.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "my_led"
#define CLASS_NAME  "my_led_class"

#define LED_IOCTL_MAGIC  'L'
#define LED_IOCTL_TOGGLE  _IO(LED_IOCTL_MAGIC,0)


struct gpio_led{
   dev_t dev_num;
   struct device *dev;
   struct cdev cdev;
   int gpio;
   int state;
};

static struct class *my_led_class;
static struct gpio_led led_dev = {
   .gpio = -1,
   .state = 0,  
};

//Character device operate
static int led_open(struct inode *inode, struct file *file){
   pr_info("opened led device\n");
   return 0;
}

static int led_release(struct inode *inode, struct file *file){
   pr_info("released led device\n");
   return 0;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t len, loff_t *off){
   char dbuf;

   if(copy_from_user(&dbuf, buf, 1))
      return -EFAULT;

   if(dbuf == '1'){
      gpio_set_value(led_dev.gpio, 1);
      led_dev.state = 1;
   }else if(dbuf == '0'){
      gpio_set_value(led_dev.gpio, 0);
      led_dev.state = 0;
   }
   return 1;
}

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
   switch(cmd){
      case LED_IOCTL_TOGGLE:
         led_dev.state = !led_dev.state;
         gpio_set_value(led_dev.gpio, led_dev.state);
         break;
      default:
         return -EINVAL;
   }
   return 0;
}

static const struct file_operations led_fops = {
   .owner = THIS_MODULE,
   .open = led_open,
   .release = led_release,
   .write = led_write,
   .unlocked_ioctl = led_ioctl,
};

//Platform driver function
static int led_probe(struct platform_device *pdev){
   struct device *dev = &pdev->dev;
   led_dev.gpio = of_get_named_gpio(dev->of_node, "gpios", 0); //lay gpio tu DT
   
   //kiem tra va yeu cau su dung gpio
   if (!gpio_is_valid(led_dev.gpio)) {
      dev_err(dev, "Invalid LED GPIO\n");
      return -EINVAL;
   }
   if (gpio_request(led_dev.gpio, "led_gpio")) {
      dev_err(dev, "Failed to request LED GPIO\n");
      return -EBUSY;
   }

   gpio_direction_output(led_dev.gpio, 0); //set gpio lam output va muc logic ban dau la 0

   alloc_chrdev_region(&led_dev.dev_num, 0, 1, DEVICE_NAME);
   cdev_init(&led_dev.cdev, &led_fops);
   cdev_add(&led_dev.cdev, led_dev.dev_num, 1);

   my_led_class = class_create(THIS_MODULE, CLASS_NAME);
   led_dev.dev = device_create(my_led_class, NULL, led_dev.dev_num, NULL, DEVICE_NAME);

   dev_info(dev, "Led driver probed\n");
   return 0;
}

static int led_remove(struct platform_device *pdev) {
   gpio_set_value(led_dev.gpio, 0);
   gpio_free(led_dev.gpio);

   device_destroy(my_led_class, led_dev.dev_num);
   class_destroy(my_led_class);
   cdev_del(&led_dev.cdev);
   unregister_chrdev_region(led_dev.dev_num, 1);

   pr_info("Led driver removed\n");
   return 0;
}

//DT match table
static const struct of_device_id led_dt_ids[] = {
   {.compatible = "myFPT,my-led"},
   {},
};
MODULE_DEVICE_TABLE(of, led_dt_ids);


//Register platform driver
static struct platform_driver led_driver ={
   .probe = led_probe,
   .remove = led_remove,
   .driver = {
      .name = DEVICE_NAME,
      .of_match_table = led_dt_ids,
   },
};

module_platform_driver(led_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("duongdt65");
MODULE_DESCRIPTION("Combined Platform + Character device diver for LED GPIO");
