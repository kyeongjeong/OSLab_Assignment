#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#define __NR__ftrace 336
#define FILE_LENGTH 500

void **syscall_table;
void *real_ftrace;

void make_rw(void *addr);
void make_ro(void *addr);

static asmlinkage pid_t file_varea(const struct pt_regs *regs) {

    pid_t trace_task = regs->di;
    struct task_struct *task;
    struct mm_struct *mm;
    struct vm_area_struct *vm;
    struct file *exe_file;
    char* file_path;
    char buf[FILE_LENGTH];
    
    task = pid_task(find_vpid(trace_task), PIDTYPE_PID);
    if (!task) {
        printk("Process with PID %d not found.\n", trace_task);
        return -1;
    }

    mm = task->mm;
    if (!mm) {
        printk("Memory information not available for PID %d.\n", trace_task);
        return -1;
    }

    printk("######## Loaded files of a process '%s(%d)' in VM ########\n", task->comm, task->pid);

    vm = mm->mmap;
    while (vm) { 
        
        down_read(&mm->mmap_sem);
        exe_file = vm->vm_file;    
        
        if (exe_file) {
            
            get_file(exe_file);
            up_read(&mm->mmap_sem);
            file_path = d_path(&exe_file->f_path, buf, FILE_LENGTH);
            printk("mem[%lx~%lx] code[%lx~%lx] data[%lx~%lx] heap[%lx~%lx] %s\n", vm->vm_start, vm->vm_end, mm->start_code, mm->end_code, mm->start_data, mm->end_data, mm->start_brk, mm->brk, file_path);
        }
        vm = vm->vm_next;
    }

    printk("##################################################################\n");

    return task->pid;
}

static int __init file_varea_init(void) {

    syscall_table = (void**) kallsyms_lookup_name("sys_call_table");

    make_rw(syscall_table);
    real_ftrace = syscall_table[__NR_ftrace];
    syscall_table[__NR_ftrace] = file_varea;

    return 0;
}

static void __exit file_varea_exit(void) {

    syscall_table[__NR_ftrace] = real_ftrace;

    make_ro(syscall_table);
}

void make_rw(void *addr) {

    unsigned int level;
    pte_t * pte = lookup_address((u64)addr, &level);
    if(pte->pte &~ _PAGE_RW)
        pte->pte |= _PAGE_RW;
}

void make_ro(void *addr){

    unsigned int level;
    pte_t * pte = lookup_address((u64)addr, &level);

    pte->pte = pte->pte &~ _PAGE_RW;
}

module_init(file_varea_init);
module_exit(file_varea_exit);
MODULE_LICENSE("GPL");
