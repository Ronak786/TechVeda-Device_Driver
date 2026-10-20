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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/sched/task.h>

#define MODULE_NAME "tbh"
#define SHARED_IRQ 1
static int irq = SHARED_IRQ, my_dev_id, irq_counter = 0;
module_param(irq, int, S_IRUGO);

static DECLARE_WAIT_QUEUE_HEAD(wq);
static atomic_t cond;

static struct task_struct *tsk;

static int t_fun(void *t_arg);

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	irq_counter++;
	pr_info("In the ISR: counter = %d\n", irq_counter);

	/* set atomic counter and wake up thread (BH) */
	atomic_set(&cond, 1);
	wake_up_interruptible(&wq);
	return IRQ_NONE;
}

/* BH routine */

static int t_fun(void *t_arg)
{
	do {
		atomic_set(&cond, 0);
		/* wait until either thread is suspended or an atomic 
		   counter is set */
		wait_event_interruptible(wq, kthread_should_stop()
					 || atomic_read(&cond));
		pr_info("%s:Entering t_fun, pid = %d ,context of %s\n",
			__func__, current->pid, current->comm);

	} while (!kthread_should_stop());
	do_exit(0);
	BUG();
	return 0;
}

static int __init my_init(void)
{
	if (request_irq
	    (irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id))
		return -1;

	tsk = kthread_run(t_fun, NULL, "%s.0", MODULE_NAME);
	get_task_struct(tsk);

	return 0;
}

static void __exit my_exit(void)
{
	synchronize_irq(irq);
	free_irq(irq, &my_dev_id);
	if (tsk)
		kthread_stop(tsk);
	put_task_struct(tsk);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("Interrupt Handler Test");
MODULE_LICENSE("Dual MIT/GPL");
