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

#define  SECOND         0x00  //Second       (00..59) 
#define  MINUTE         0x02  //Minute       (00..59)
#define  HOUR           0x04  //Hour         (00..23) 
#define  DAY_IN_WEEK    0x06  //Day of week  (01..07)
#define  DAY            0x07  //Day          (01..31) 
#define  MONTH          0x08  //Month        (01..12) 
#define  YEAR           0x09 // Year         (00..99) 

#define  ADDRESS_REG       0x70
#define  DATA_REG          0x71
#define  ADDRESS_REG_MASK  0xe0

struct rtc_time {
        unsigned char sec;
        unsigned char min;
        unsigned char hour;
        unsigned char day;
        unsigned char mon;
        unsigned char year;

};

# define SUCCESS 0
# define DEV_NAME "myrtc"










/*

ioctl implementation 
====================


step 1: identify config ops possible on target device

step 2: create "request code" for each config operation

step 3: implement ioctl call back function and register 
           it with "unlocked_ioctl" interface of 
           file_operations structure.












*/

 #define CONFIG0  0
 #define CONFIG1  10

/*

   To encode request commnands the following params are needed
    1. MAGIC NO(8 bit)(usually common for all commands)
    2. Seq no(8 bit) (incremental)
    3. Arg type(can vary for each command: may or may not be 
	         applicable)
    4. Direction of data transfer( in | out param: may or may 
                                   not be applicable)


 encoding macros:
    _IO(MAGIC,seqno)
	-- use this when arg is not involved 
    _IOW(MAGIC, seqno, type)
	-- use this when arg is in-param for application
    _IOR(MAGIC, seqno, type)
	-- use this when arg is out-param for application
    _IOWR(MAGIC, seqno, type)
	-- use this if arg is used both as in/out-param.

*/


#define VEDA_MAGIC       'V'

#define  SET_SECOND  _IOW(VEDA_MAGIC, 1, char)
#define  SET_MINUTE  _IOW(VEDA_MAGIC, 2, char)
#define  SET_HOUR    _IOW(VEDA_MAGIC, 3, char)
#define  SET_DAY     _IOW(VEDA_MAGIC, 4, char)
#define  SET_MONTH   _IOW(VEDA_MAGIC, 5, char)
#define  SET_YEAR    _IOW(VEDA_MAGIC, 6, char)



/* Example -- memory device (vramdev) ioctls

1. VGA_FILLZERO
#define VGA_FILLZERO 	_IO(MAGIC,seqno)

2. VGA_FILLCHAR
#define VGA_FILLCHAR	_IOW(MAGIC,seqno,char)

3. VGA_GETSIZE
#define VGA_GETSIZE	_IOR(MAGIC,seqno,uint)

4. VGA_SETSIZE
#define VGA_SETSIZE 	_IOW(MAGIC,seqno,uint)
*/












