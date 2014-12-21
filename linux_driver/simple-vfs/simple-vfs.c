#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
/* Defines the license for this LKM */
MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;

/* Init function called on module entry */
int my_module_init( void )
{
  proc_entry = create_proc_entry( "simple-vfs", 0644, NULL );
  printk(KERN_INFO "my_module_init called.  Module is now loaded.\n");
  return 0;
}
/* Cleanup function called on module exit */
void my_module_cleanup( void )
{
  remove_proc_entry("simple-vfs", &proc_root);
  printk(KERN_INFO "my_module_cleanup called.  Module is now unloaded.\n");
  return;
}
/* Declare entry and exit functions */
module_init( my_module_init );
module_exit( my_module_cleanup );
