#include <linux/module.h>
#include <linux/fs.h> //open, write,alloc_chdev_region, fops,...
#include <linux/uaccess.h> //copy_from...
#include <linux/cdev.h> //khoi tao, xoa device
#include <linux/device.h> //tao class, device
#include <linux/of.h> //doc thong tin tu DT
#include <linux/of_gpio.h>  //gpio_is_valid, request, free,drirection_output, set_value
#include <linux/gpio.h> //request, free,drirection_output, set_value
#include <linux/platform_device.h>

#define DEVICE_NAME "my_btn"
#define CLASS_NAME  "my_btn_class"

struct gpio_btn{
   dev_t dev_num;
   struct device *dev;
   struct cdev cdev;
   int gpio;
   int state;
};

static struct class *my_btn_class;
static struct gpio_btn btn_dev = {
   .gpio = -1,
   .state = 0,  
};

//Character device operate
static int btn_open(struct inode *inode, struct file *file){
   pr_info("opened btn device\n");
   return 0;
}

static int btn_release(struct inode *inode, struct file *file){
   pr_info("released btn device\n");
   return 0;
}

static ssize_t btn_read(struct file *file, char __user *buf, size_t len, loff_t *off){
   char dbuf = gpio_get_value(btn_dev.gpio) ? '1' : '0';

   if (copy_to_user(buf, &dbuf, 1))
      return -EFAULT;
   return 1;
}


static const struct file_operations btn_fops = {
   .owner = THIS_MODULE,
   .open = btn_open,
   .release = btn_release,
   .read = btn_read,
};

//Platform driver function
static int btn_probe(struct platform_device *pdev){
   struct device *dev = &pdev->dev;
   btn_dev.gpio = of_get_named_gpio(dev->of_node, "gpios", 0); //lay gpio tu DT
   
   //kiem tra va yeu cau su dung gpio
   if (!gpio_is_valid(btn_dev.gpio)) {
      dev_err(dev, "Invalid button GPIO\n");
      return -EINVAL;
   }
   if (gpio_request(btn_dev.gpio, "btn_gpio")) {
      dev_err(dev, "Failed to request button GPIO\n");
      return -EBUSY;
   }

   gpio_direction_input(btn_dev.gpio); //set gpio lam input

   alloc_chrdev_region(&btn_dev.dev_num, 0, 1, DEVICE_NAME);
   cdev_init(&btn_dev.cdev, &btn_fops);
   cdev_add(&btn_dev.cdev, btn_dev.dev_num, 1);

   my_btn_class = class_create(THIS_MODULE, CLASS_NAME);
   btn_dev.dev = device_create(my_btn_class, NULL, btn_dev.dev_num, NULL, DEVICE_NAME);

   dev_info(dev, "Button driver probed\n");
   return 0;
}

static int btn_remove(struct platform_device *pdev) {
   
   gpio_free(btn_dev.gpio);

   device_destroy(my_btn_class, btn_dev.dev_num);
   class_destroy(my_btn_class);
   cdev_del(&btn_dev.cdev);
   unregister_chrdev_region(btn_dev.dev_num, 1);

   pr_info("Button driver removed\n");
   return 0;
}

//DT match table
static const struct of_device_id btn_dt_ids[] = {
   {.compatible = "myFPT,my-button"},
   {},
};
MODULE_DEVICE_TABLE(of, btn_dt_ids);


//Register platform driver
static struct platform_driver btn_driver ={
   .probe = btn_probe,
   .remove = btn_remove,
   .driver = {
      .name = DEVICE_NAME,
      .of_match_table = btn_dt_ids,
   },
};

module_platform_driver(btn_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("duongdt65");
MODULE_DESCRIPTION("Combined Platform + Character device diver for Button GPIO");
