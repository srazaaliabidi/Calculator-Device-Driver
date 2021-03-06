#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)


int main(){
	int num;
	int fd;
	int32_t val, number1, number2, output, operation;
	printf("\n--------------------------\n");
	printf("\n Opening Driver...\n");

	fd = open("/dev/etx_device", O_RDWR);
	if(fd < 0) {
		printf("Can't open device file...\n");
		return 0;
	}

	printf("Operation to preform:\n\n");
	printf("\
        1. Add \n \
        2. Subtract \n \
        3. Multiply \n \
        4. Divide \n\n\n ");

	scanf("%d", &num);
	if(num > 4 && num < 1){
		printf("Enter a number between 1 and 4");
		return 0;
	}
	ioctl(fd, WR_VALUE, (int32_t*) &num);

	printf("Enter first value: ");
	scanf("%d", &number1);

	printf("Enter second value: ");
	scanf("%d", &number2);
	printf("writing value to driver...\n");

	ioctl(fd, WR_VALUE, (int32_t*) &number1);
	ioctl(fd, WR_VALUE, (int32_t*) &number2);

	printf("Reading value from driver...\n");
	ioctl(fd, RD_VALUE, (int32_t*) &val);
	printf("Value is: %d\n", val);

	printf("Closing our driver....\n");
	close(fd);
}
