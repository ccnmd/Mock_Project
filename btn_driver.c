#include <linux/module.h>
#include <linux/fs.h> //open, write,alloc_chdev_region, fops,...
#include <linux/uaccess.h> //copy_from...
#include <linux/cdev.h> //khoi tao, xoa device
#include <linux/device.h> //tao class, device
#include <linux/of.h> //doc thong tin tu DT
#include <linux/gpio/consumer.h>  //gpiod,...
//#include <linux/gpio.h> //request, free,drirection_output, set_value
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>

#define DEVICE_NAME "my_btn"
#define CLASS_NAME  "my_btn_class"

struct gpio_btn{
   dev_t dev_num;
   struct device *dev;
   struct cdev cdev;
   int gpio_desc *desc;
   int irq;
   struct delayed_work work;
   bool last_state;
   wait_queue_head_t wait;
   bool event_flag;
};

static struct class *my_btn_class;
static struct gpio_btn btn_dev = {
   .desc = NULL,
   //.state = 0,  
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
   bool val;
   
   wait_event_interruptible(btn_dev.wait, btn_dev.event_flag);
   val = btn_dev.last_state;
   btn_dev.event_flag = false;

   if (copy_to_user(buf, val ? "1" : "0", 1))
      return -EFAULT;
   return 1;
}


static const struct file_operations btn_fops = {
   .owner = THIS_MODULE,
   .open = btn_open,
   .release = btn_release,
   .read = btn_read,
};

//workqueue func goi sau khi debounce
static void btn_work_fn(struct work_struct *work){
	struct gpio_btn *b = container_of(to_delayed_work(work), struct gpio_btn, work);
	bool val = gpio_get_value(b->desc);
	
	if(val != b->last_state){
		b->last_state = val;
		b->event_flag = true;
		wake_up_interruptible(&b->wait);
		pr_info("btn_gpio state changed to %d\n", val);
	}
}

//interrupt handler chi de schedule vao workqueue
static irqreturn_t btn_irq_handler(int irq, void *dev){
	schedule_delayed_work(&btn_dev.work, msecs_to_jiffies(20));
	return IRQ_HANDLED;
}

//Platform driver function
static int btn_probe(struct platform_device *pdev){
   struct device *dev = &pdev->dev;
   
   btn_dev.desc = devm_gpiod_get(dev, NULL, GPIOD_IN);
   
   //kiem tra va yeu cau su dung gpio
   if(IS_ERR(btn_dev.desd)){
      dev_err(dev, "failed to get gpio descriptor\n");
      return PTR_ERR(btn_dev.desc);
   }

   //gpio_direction_input(btn_dev.gpio); //set gpio lam input

   
   btn_dev.irq = gpio_to_irq(btn_dev.desc);
   if (btn_dev.irq < 0) return dev_err_probe(dev, btn_dev.irq, "gpio_to_irq failed\n");
   
   INIT_DELAYED_WORK(&btn_dev.work, btn_work_fn);
   init_waitqueue_head(&btn_dev.wait);
   btn_dev.event_flag = false;
   
   if (request_threaded_irq(btn_dev.irq, NULL, btn_irq_handler, IRQF_TRIGGER_RISING | IRQF_ONESHOT, DEVICE_NAME, &btn_dev)) {
      //gpio_free(btn_dev.gpio);
      dev_err(dev, "request_irq failed\n");
      return -EBUSY;
   }
   
   alloc_chrdev_region(&btn_dev.dev_num, 0, 1, DEVICE_NAME);
   cdev_init(&btn_dev.cdev, &btn_fops);
   cdev_add(&btn_dev.cdev, btn_dev.dev_num, 1);

   my_btn_class = class_create(THIS_MODULE, CLASS_NAME);
   btn_dev.dev = device_create(my_btn_class, NULL, btn_dev.dev_num, NULL, DEVICE_NAME);

   dev_info(dev, "Button driver probed\n");
   return 0;
}

static int btn_remove(struct platform_device *pdev) {
   free_irq(btn_dev.irq, &btn_dev);
   cancel_delayed_work_sync(&btn_dev.work);
   //gpio_free(btn_dev.gpio);

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
