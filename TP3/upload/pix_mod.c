#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int __init pix_init(void){
  printk(KERN_INFO "PIX: staring...");
  // stuff to do
  printk(KERN_INFO "PIX: staring done.");
  return 0;
}

static void __exit pix_exit(void){
  printk(KERN_INFO "PIX: stopping...");
  // stuff to do
  printk(KERN_INFO "PIX: stopping done.");
}

module_init(pix_init);
module_exit(pix_exit);
