#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

typedef struct INPUT_DATA 
{
	char data[16];
}	INPUT_DATA;
#define MODULE_NAME "ioctl_module"
#define MAJOR_NUMBER 232
#define WRITE_TO_MODULE _IOW(MAJOR_NUMBER, 0, INPUT_DATA)
#define READ_FROM_MODULE _IOR(MAJOR_NUMBER, 0, INPUT_DATA)

int main()
{
	int fd;
	INPUT_DATA to, from;
	
	if((fd = open("./ioctl_module", O_RDWR)) < 0)
		perror("open");
	strncpy(to.data, "from user spc",sizeof(INPUT_DATA));
	printf("%s\n", to.data);
	if(ioctl(fd, WRITE_TO_MODULE, &to) < 0)
		printf("ERROR WRITE_TO_MODULE\n");
	if(ioctl(fd, READ_FROM_MODULE, &from) < 0)
		printf("ERROR READ_FROM_MODULE\n");
	printf("%s\n", from.data);
	close(fd);
	return 0;
}