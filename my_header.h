#ifndef _MY_HEADER_H
#define _MY_HEADER_H

#include <uapi/asm-generic/ioctl.h>
typedef struct INPUT_DATA 
{
	char data[16];
}	INPUT_DATA;
#define MODULE_NAME "ioctl_module"
#define MAJOR_NUMBER 232
#define WRITE_TO_MODULE _IOW(MAJOR_NUMBER, 0, INPUT_DATA)
#define READ_FROM_MODULE _IOR(MAJOR_NUMBER, 0, INPUT_DATA)
#endif 