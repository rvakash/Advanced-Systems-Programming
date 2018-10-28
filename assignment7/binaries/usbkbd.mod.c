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
	{ 0x1deb9105, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x62aea950, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0x475bfdbb, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0xd19b25d6, __VMLINUX_SYMBOL_STR(device_set_wakeup_enable) },
	{ 0xd1a8ec31, __VMLINUX_SYMBOL_STR(input_free_device) },
	{ 0xd1e5002a, __VMLINUX_SYMBOL_STR(input_register_device) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xf9c0b663, __VMLINUX_SYMBOL_STR(strlcat) },
	{ 0x5792f848, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0x6d0dfe2, __VMLINUX_SYMBOL_STR(usb_alloc_coherent) },
	{ 0xf2a6e0a2, __VMLINUX_SYMBOL_STR(usb_alloc_urb) },
	{ 0x31565b1, __VMLINUX_SYMBOL_STR(input_allocate_device) },
	{ 0xf1943569, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xef551010, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x2cf1909a, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xf812cff6, __VMLINUX_SYMBOL_STR(memscan) },
	{ 0x33513762, __VMLINUX_SYMBOL_STR(input_event) },
	{ 0xcb7a6f4, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0xbecbac09, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x8f64aa4, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x9327f5ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0xdf968ae4, __VMLINUX_SYMBOL_STR(usb_submit_urb) },
	{ 0x9d3b2dcd, __VMLINUX_SYMBOL_STR(input_unregister_device) },
	{ 0x4f7aae1a, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x4d47be4a, __VMLINUX_SYMBOL_STR(usb_free_coherent) },
	{ 0xb475cb62, __VMLINUX_SYMBOL_STR(usb_free_urb) },
	{ 0x53f93d95, __VMLINUX_SYMBOL_STR(usb_kill_urb) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x9b5bde1e, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic03isc01ip01in*");

MODULE_INFO(srcversion, "53193B053D46DEFDD2D28BF");
