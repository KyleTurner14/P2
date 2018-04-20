#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xb6fd8534, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xb2d09102, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x52483983, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x47256491, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x3be86a54, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xc3aaf0a9, __VMLINUX_SYMBOL_STR(__put_user_1) },
	{ 0xbfe8ed5b, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xa80ee188, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x8b8dd7ac, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xf6185bb8, __VMLINUX_SYMBOL_STR(__class_create) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "5A796360F83E31602C1C38A");
