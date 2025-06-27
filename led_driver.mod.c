#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x7c2dc553, "__platform_driver_register" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x92997ed8, "_printk" },
	{ 0x33b4203d, "platform_driver_unregister" },
	{ 0x73d4956c, "of_get_named_gpio_flags" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xb4fd8b29, "gpio_to_desc" },
	{ 0x8e0d6f5d, "gpiod_direction_output_raw" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x2085a943, "cdev_init" },
	{ 0x13cbe78c, "cdev_add" },
	{ 0x5505cbe1, "__class_create" },
	{ 0xf7f249d3, "device_create" },
	{ 0x877f1970, "_dev_info" },
	{ 0xd735893f, "_dev_err" },
	{ 0x1a632ddc, "gpiod_set_raw_value" },
	{ 0xfe990052, "gpio_free" },
	{ 0x3d9e0db, "device_destroy" },
	{ 0xb736e912, "class_destroy" },
	{ 0x97fc4079, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x3ea1b6e4, "__stack_chk_fail" },
	{ 0x2d66370f, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*CmyFPT,my-led");
MODULE_ALIAS("of:N*T*CmyFPT,my-ledC*");
