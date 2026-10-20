/*
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Device Drivers"
 * Course offered by Techveda <www.techveda.org>
 *
 * (C) 2020 Raghu Bharadwaj/Techveda
 *
 * Git repository:
 *   https://gitlab.com/techveda/ldd
 * ******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/task.h>	// {get,put}_task_struct()
#include <linux/sched/signal.h>	// signal_pending()
#include <linux/signal.h>	// allow_signal()
#include <linux/kthread.h>

#define MODULE_NAME	"mykthread"
struct task_struct *Kthrd;

/* Our simple kernel thread. */
static int mykt(void *arg)
{
	pr_info("%s:name: %s PID: %d TGID: %d\n",
		__func__, current->comm, current->pid, current->tgid);

	allow_signal(SIGINT);
	allow_signal(SIGQUIT);

	while (1) {
		pr_info("%s:KThread %d going to sleep now...\n", __func__,
			current->pid);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();	// and yield the processor...
		// we're back on! has to be due to either the SIGINT or SIGQUIT signal!
		if (signal_pending(current))
			break;
	}
	// We've been interrupted by a signal...
	set_current_state(TASK_RUNNING);
	pr_info("%s:KThread %d exiting now...\n", __func__, current->pid);
	do_exit(0);
	BUG();
	return 0;
}

static int kthrd_init(void)
{
	int ret = 0, i = 0;

	pr_info("kt1: Create a kernel thread...\n");

	Kthrd = kthread_run(mykt, NULL, "%s.%d", MODULE_NAME, i);
	/* 2nd arg is (void * arg) to pass, ret val is task ptr on success */
	if (ret < 0) {
		pr_err("kt1: kthread_create failed (%d)\n", ret);
		return ret;
	}
	get_task_struct(Kthrd);	// inc refcnt, "take" the task struct

	pr_info("Module %s initialized, thread task ptr is 0x%pK.\n"
		"See the new k thread '%s.0' with ps "
		"(and kill it with SIGINT or SIGQUIT)\n",
		MODULE_NAME, Kthrd, MODULE_NAME);
	return 0;
}

static void kthrd_exit(void)
{
	if (Kthrd)
		kthread_stop(Kthrd);
	put_task_struct(Kthrd);	// dec refcnt, "release" the task struct
	pr_info("%s:Module %s unloaded.\n", __func__, MODULE_NAME);
}

module_init(kthrd_init);
module_exit(kthrd_exit);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org");
MODULE_DESCRIPTION("Simple kernel thread");
MODULE_LICENSE("Dual MIT/GPL");
