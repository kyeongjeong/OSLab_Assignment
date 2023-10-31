#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <linux/init_task.h>
#define __NR__ftrace 336

void **syscall_table;
void *real_ftrace;

void make_rw(void *addr);
void make_ro(void *addr);

static asmlinkage pid_t process_tracer(const struct pt_regs *regs) {
 
    struct task_struct *task = &init_task;
    struct task_struct *sibling, *child;
    struct list_head *p_list;
    int sibling_count = 0, child_count = 0;
    pid_t trace_task = regs->di;

    for_each_process(task) {
        
        if (task->pid == trace_task) {

            printk("##### TASK INFORMATION of ''[%d] %s'' #####\n", task->pid, task->comm);

            char *state_str;
            switch (task->state) {
                case TASK_RUNNING:
                    state_str = "Running or Ready";
                    break;
                case TASK_INTERRUPTIBLE:
                    state_str = "Wait with ignoring all signals";
                    break;
                case TASK_UNINTERRUPTIBLE:
                    state_str = "Wait";
                    break;
                case __TASK_STOPPED:
                    state_str = "Stopped";
                    break;
                case EXIT_ZOMBIE:
                    state_str = "Zombie Process";
                    break;
                case EXIT_DEAD:
                    state_str = "Dead";
                    break;
                default:
                    state_str = "etc.";
                    break;
            }
            printk("- task state : %s\n", state_str);

            printk("- Process Group Leader: [%d] %s\n", task->group_leader->pid, task->group_leader->comm);
            
            printk("- Number of context switches : %ld\n", task->nvcsw + task->nivcsw);

            printk("- Number of calling fork() : %d\n", task->fork_count);

            printk("- it's parent process : [%d] %s\n", task->parent->pid, task->parent->comm);

            printk("- it's sibling process(es) :\n");
            list_for_each(p_list, &task->parent->children) {

                sibling = list_entry(p_list, struct task_struct, sibling);
                
                if(task->pid != sibling->pid) {
                    printk("  > [%d] %s\n", sibling->pid, sibling->comm);
                    ++sibling_count;
                }
                else
                    continue;
            }
            if(sibling_count > 0)
                printk("  > This process has %d sibling process(es)\n", sibling_count);
            else 
                printk("  > It has no sibling.\n");

            printk("- it's child process(es) :\n");
            list_for_each(p_list, &task->children) {

                child = list_entry(p_list, struct task_struct, sibling);
                printk("  > [%d] %s\n", child->pid, child->comm);
                ++child_count;
            }

            if(child_count > 0)
                printk("  > This process has %d child process(es)\n", child_count);
            else 
                printk("  > It has no child.\n");

            printk("##### END OF INFORMATION #####\n");
            return task->pid;
        }    
    }
    return -1;
}

static int __init process_tracer_init(void) {

    syscall_table = (void**) kallsyms_lookup_name("sys_call_table");

    make_rw(syscall_table);
    real_ftrace = syscall_table[__NR_ftrace];
    syscall_table[__NR_ftrace] = process_tracer;

    return 0;
}

static void __exit process_tracer_exit(void) {

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

module_init(process_tracer_init);
module_exit(process_tracer_exit);
MODULE_LICENSE("GPL");
