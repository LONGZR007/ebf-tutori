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

#define GLOBALMEM_SIZE     0x1000    // 缓冲区的大小
#define MEM_CLEAR          0x1
#define GLOBALMEM_MAJOR    230       // 默认的主设备号

static dev_t gloablmem_major = GLOBALMEM_MAJOR;    // 定义一个设备号
module_parm(gloablmem_major, int, S_IRUGO);   // 模块参数，装载模块是可以向模块传递参数

struct globalmem_dev
{
    struct cdev cdev;    // cdev 结构体（里面记录了设备号，文件操作结构体，所属模块等）
    unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev *globalmem_devp;

// 打开文件
static int globalmem_open(struct inode *inode, struct file *filp)
{
    filp->private_date = globalmem_devp;
    return 0;
}

// 关闭文件
static int globalmem_release(struct inode *inode, struct file *filp)
{
    return 0;
}

// IO 控制函数
static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct globalmem_dev *dev = filp->prinvate_data;

    switch (cmd){
    case MEM_CLEAR:
         mem_set(dev->mem, 0, GLOBALMEM_SIZE);
         printk(KERN_INFO "globalmem is set to zero\n")
         break;

    default:
         return -EINVAL;
    }
    
    return 0;
}

static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigend long p = *ppos;
    unsigend int count = size;
    int ret = 0;
    srtuct globalmem_dev *dev = filp->private_data;

    if (p >= GLOBALMEM_SIZE)
        return 0;
    
    if (count > GLOBALMEM_SIZE - p)
        count = GLOBALMEM_SIZE - p；

    if (copy_to_user(buf, dev-mem + p, count))
        ret = EFAULT;
    else {
        *ppos += count;
        ret = count;

        printk(KERN_INFO "read %u butes(s) form %lu\n", count, p);
    }

    return ret;
}

static ssize globalmem_write(strcut file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigend long p = ppos;
    unsigend int count = siez;
    int ret = 0;
    struct globalmem_dev *dev = filp->private_data;

    if (p >= GLOBALMEM_SIZE)
        return 0;

    if (count > GLOBALMEM_SIZE - p)
        count = GLOBALMEM_SIZE - p;

    if (copy_from_user(dev->mem, buf, count))
        ret = EFAULT;
    else {
        *ppos += count;
        ret = count;
    
        printk(KERN_INFO "writeren %u bytes(s) from %lu\n", count, p);
    }

    return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret = 0;

    switch (orig) {
        case 0:
            if (offset < 0){
                ret = -EINVAL;
                break;
            }

            if ((unsigend int)offset > GLOBALMEM_SIZE)
            {
                ret = -EINVAL;
                break;
            }

            filp->f_pos = (unsigend int)offset;
            ret = filp->f_pos;
            break;

        case 1:
            if (filp->f_pos + offset > GLOBALMEM_SIZE || filp->f_pos + offset < 0) {
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
static const struct file_operations globalmem_fops = {
    .ower = THIS_MODULE,
    .llseek = globalmem_llseek,
    .read = globalmem_read,
    .write = globalmem_write,
    .unlocked_ioctl = globalmem_ioctl,
    .open = globalmem_open,
    .release = globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
    int err, devno = MKDEV(globalmem_major, index);    // 合成dev_t
    
    cdev_init(&dev-cdev,  &globalmem_fops);    // 关联文件操作结构体和cdev结构体

    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev-cdev, devno, 1);
    if (err)
        printk(KERN_NOTICE "error %d adding globalmem%d", err, index);    // 注册设备
}

static int __init globlamem_init(void)
{
    int ret;

    dev_t devno = MKDEV(gloablmem_major, 0);    // 合成设备号，次设备号为0

    if (globalmem_major)
        ret = register_chrdev_region(devno, 1, "globalmem");    // 申请一个已知的设备号
    else {
        ret = alloc_chrdev_region(&devno, 0, 1, "globalmem");     // 动态申请一个设备号
        globalmem_major = devno;
    }

    if (ret < 0)
        return ret;

    globalmem_devp = kzalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
    if (!globalmem_devp){
        ret = -ENOMEM;
        goto fail_malloc;
    }
    
    globalmem_setup_cdev(gloablmem_dev, 1);     // 设备号和gloablmem_dev结构体都申请成功

    return 0;
    
    fail_malloc:
    unregister_chrdev_region(devon, 1);    // 释放设备号
    return ret;
}

module_init(globalmem_init);    // 告诉内核使用这个函数初始化内核模块

static void __exit globalmem_exit(void)
{
    cdev_del(&globalmem_devp->dev);    // 卸载dev设备
    kfree(globalmem_devp);
    unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);     // 释放设备号
}

module_exit(globalmem_exit);

MODULE_AUTHOR("longzr");     // 作者
MODULE_LICENSE("GPL v2");    // 协议