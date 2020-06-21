#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define BUFFER_SIZE 500

int main()
{
	int fd, bytes_read;
	char message[BUFFER_SIZE];
	if ((fd = open("/dev/process_list", O_RDONLY)) < 0) //Open the character Device
	{
		printf("Unable to open the character device\n");
		return 0;
	}

	while ((bytes_read = read(fd, message, strlen(message))) > 0) // Print until read returns 0
	{
		printf("%s\n", message);
		memset(message, 0, sizeof(char) * BUFFER_SIZE);
	}

	if (bytes_read < 0)
	{
		perror("ERROR in reading from device Process_List!"); // Error handling condition
		return 0;
	}
	printf("\n");
	close(fd);
	return 0;
}
