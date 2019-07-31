/*
CS342 Project 4
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 24.05.2019
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/fs.h> 
#include <linux/types.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/genhd.h>
#include <asm/uaccess.h>
#include <linux/xarray.h>
#include <linux/proc_fs.h>

#define next_task(p) \
    list_entry_rcu((p)->tasks.next, struct task_struct, tasks)

#define for_each_process(p) \
    for (p = &init_task ; (p = next_task(p)) != &init_task ; )

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Slanci");

static int processid = 5;
struct task_struct *task;
struct task_struct *cp; //pointer to current process


module_param(processid,int,0);
MODULE_PARM_DESC(processid,"This is processid");

static int mod_init(void){

	struct files_struct *current_files;
	struct fs_struct *current_fs;
	
 	struct fdtable *files_table;
 	struct path files_path;
 	char *cwd;
 	struct path pwd;
 	struct path root;
 	void* copy_array[100];
 	char buf[100];
 	int i = 0;

	struct inode * current_inode;
	printk(KERN_ALERT "Hello World\n");
	printk(KERN_INFO "processid: %d\n", processid);
	
	for_each_process(task) {
		printk(KERN_INFO "[pid: %d] pname: %s [state: %ld]\n", task->pid, task->comm, task->state);
		if (task->pid == processid)
		{
			current_fs = task->fs;
			// printk(KERN_INFO "USER ID: %d", current_fs->users); //process UID
			printk(KERN_INFO "[pid: %d] pname: %s [state: %ld]\n", task->pid, task->comm, task->state);
			
			//printk("Process Access Mode: %d", current->euid);
			current_files = task->files;
			files_table = files_fdtable(current_files);
				
			get_fs_pwd(current_fs, &pwd); //path of file
 			cwd = d_path(&pwd,buf,100*sizeof(char)); //convert to string 
			printk("pwd: %s",cwd);
 			
 			get_fs_root(current_fs, &root); //path of file
 			cwd = d_path(&root,buf,100*sizeof(char)); //convert to string 
			printk("root: %s",cwd);

			while(files_table->fd[i] != NULL) { 
				//means this process has open files 
				//files_table->fd[i] == file
 				files_path = files_table->fd[i]->f_path; //path of file
 				current_inode = file_inode(files_table->fd[i]);//inode of file

 				uid_t uid = i_uid_read(current_inode);// USER ID 
 				umode_t	i_mode = current_inode->i_mode;
 				fmode_t f_mode = files_table->fd[i]->f_mode;
 				unsigned long i_ino = current_inode->i_ino;
 				cwd = d_path(&files_path,buf,100*sizeof(char)); //convert to string 


 				printk(KERN_ALERT "Open file with fd %d  %s", i,cwd);
 				printk ("Current file position pointer: %p",files_table->fd[i]);
 				
 				//printk ("Current file position pointer: %lld",files_table->fd[i]->f_pos);
 				
 				printk("Descriptor number: %d", i);
 				printk("User id: %d", uid);
				printk("i_mode: %d", i_mode);
 				printk("f_mode: %d", f_mode);
 				printk("File name: %s",files_path.dentry->d_iname);
 				printk("Inode no: %lu",files_path.dentry->d_inode->i_ino);
 				printk("File length in bytes: %lld", current_inode->i_size);
 				printk("Number of blocks allocated to file: %ld", current_inode->i_blocks);
 				printk("Current directory of process: %s",cwd);
 				//printk("File name %s",files_path.dentry->d_name.name); //same with d_iname

 				//buffer cache == adress_space
 				struct address_space *inode_adr_space = current_inode->i_mapping;
 				struct xarray array = inode_adr_space->i_pages;

 				xa_mark_t filter = XA_PRESENT;
 				xa_extract(&array, copy_array, 0, 100, 100, filter); //copy xarray(array of pointers) to regular array 
 				int j = 0;
 				
 				struct inode *current_host = inode_adr_space->host; //storage device the block is in 
 				atomic_t count = current_host->i_count;
 				atomic_t read_count = current_host->i_readcount; 
 				atomic_t write_count = current_host->i_writecount;
 				unsigned long host_ino = current_host->i_ino;

 				//use counts 
 				printk("Host(inode or block device): %ld", host_ino);
 				printk("Use Count: %d", atomic_read(&count));
 				printk("Read Count: %d", atomic_read(&read_count));
 				printk("Write Count: %d", atomic_read(&write_count));

 				while(j != 100)
 				{
 					printk("Block No %d: %p", j, copy_array[j]);
 					j++;
 				}
 				i++;
 			}
		}
	}
	return 0;
}
static void mod_exit(void){
	printk(KERN_ALERT "Goodbye\n");
}
module_init(mod_init);
module_exit(mod_exit);
