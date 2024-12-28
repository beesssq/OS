#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux kernel module for Tomsk State University");

static int __init tsu_module_init(void) {
    pr_info("Welcome to the Tomsk State University\n");
    return 0;
}

static void __exit tsu_module_exit(void) {
    pr_info("Tomsk State University forever!\n");
}

module_init(tsu_module_init);
module_exit(tsu_module_exit);
