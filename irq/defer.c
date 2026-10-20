/*
 * Copyright (c) 2020 Techveda
 *
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
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/sched/task.h>

#define MODNAME "[deferred work]"

static void fn_deferred(struct work_struct *);

static DECLARE_WORK(work, (work_func_t)fn_deferred);

static void fn_deferred(struct work_struct *w_arg)
{
   pr_info("%s:current task name:%s pid:%d\n",__func__, current->comm, (int)current->pid);
   schedule_work(&work);
}

static int __init my_init(void)
{
	pr_info("%s: scheduling deferred work\n",MODNAME);
	schedule_work(&work);
	return 0;
}

static void __exit my_exit(void)
{
	cancel_work_sync(&work);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("deferring work");
MODULE_LICENSE("Dual MIT/GPL");
