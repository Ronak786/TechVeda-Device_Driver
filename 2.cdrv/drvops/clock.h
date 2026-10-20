/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * ******************************************************************************
 * This source code is part of the coursware provided with "Linux Device Drivers"
 * training program offered by Techveda <www.techveda.org>
 *
 * Copyright (C) 2020 Techveda
 *
 * Author: Raghu Bharadwaj
 *
 * Git repository:
 *   https://gitlab.com/techveda/ldd-0920
 * ******************************************************************************
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



/* clock.h -- definition of registers and commands to access RTC.
Version : 1.0
Author : Team -C
*/

# define RTC_PORT_BASE 0x70

# define RTC_DATA_PORT  0x71

# define RTC_CMD_PORT RTC_PORT_BASE

# define SECONDS_CMD 0
# define MINUTES_CMD 2
# define HOURS_CMD 4
# define DAY_CMD 7
# define MONTH_CMD 8
# define YEAR_CMD 9

/* cmd is SECONDS_CMD .... YEARS_CMD... */

# define READ_FROM_CLOCK(cmd,data)\
	outb_p(cmd,RTC_CMD_PORT);data=inb_p(RTC_DATA_PORT);

/* cmd is SECONDS_CMD .... YEARS_CMD... and value is the value to be set */
# define WRITE_TO_CLOCK(cmd,value) \
	outb_p(cmd,RTC_CMD_PORT); outb_p(value,RTC_DATA_PORT);

#define SUCCESS 0

# define FILE_SIZE 6
