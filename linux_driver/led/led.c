/********************************************************************
 *    MAJOR(dev_t dev)    // 获取主设备号
 *    MINOR(dev_t dev)    // 获取次设备号
 *    MKDEV(int mojor, int moinor)    // 合成dev_t设备号
*********************************************************************/

/*********************************************************************
struct file {
const struct file_operations *f_op;
//needed for tty driver, and maybe others 
void *private_data;
};

f_op：存放与文件操作相关的一系列函数指针，如open、read、wirte等函数。
private_data：该指针变量只会用于设备驱动程序中，内核并不会对该成员进行操作。因此，在驱动 程序中，通常用于指向描述设备的结构体。
*********************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define CCM_CCGR1 (volatile unsigned long*)0x20C406C          //时钟控制寄存器
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04 (volatile unsigned long*)0x20E006C//GPIO1_04复用功能选择寄存器
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04 (volatile unsigned long*)0x20E02F8 //PAD属性设置寄存器
#define GPIO1_GDIR (volatile unsigned long*)0x0209C004 //GPIO方向设置寄存器（输入或输出）
#define GPIO1_DR (volatile unsigned long*)0x0209C000   //GPIO输出状态寄存器

void led_init(void)
{
    *(CCM_CCGR1) = 0xFFFFFFFF;   //开启GPIO1的时钟
    *(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04) = 0x5;//设置PAD复用功能为GPIO
    *(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04) = 0x1F838;//设置PAD属性
    *(GPIO1_GDIR) = 0x10;//设置GPIO为输出模式
    *(GPIO1_DR) = 0x0;   //设置输出电平为低电平
}

#define led_on()     *(GPIO1_DR) &= ~(1<<4)
#define led_off()    *(GPIO1_DR) |= 1<<4

#define LED_MAJOR    230       // 默认的主设备号
#define LED_ON       1
#define LED_OFF      0

static int led_major = LED_MAJOR;    // 定义一个设备号
module_param(led_major, int, S_IRUGO);   // 模块参数，装载模块是可以向模块传递参数

struct led_dev
{
    struct cdev cdev;    // cdev 结构体（里面记录了设备号，文件操作结构体，所属模块等）
    unsigned char status;     // 保存 LED 灯的状态
};

struct led_dev *led_devp;

// 打开文件
static int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = led_devp;
    printk("open dev file\n");
    return 0;
}

// 关闭文件
static int led_release(struct inode *inode, struct file *filp)
{
    printk("cloes dev file\n");
    return 0;
}

// IO 控制函数
static long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct led_dev *dev = filp->private_data;

    switch (cmd){
    case LED_ON:
         led_on();
         dev->status = LED_ON;
         printk(KERN_INFO "led to on\n");
         break;

    case LED_OFF:
         led_off();
         dev->status = LED_OFF;
         printk(KERN_INFO "led to off\n");
         break;

    default:
         return -EINVAL;
    }
    
    return 0;
}

static ssize_t led_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    int ret = 0;
    struct led_dev *dev = filp->private_data;

    printk(KERN_INFO "read ing\n");

    if (copy_to_user(buf, &(dev->status), 1))
        ret = -EFAULT;
    else {
        ret = 1;
    }

    return ret;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    int ret = 0;
    struct led_dev *dev = filp->private_data;

    printk(KERN_INFO "write ing\n");

    if (copy_from_user(&(dev->status), buf, 1))
        ret = -EFAULT;
    else {
        if (dev->status != LED_OFF)
            led_on();
        else
            led_off();
        
        ret = 1;
    }

    return ret;
}

/* 文件操作结构体 */
static const struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .read = led_read,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
    .open = led_open,
    .release = led_release,
};

static void led_setup_cdev(struct led_dev *dev, int index)
{
    int err, devno = MKDEV(led_major, index);    // 合成dev_t
    
    cdev_init(&dev->cdev,  &led_fops);    // 关联文件操作结构体和cdev结构体

    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev->cdev, devno, 1);    // 注册设备
    if (err)
        printk(KERN_NOTICE "error %d adding led%d", err, index);
}

static int __init led_dev_init(void)
{
    int ret;

    dev_t devno = MKDEV(led_major, 0);    // 合成设备号，次设备号为0

    if (led_major)
        ret = register_chrdev_region(devno, 1, "led");    // 申请一个已知的设备号
    else {
        ret = alloc_chrdev_region(&devno, 0, 1, "led");     // 动态申请一个设备号
        led_major = MAJOR(devno);
    }

    if (ret < 0)
        return ret;

    led_devp = kzalloc(sizeof(struct led_dev), GFP_KERNEL);
    if (!led_devp){
        ret = -ENOMEM;
        goto fail_malloc;
    }
    
    led_setup_cdev(led_devp, 0);     // 设备号和gloablmem_dev结构体都申请成功

    led_init();

    printk(KERN_DEBUG "debug -> insmod led\n");

    return 0;
    
    fail_malloc:
    unregister_chrdev_region(devno, 1);    // 释放设备号
    return ret;
}

module_init(led_dev_init);    // 告诉内核使用这个函数初始化内核模块

static void __exit led_exit(void)
{
    cdev_del(&led_devp->cdev);    // 卸载dev设备
    kfree(led_devp);
    unregister_chrdev_region(MKDEV(led_major, 0), 1);     // 释放设备号
}

module_exit(led_exit);

MODULE_AUTHOR("longzr");     // 作者
MODULE_LICENSE("GPL v2");    // 协议