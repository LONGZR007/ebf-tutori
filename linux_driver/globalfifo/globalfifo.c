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
#include <linux/signalfd.h>

#define GLOBALFIFO_SIZE     0x1000    // 缓冲区的大小
#define MEM_CLEAR           0x1
#define GLOBALFIFO_MAJOR    230       // 默认的主设备号
#define DEVICE_NUM          10

static int globalfifo_major = GLOBALFIFO_MAJOR;    // 定义一个设备号
module_param(globalfifo_major, int, S_IRUGO);   // 模块参数，装载模块是可以向模块传递参数

struct globalfifo_dev
{
    struct cdev cdev;       // cdev 结构体（里面记录了设备号，文件操作结构体，所属模块等）
    unsigned char mem[GLOBALFIFO_SIZE];
    struct mutex mutex;    // 互斥体
    wait_queue_head_t r_wait;
    wait_queue_head_t w_wait;
    unsigned int current_len;
};

struct globalfifo_dev *globalfifo_devp;

// 打开文件
static int globalfifo_open(struct inode *inode, struct file *filp)
{
    struct globalfifo_dev *dev = container_of(inode->i_cdev, struct globalfifo_dev, cdev);
    filp->private_data = dev;
    printk("open dev file\n");
    return 0;
}

// 关闭文件
static int globalfifo_release(struct inode *inode, struct file *filp)
{
    printk("cloes dev file\n");
    return 0;
}

// IO 控制函数
static long globalfifo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct globalfifo_dev *dev = filp->private_data;

    switch (cmd){
    case MEM_CLEAR:
         mutex_lock(&dev->mutex);
         memset(dev->mem, 0, GLOBALFIFO_SIZE);
         mutex_unlock(&dev->mutex);
         printk(KERN_INFO "globalfifo is set to zero\n");
         break;

    default:
         return -EINVAL;
    }
    
    return 0;
}

static ssize_t globalfifo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    int ret = 0;
    struct globalfifo_dev *dev = filp->private_data;
    DECLARE_WAITQUEUE(wait, current);

    mutex_lock(&dev->mutex);
    add_wait_queue(&dev->r_wait, &wait);    // 添加到读等待队列

    while (dev->current_len == 0){
        if (filp->f_flags & O_NONBLOCK){
            ret = -EAGAIN;
            goto out;
        }
        
        __set_current_state(TASK_INTERRUPTIBLE);    // 设置为浅度睡眠
        mutex_unlock(&dev->mutex);

        schedule();    // 进程进入睡眠
        if (signal_pending(current)){    // 判断是不是被信号唤醒
            ret = -ERESTARTSYS;
            goto out2;
        }

        mutex_lock(&dev->mutex);
    }

    if (count > dev->current_len)
        count = dev->current_len;

    if (copy_to_user(buf, dev->mem, count))
        ret = -EFAULT;
    else {
        memcpy(dev->mem, dev->mem + count, dev->current_len - count);
        dev->current_len -= count;
        ret = count;

        printk(KERN_ERR "read %u butes(s) form %u\n", count, dev->current_len);

        wake_up_interruptible(&dev->w_wait);     // 唤醒可能正在等待的写队列
    }

    out:
    mutex_unlock(&dev->mutex);

    out2:
    remove_wait_queue(&dev->r_wait, &wait);    // 移除等待队列 wait
    set_current_state(TASK_RUNNING);

    return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    int ret = 0;
    struct globalfifo_dev *dev = filp->private_data;
    DECLARE_WAITQUEUE(wait, current);

    add_wait_queue(&dev->w_wait, &wait);    // 添加到读等待队列

    while (dev->current_len == GLOBALFIFO_SIZE){
        if (filp->f_flags & O_NONBLOCK){
            ret = -EAGAIN;
            goto out;
        }
        
        __set_current_state(TASK_INTERRUPTIBLE);    // 设置为浅度睡眠
        mutex_unlock(&dev->mutex);

        schedule();    // 进程进入睡眠
        if (signal_pending(current)){    // 判断是不是被信号唤醒
            ret = -ERESTARTSYS;
            goto out2;
        }

        mutex_lock(&dev->mutex);
    }

    if (count > GLOBALFIFO_SIZE - dev->current_len)
        count = GLOBALFIFO_SIZE - dev->current_len;

    mutex_lock(&dev->mutex);

    if (copy_from_user(dev->mem + dev->current_len, buf, count))
        ret = -EFAULT;
    else {
        dev->current_len += count;
        ret = count;
    
        wake_up_interruptible(&dev->r_wait);

        printk(KERN_INFO "writeren %u bytes(s) from %u\n", count, dev->current_len);
    }

    out:
    mutex_unlock(&dev->mutex);

    out2:
    remove_wait_queue(&dev->w_wait, &wait);    // 移除等待队列 wait
    set_current_state(TASK_RUNNING);

    return ret;
}

static loff_t globalfifo_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret = 0;

    switch (orig) {
        case 0:
            if (offset < 0){
                ret = -EINVAL;
                break;
            }

            if ((unsigned int)offset > GLOBALFIFO_SIZE)
            {
                ret = -EINVAL;
                break;
            }

            filp->f_pos = (unsigned int)offset;
            ret = filp->f_pos;
            break;

        case 1:
            if ((filp->f_pos + offset) > GLOBALFIFO_SIZE || (filp->f_pos + offset) < 0) {
                ret = -EINVAL;
                break;
            }

            filp->f_pos += offset;
            ret = filp->f_pos;
            break;

        default:
            break;
    }

    return ret;
}

/* 文件操作结构体 */
static const struct file_operations globalfifo_fops = {
    .owner = THIS_MODULE,
    .llseek = globalfifo_llseek,
    .read = globalfifo_read,
    .write = globalfifo_write,
    .unlocked_ioctl = globalfifo_ioctl,
    .open = globalfifo_open,
    .release = globalfifo_release,
};

static void globalfifo_setup_cdev(struct globalfifo_dev *dev, int index)
{
    int err, devno = MKDEV(globalfifo_major, index);    // 合成dev_t
    
    cdev_init(&dev->cdev,  &globalfifo_fops);    // 关联文件操作结构体和cdev结构体

    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev->cdev, devno, 1);    // 注册设备
    if (err)
        printk(KERN_NOTICE "error %d adding globalfifo%d", err, index);
}

static int __init globalfifo_init(void)
{
    int ret;
    int i=0;

    dev_t devno = MKDEV(globalfifo_major, 0);    // 合成设备号，次设备号为0

    if (globalfifo_major)
        ret = register_chrdev_region(devno, DEVICE_NUM, "globalfifo");    // 申请一个已知的设备号
    else {
        ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalfifo");     // 动态申请一个设备号
        globalfifo_major = MAJOR(devno);
    }

    if (ret < 0)
        return ret;

    globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev) * DEVICE_NUM, GFP_KERNEL);
    if (!globalfifo_devp){
        ret = -ENOMEM;
        goto fail_malloc;
    }
    
    for (i=0; i<DEVICE_NUM; i++)
    {
        globalfifo_setup_cdev(globalfifo_devp + i, i);     // 设备号和gloablmem_dev结构体都申请成功
        mutex_init(&(globalfifo_devp + 1)->mutex);
        init_waitqueue_head(&(globalfifo_devp + i)->r_wait);
        init_waitqueue_head(&(globalfifo_devp + i)->w_wait);
    }

    printk(KERN_DEBUG "debug -> insmod globalfifo\n");

    return 0;
    
    fail_malloc:
    unregister_chrdev_region(devno, DEVICE_NUM);    // 释放设备号
    return ret;
}

module_init(globalfifo_init);    // 告诉内核使用这个函数初始化内核模块

static void __exit globalfifo_exit(void)
{
    int i = 0;

    for (i=0; i<DEVICE_NUM; i++)
        cdev_del(&(globalfifo_devp + i)->cdev);    // 卸载dev设备
        
    kfree(globalfifo_devp);
    unregister_chrdev_region(MKDEV(globalfifo_major, 0), DEVICE_NUM);     // 释放设备号
}

module_exit(globalfifo_exit);

MODULE_AUTHOR("longzr");     // 作者
MODULE_LICENSE("GPL v2");    // 协议