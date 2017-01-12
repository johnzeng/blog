#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/mm.h>
#include <linux/string.h>

struct jprobe get_user_pages_jp;
struct jprobe handle_mm_fault_jp;
struct jprobe follow_page_jp;

int __jp_get_user_pages(struct task_struct *tsk, struct mm_struct *mm,
		     unsigned long start, int nr_pages, unsigned int gup_flags,
		     struct page **pages, struct vm_area_struct **vmas,
             int *nonblocking){
    char *command = current->comm;
    if (strcmp(command, "dcopoc") == 0) {
        printk("go into get user pages");
    }
    jprobe_return();
    return 0;

}

int jp_handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags){

    char *command = current->comm;
    if (strcmp(command, "dcopoc") == 0) {
        printk("go into mm fault, address and flag is : %ld, %d", address, flags);
    }
    jprobe_return();
    return 0;

}

struct page *jp_follow_page(struct vm_area_struct *vma, unsigned long address,
			unsigned int flags)
{
    char *command = current->comm;
    if (strcmp(command, "dcopoc") == 0) {
        printk("go into follow pages, address and flag is : %ld, %d", address, flags);
    }
    jprobe_return();
    return 0;

}

static __init int jprobes_exec_init(void)
{    
    get_user_pages_jp.kp.symbol_name = "__get_user_pages";

    get_user_pages_jp.entry = JPROBE_ENTRY(__jp_get_user_pages);

    /*注册jprobes*/
    register_jprobe(&get_user_pages_jp);
    
    handle_mm_fault_jp.kp.symbol_name = "handle_mm_fault";

    handle_mm_fault_jp.entry = JPROBE_ENTRY(jp_handle_mm_fault);

    /*注册jprobes*/
    register_jprobe(&handle_mm_fault_jp);

    follow_page_jp.kp.symbol_name = "follow_page";

    follow_page_jp.entry = JPROBE_ENTRY(jp_follow_page);

    /*注册jprobes*/
    register_jprobe(&follow_page_jp);
    return 0;
}



static __exit void jprobes_exec_cleanup(void)
{
    /*撤销jprobes注册*/
    unregister_jprobe(&get_user_pages_jp);
    unregister_jprobe(&handle_mm_fault_jp);
    unregister_jprobe(&follow_page_jp);
}

module_init(jprobes_exec_init); 
module_exit(jprobes_exec_cleanup);
MODULE_LICENSE("GPL");
