#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
/* Defines the license for this LKM */
MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;
static int cnt = 0;

static int proc_simple_vfs_read(struct seq_file *m, void *v)
{
  printk(KERN_INFO "simple-vfs read:%d\n", cnt);
  seq_printf(m, "simple-vfs read:%d\n", cnt);
  return 0;
}

static int proc_simple_vfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_simple_vfs_read, NULL);
}

static ssize_t proc_simple_vfs_write(struct file *file, const char __user *buf,
		   size_t count, loff_t *ppos)
{
  printk(KERN_INFO "simple-vfs write:%d\n", cnt);
  cnt++;
  return count;
}

static const struct file_operations proc_file_operations = {
  .open = proc_simple_vfs_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
  .write  = proc_simple_vfs_write,
};

/* Init function called on module entry */
int my_module_init( void )
{
  proc_entry = proc_create( "simple-vfs", 0666, NULL, &proc_file_operations);
  printk(KERN_INFO "my_module_init called.  Module is now loaded.\n");
  return 0;
}
/* Cleanup function called on module exit */
void my_module_cleanup( void )
{
  remove_proc_entry("simple-vfs", NULL);
  printk(KERN_INFO "my_module_cleanup called.  Module is now unloaded.\n");
  return;
}
/* Declare entry and exit functions */
module_init( my_module_init );
module_exit( my_module_cleanup );
