#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include "omap_gpio.h"

MODULE_LICENSE("GPL");

/******************************** 本模块的宏定义 ****************************/
// pinmux系列寄存器物理基地址和大小
#define PINMUX_ADDR_BASE 0x01c14120
#define PINMUX_SIZE (4*20)
// gpio系列寄存器物理基地址和大小
#define GPIO_ADDR_BASE 0x01e26000
#define GPIO_SIZE (0x88)
// ioctl所支持的命令
#define SET				0x80000001
#define CLR				0x80000002

#define LED (1<<8)
#define OUT (0)
#define IN (1)
#define HIGH (1)
#define LOW (0)
/********************************  引用模块外部变量 ************************/


/********************************  引用模块内部声明 ************************/
int scull_open(struct inode * inode, struct file * filp);
long scull_ioctl(struct file * filp, unsigned int cmd, unsigned long arg);
int scull_release(struct inode * inode, struct file * filp);

/********************************  模块内部变量 ****************************/
char * device_name = "led"; // 设备名称
void * pinmux_reg;             // pinmux寄存器内核地址
void * gpio_reg;               // gpio寄存器内核地址
unsigned int pinmux_data = 0;  // pinmux寄存器读写使用的变量
unsigned int gpio_data = 0;    // gpio寄存器读写使用的变量
static int major = 0;		   // 设备的主设备号
dev_t dev;                     // 内核中的设备编号
struct cdev scull_cdev;        // 内核中的字符设备
struct file_operations scull_fops = {  // 驱动程序所实现的方法
	.owner = THIS_MODULE,
	.unlocked_ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};
static struct class * scull_class;  // 内核中表示设备的一个类
/*
** 函数功能：设置LED引脚方向，输入/输出
** 输入参数：dir=IN 输入， dir=OUT 输出
** 输出参数：无
** 返回值：无
** 备注：
*/
void set_dir_led(int dir)
{
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, dir01));
	if(dir == OUT){
		gpio_data &= ~LED;
	}else{
		gpio_data |= LED;
	}
	iowrite32(gpio_data, ADDR(gpio_reg, gpio_t,dir01));
}
/*
** 函数功能：led引脚输出电平
** 输入参数：a=HIGH 输出高电平， a=LOW 输出低电平
** 输出参数：无
** 返回值：无
** 备注：
*/
void tx_led(unsigned char a)
{
	gpio_data = LED;
	if(a == HIGH){
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, set_data01));
	}else{
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, clr_data01));
	}
}
/*
** 函数功能：驱动加载函数
** 输入参数：无
** 输出参数：无
** 返回值： 0=成功 其他=失败
** 备注：
*/
static int hello_init(void)
{
	int result = 0;
	int err = 0;
	struct device * device;

	if(major){// 静态分配设备编号范围
		dev = MKDEV(major, 0);	
		result = register_chrdev_region(dev, 1, device_name);
	}else{
		// 动态分配设备编号范围
		result = alloc_chrdev_region(&dev, 0, 1, device_name);
		major = MAJOR(dev);
	}
	if(result < 0){
		printk(KERN_WARNING "scull: can't get major %d\n", major);
		return result;
	}
	printk("%s major is %d\n", device_name, major);
	// 分配初始化struct cdev和struct file_operations
	cdev_init(&scull_cdev, &scull_fops);
	scull_cdev.owner = THIS_MODULE;
	scull_cdev.ops = &scull_fops;
	// 将设备添加到系统中
	err = cdev_add(&scull_cdev, dev, 1);
	if(err){
		printk(KERN_NOTICE "Error %d adding %s", err, device_name);
		return -1;
	}
	// 为设备创建一个类
	scull_class = class_create(THIS_MODULE, device_name);
	if(IS_ERR(scull_class)){
		printk(KERN_NOTICE "Error faild in creating class.\n");
		return -1;
	}
	// 创建对应设备节点
	device = device_create(scull_class, NULL, dev, NULL, device_name);
	if(IS_ERR(device)){
		printk(KERN_NOTICE "error faild in device_create.\n");
		return -2;
	}
	// 内存映射，访问设备的物理地址
	// PINMUX
	pinmux_reg = (unsigned int *)ioremap(PINMUX_ADDR_BASE, PINMUX_SIZE); // pinmux映射
	printk("pinmux addr is 0x%08x.\n", (unsigned int)pinmux_reg);
	pinmux_data = ioread32(ADDR(pinmux_reg, pinmux_t, pinmux0)); // led
	pinmux_data &= ~(0xf<<28);
	pinmux_data |= (0x8<<28);
	iowrite32(pinmux_data, ADDR(pinmux_reg, pinmux_t, pinmux0));

	// GPIO
	gpio_reg = ioremap(GPIO_ADDR_BASE, GPIO_SIZE); // gpio映射
	printk("gpio addr is 0x%08x.\n", (unsigned int)gpio_reg);
	return 0;
}
/*
** 函数功能：驱动OPEN函数
** 输入参数：无
** 输出参数：无
** 返回值： 0=成功 其他=失败
** 备注：
*/
int scull_open(struct inode * inode, struct file * filp)
{
	set_dir_led(OUT);
	return 0;
}
/*
** 函数功能：驱动IOCTL函数
** 输入参数：cmd=命令 arg=参数
** 输出参数：无
** 返回值： 0=成功 其他=失败
** 备注：
*/
long scull_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case SET: // LED输出高电平
			tx_led(1);
			break;
		case CLR: // LED输出低电平
			tx_led(0);
			break;
		default:
			break;
	}
	return 0;
}

/*
** 函数功能：驱动CLOSE函数
** 输入参数：无
** 输出参数：无
** 返回值： 0=成功 其他=失败
** 备注：
*/
int scull_release(struct inode * inode, struct file * filp)
{
	return 0;
}
/*
** 函数功能：驱动卸载函数
** 输入参数：无
** 输出参数：无
** 返回值： 0=成功 其他=失败
** 备注：
*/
static void hello_exit(void)
{
	// 从系统中移除这个字符设备
	cdev_del(&scull_cdev);
	// 删除设备节点
	device_destroy(scull_class, dev);
	// 释放设备节点类
	class_destroy(scull_class);
	// 释放设备编号
	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
