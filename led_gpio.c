#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>

//设计一个类型，描述一个设备的信息
struct led_desc{
	unsigned int dev_major;   //设备号
	struct class *cls;     
	struct device *dev;   //创建设备文件
 };

//定义gpio引脚功能
struct gpio led0 = {
	.gpio = 17,   
	.flags = GPIOF_OUT_INIT_LOW,   //设置为输出且初始值为低
	.label = "led"
};

//定义一个全局的设备对象
struct led_desc *led_dev; 


int open_led_dev(struct inode *ino, struct file *filp)
{
	printk("open led_dev\n");
	return 0;
}

ssize_t write_led_dev(struct file *filp, const char __user *buf, size_t count, loff_t *loff)
{
	int value;
	int ret;
	
	ret = copy_from_user(&value, buf, count);
	if(ret > 0)
	{
		printk("copy_from_user error\n");

		return -EFAULT; 
	}

	if(value == 1){
		printk("led on!\n");
		gpio_set_value(led0.gpio, 1); 
	}else{
		printk("led off!\n");
		gpio_set_value(led0.gpio, 0); 
	}
		
	return 0;
}

struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open  = open_led_dev,
	.write = write_led_dev
};

static int led_dev_init(void)
{
	int ret; 
	
	//实例化全局的设备对象-->分配空间
	led_dev = kmalloc(sizeof(struct led_desc), GFP_KERNEL);
    if(led_dev == NULL)
    {
		printk(KERN_ERR"kmalloc error\n");
		return -ENOMEM;
    }
    
	//一般都是申请设备资源
	
	//申请设备号（设置为0表示有内核自动分配可用设备号）
	led_dev->dev_major = register_chrdev(0, "led_dev", &my_fops); 
	if(led_dev->dev_major < 0)
	{
		printk(KERN_ERR"register_chrdev error\n");
		ret = -ENODEV;
		goto err_0;
	}

	//创建设备文件
	led_dev->cls = class_create(THIS_MODULE, "led_cls");
	if(IS_ERR(led_dev->cls))
	{
		printk(KERN_ERR"class_create error\n");
		ret = PTR_ERR(led_dev->cls);
		goto err_1;
	}

	//  /dev/led0
	led_dev->dev = device_create(led_dev->cls, NULL,
			     	             MKDEV(led_dev->dev_major, 0), NULL, "led%d", 0);
	if(IS_ERR(led_dev->dev))
	{
		printk(KERN_ERR"device_create error\n");
		ret = PTR_ERR(led_dev->dev);
		goto err_2;
	}

	//向内核申请gpio
	ret = gpio_request_one(led0.gpio, led0.flags, led0.label);
	if(ret < 0)
	{
		printk(KERN_ERR"gpio_request error\n");
		goto err_3;
	}
	
	return 0;

	err_3:
		device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	
	err_2:
		class_destroy(led_dev->cls);

	err_1:
		unregister_chrdev(led_dev->dev_major, "led_dev");

	err_0:
		kfree(led_dev);
		return ret;
}

static void led_dev_exit(void)
{
	//一般都是释放资源
	kfree(led_dev);
	unregister_chrdev(led_dev->dev_major, "led_dev");
	class_destroy(led_dev->cls);
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	gpio_free(led0.gpio);
}


module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");






