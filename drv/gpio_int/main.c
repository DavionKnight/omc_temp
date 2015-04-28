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
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm/signal.h>
#include "omap_gpio.h"

MODULE_LICENSE("GPL");

/******************************** 本模块的宏定义 ****************************/
// pinmux系列寄存器物理基地址和大小
#define PINMUX_ADDR_BASE 0x01c14120
#define PINMUX_SIZE (4*20)
// gpio系列寄存器物理基地址和大小
#define GPIO_ADDR_BASE 0x01e26000
#define GPIO_SIZE (0x88)
// aintc系列寄存器物理基地址和大小
#define AINTC_ADDR_BASE 0xFFFEE000
#define AINTC_SIZE 0x2000
// ioctl所支持的命令
#define START			0x00000001
#define STOP			0x00000002
#define SET				0x80000001
#define CLR				0x80000002
#define READ_ST			0x80000003
#define DEBUG			0x80000004

/********************************  引用模块外部变量 ************************/


/********************************  引用模块内部声明 ************************/
int scull_open(struct inode * inode, struct file * filp);
//long scull_ioctl(struct file * filp, unsigned int cmd, unsigned long arg);
int scull_release(struct inode * inode, struct file * filp);
ssize_t scull_read(struct file * filp, char __user * buff, size_t count, loff_t * offp);
ssize_t scull_write(struct file * filp, const char __user * buff, size_t count, loff_t * offp);

/********************************  模块内部变量 ****************************/
char * device_name = "gpio_int"; // 设备名称
void * pinmux_reg;             // pinmux寄存器内核地址
void * gpio_reg;               // gpio寄存器内核地址
void * aintc_reg;             // aintc寄存器内核地址
unsigned int pinmux_data = 0;  // pinmux寄存器读写使用的变量
unsigned int gpio_data = 0;    // gpio寄存器读写使用的变量
unsigned int aintc_data = 0;   // aintc寄存器读写使用的变量
static int major = 0;		   // 设备的主设备号
dev_t dev;                     // 内核中的设备编号
struct cdev scull_cdev;        // 内核中的字符设备
struct file_operations scull_fops = {  // 驱动程序所实现的方法
	.owner = THIS_MODULE,
	.open = scull_open,
	.read = scull_read,
	.release = scull_release,
};
static struct class * scull_class;  // 内核中表示设备的一个类

static DECLARE_WAIT_QUEUE_HEAD (wq);
static int wait_flag = 0;
static int interrupt_flag = 0;
unsigned int aintc_read(unsigned offset)
{
	return ioread32(aintc_reg+offset);
}
void aintc_write(unsigned long value, unsigned offset)
{
	iowrite32(value, aintc_reg+offset);
}
void printk_reg(char * str, unsigned offset)
{
	printk("%s:0x%08x.\n", str, aintc_read(offset));
}

static irqreturn_t int_cpld(int irq, void * dev_id)
{
	if(irq == gpio_to_irq(12)){
		printk("cpld interrupt.\n");
		interrupt_flag |= 2;
		wait_flag = 1;
		wake_up_interruptible(&wq);
	}
	return IRQ_HANDLED;
}
static irqreturn_t int_fpga(int irq, void * dev_id)
{
	if(irq == gpio_to_irq(10)){
		printk("fpga interrupt.\n");
		interrupt_flag |= 1;
		wait_flag = 1;
		wake_up_interruptible(&wq);
	}
	return IRQ_HANDLED;
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
	int i = 0;

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
	printk("get major is %d\n", major);
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
	// GPIO
	i = request_irq(gpio_to_irq(12), int_cpld, IRQF_DISABLED | IRQF_TRIGGER_FALLING, "int_cpld", NULL);
	if(i){
		printk("Kernel panic regster interrupt error! gpio_interrupt. rlt=%d\n", i);
	}
	i = request_irq(gpio_to_irq(10), int_fpga, IRQF_DISABLED | IRQF_TRIGGER_FALLING, "int_fpga", NULL);
	if(i){
		printk("Kernel panic regster interrupt error! gpio_interrupt. rlt=%d\n", i);
	}
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
	return 0;
}
/*
** 函数功能：驱动READ函数
** 输入参数：count=需读取数据长度
** 输出参数：buff=用户空间数据缓冲区
** 返回值： 0=成功 其他=失败
** 备注：
*/
ssize_t scull_read(struct file * filp, char __user * buff, size_t count, loff_t * offp)
{
	if(count != 4){
		return -1;
	}
	wait_event_interruptible(wq, wait_flag != 0);
	wait_flag = 0;
	if(copy_to_user(buff, (unsigned char *)&interrupt_flag, count)){
		return -2;
	}
	interrupt_flag = 0;
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
	free_irq(gpio_to_irq(12), NULL);
	free_irq(gpio_to_irq(10), NULL);
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
