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
#include "sw_iic.h"

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
#define READ_ST			0x80000003
#define DEBUG			0x80000004
#define SET_ADDR        0x80000005
// SDA和SCL引脚定义
#define SDA (1<<20)
#define SCL (1<<21)

#define SDA_EN (1<<8)
#define SCL_EN (1<<29)
/********************************  引用模块外部变量 ************************/


/********************************  引用模块内部声明 ************************/
int scull_open(struct inode * inode, struct file * filp);
long scull_ioctl(struct file * filp, unsigned int cmd, unsigned long arg);
int scull_release(struct inode * inode, struct file * filp);
ssize_t scull_read(struct file * filp, char __user * buff, size_t count, loff_t * offp);
ssize_t scull_write(struct file * filp, const char __user * buff, size_t count, loff_t * offp);

/********************************  模块内部变量 ****************************/
char * device_name = "iic_laser"; // 设备名称
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
	.read = scull_read,
	.write = scull_write,
	.release = scull_release,
};
static struct class * scull_class;  // 内核中表示设备的一个类
unsigned char slave_addr;
/*
** 函数功能：设置SDA_EN引脚方向，输入/输出
** 输入参数：dir=IN 输入， dir=OUT 输出
** 输出参数：无
** 返回值：无
** 备注：
*/
void set_dir_sda_en(int dir)
{
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, dir23));
	if(dir == OUT){
		gpio_data &= ~SDA_EN;
	}else{
		gpio_data |= SDA_EN;
	}
	iowrite32(gpio_data, ADDR(gpio_reg, gpio_t,dir23));
}
/*
** 函数功能：设置SCL_EN引脚方向，输入/输出
** 输入参数：dir=IN 输入， dir=OUT 输出
** 输出参数：无
** 返回值：无
** 备注：
*/
void set_dir_scl_en(int dir)
{
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, dir23));
	if(dir == OUT){
		gpio_data &= ~SCL_EN;
	}else{
		gpio_data |= SCL_EN;
	}
	iowrite32(gpio_data, ADDR(gpio_reg, gpio_t,dir23));
}
/*
** 函数功能：SDA_EN引脚输出电平
** 输入参数：a=HIGH 输出高电平， a=LOW 输出低电平
** 输出参数：无
** 返回值：无
** 备注：
*/
void tx_sda_en(unsigned char a)
{
	gpio_data = SDA_EN;
	if(a == HIGH){
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, set_data23));
	}else{
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, clr_data23));
	}
}
/*
** 函数功能：SCL_EN引脚输出电平
** 输入参数：a=HIGH 输出高电平， a=LOW 输出低电平
** 输出参数：无
** 返回值：无
** 备注：
*/
void tx_scl_en(unsigned char a)
{
	gpio_data = SCL_EN;
	if(a == HIGH){
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, set_data23));
	}else{
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, clr_data23));
	}
}
/*
** 函数功能：设置SDA引脚方向，输入/输出
** 输入参数：dir=IN 输入， dir=OUT 输出
** 输出参数：无
** 返回值：无
** 备注：
*/
void set_dir_sda(int dir)
{
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, dir01));
	if(dir == OUT){
		gpio_data &= ~SDA;
	}else{
		gpio_data |= SDA;
	}
	iowrite32(gpio_data, ADDR(gpio_reg, gpio_t,dir01));
}
/*
** 函数功能：设置SCL引脚方向，输入/输出
** 输入参数：dir=IN 输入， dir=OUT 输出
** 输出参数：无
** 返回值：无
** 备注：
*/
void set_dir_scl(int dir)
{
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, dir01));
	if(dir == OUT){
		gpio_data &= ~SCL;
	}else{
		gpio_data |= SCL;
	}
	iowrite32(gpio_data, ADDR(gpio_reg, gpio_t,dir01));
}
/*
** 函数功能：5us延时
** 输入参数：无
** 输出参数：无
** 返回值：无
** 备注：
*/
void delay5us(void)
{
	udelay(5);
}
/*
** 函数功能：6us延时
** 输入参数：无
** 输出参数：无
** 返回值：无
** 备注：
*/
void delay6us(void)
{
	udelay(6);
}
/*
** 函数功能：SDA引脚输出电平
** 输入参数：a=HIGH 输出高电平， a=LOW 输出低电平
** 输出参数：无
** 返回值：无
** 备注：
*/
void tx_sda(unsigned char a)
{
	tx_sda_en(HIGH);// 使能CPLD中SDA的方向，ARM->LASER
	set_dir_sda(OUT);
	gpio_data = SDA;
	if(a == HIGH){
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, set_data01));
	}else{
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, clr_data01));
	}
}
/*
** 函数功能：读取SDA引脚状态
** 输入参数：无
** 输出参数：无
** 返回值：HIGH 输出高电平， LOW 输出低电平
** 备注：
*/
unsigned char rx_sda(void)
{

	tx_sda_en(LOW);// 使能CPLD中SDA的方向，ARM<-LASER
	set_dir_sda(IN);
	udelay(5);
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, in_data01));
	if(gpio_data & SDA){
		return HIGH;
	}else{
		return LOW;
	}
}
/*
** 函数功能：SCL引脚输出电平
** 输入参数：a=HIGH 输出高电平， a=LOW 输出低电平
** 输出参数：无
** 返回值：无
** 备注：
*/
void tx_scl(unsigned char a)
{
	tx_scl_en(HIGH);// 使能CPLD中SCL的方向，ARM->LASER
	set_dir_scl(OUT);
	gpio_data = SCL;
	if(a == HIGH){
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, set_data01));
	}else{
		iowrite32(gpio_data, ADDR(gpio_reg, gpio_t, clr_data01));
	}
}
/*
** 函数功能：读取SCL引脚状态
** 输入参数：无
** 输出参数：无
** 返回值：HIGH 输入高电平， LOW 输入低电平
** 备注：
*/
unsigned char rx_scl(void)
{
	tx_scl_en(LOW);// 使能CPLD中SCL的方向，ARM<-LASER
	set_dir_scl(IN);
	udelay(5);
	gpio_data = ioread32(ADDR(gpio_reg, gpio_t, in_data01));
	if(gpio_data & SCL){
		return HIGH;
	}else{
		return LOW;
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
	pinmux_data = ioread32(ADDR(pinmux_reg, pinmux_t, pinmux4)); // SDA SCL
	pinmux_data &= ~(0xff<<8);
	pinmux_data |= (0x88<<8);
	iowrite32(pinmux_data, ADDR(pinmux_reg, pinmux_t, pinmux4));

	pinmux_data = ioread32(ADDR(pinmux_reg, pinmux_t, pinmux7)); // SCL_EN
	pinmux_data &= ~(0xf<<8);
	pinmux_data |= (0x8<<8);
	iowrite32(pinmux_data, ADDR(pinmux_reg, pinmux_t, pinmux7));

	pinmux_data = ioread32(ADDR(pinmux_reg, pinmux_t, pinmux5)); // SDA_EN
	pinmux_data &= ~(0xf<<28);
	pinmux_data |= (0x8<<28);
	iowrite32(pinmux_data, ADDR(pinmux_reg, pinmux_t, pinmux5));

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
	set_dir_scl_en(OUT);
	set_dir_sda_en(OUT);
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
	unsigned char st;
	switch(cmd){
		case SET_ADDR:
			slave_addr = (unsigned char)arg;
			break;
		case SET: // 测试SDA，SCL输出高电平
			if(arg == SDA){
				printk("set sda 1.\n");
				tx_sda(1);
			}else{
				printk("set scl 1.\n");
				tx_scl(1);
			}
			break;
		case CLR: // 测试SDA，SCL输出低电平
			if(arg == SDA){
				printk("clr sda 0.\n");
				tx_sda(0);
			}else{
				printk("clr scl 0.\n");
				tx_scl(0);
			}
			break;
		case READ_ST: // 读引脚状态
			if(arg == SDA){
				st = rx_sda();
				printk("sda = %d.\n", st);
			}else{
				st = rx_scl();
				printk("scl = %d.\n", st);
			}
			break;
		default:
			break;
	}
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
	unsigned char tbuf[256];
	int i = 0;
	unsigned char tmp = 0;
	
	for(i = 0; i < 256; i++){
		tbuf[i] = 0;
	}
	iic_start();
	tmp = slave_addr|0x1;
	iic_write_bytes(&tmp, 1);
	iic_read_bytes(tbuf, count);
	iic_stop();
	if(copy_to_user(buff, tbuf, count)){
		return -1;
	}
	return 0;
}
/*
** 函数功能：驱动WRITE函数
** 输入参数：buff=用户空间数据缓冲区, count=需写入数据长度
** 输出参数：无
** 返回值： 0=成功 其他=失败
** 备注：
*/
ssize_t scull_write(struct file * filp, const char __user * buff, size_t count, loff_t * offp)
{
	unsigned char tbuf[256];
	unsigned char tmp = 0;
	
	if(count > 255){
		printk("write error. buff is too long!!!\n");
		return -2;
	}
	if(copy_from_user(tbuf, buff, count)){
		return -1;
	}
	iic_start();
	tmp = slave_addr;
	iic_write_bytes(&tmp, 1);
	if(iic_write_bytes(tbuf, count)){
		return -1;
	}
	iic_stop();
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
