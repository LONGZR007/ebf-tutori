/********************************************************************
 *    MAJOR(dev_t dev)    // 获取主设备号
 *    MINOR(dev_t dev)    // 获取次设备号
 *    MKDEV(int mojor, int moinor)    // 合成dev_t
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

static int gloablmem_major = GLOBALMEM_MAJOR;    // 定义一个设备号
module_parm(gloablmem_major, int, S_IRUGO);   // 模块参数，装载模块是可以向模块传递参数

struct globalmem_dev
{
    struct cdev cdev;    // cdev 结构体（里面记录了设备号，文件操作结构体，所属模块等）
    unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev *globalmem_dev;

static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
    int err, devno = MKDEV(globalmem_major, index);    // 合成dev_t
    
    cdev_init(&dev-cdev,  &globalmem_fops);    // 关联文件操作结构体和cdev结构体

    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev-cdev, devno, 1);
    if (err)
    {
        printk(KERN_NOTICE "error %d adding globalmem%d", err, index);    // 注册设备
    }

}
