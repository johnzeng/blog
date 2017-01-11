#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>

struct jprobe exec_jp;


int jp_do_execve(const char * filename,
    const char __user *const __user *argv,
    const char __user *const __user *envp,
    struct pt_regs * regs)
{
    int cnt = 0;

    printk("filename = %s\n", filename);

    for(; *argv != NULL;argv++,cnt++)
        printk("argv[%d] = %s\n", cnt, *argv);

    jprobe_return();
    return 0;
}


static __init int jprobes_exec_init(void)
{    
    exec_jp.kp.symbol_name = "do_execve";

    exec_jp.entry = JPROBE_ENTRY(jp_do_execve);

    /*注册jprobes*/
    register_jprobe(&exec_jp);
    
    return 0;
}



static __exit void jprobes_exec_cleanup(void)
{
    /*撤销jprobes注册*/
    unregister_jprobe(&exec_jp);
}

module_init(jprobes_exec_init); 
module_exit(jprobes_exec_cleanup);
MODULE_LICENSE("GPL");
