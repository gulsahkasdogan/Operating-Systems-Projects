/*
CS342 Project 4
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 24.05.2019
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;

	/* fork a child process*/
	pid = fork();

	if(pid < 0){ /* error occurred */
		fprintf(stderr, "Fork Failed");
		return -1;
	}
	else if(pid == 0){ /* child process */
		
	}
	else{ /* parent process */
		/*parent will wait for the child to complete */
		wait(NULL);
		printf("%d\n", pid);
		for( int k = 1; k < 1000; k++ ){

		    struct timeval t1,t2;

		    double cost_time;

		    int file,in;

		    char *c = (char *) calloc(10*k, sizeof(char));
		    file = open("sample.txt",O_CREAT | O_WRONLY,0777); //read, write & execute
		    if(file == -1)
		    {
		        printf("Opening file failed. \n");
		        exit(1);
		    }
		    write(file, "Project 4 was a nightmare! \n",10*k);

		    close(file);

		    sleep(5);
		    file = open("sample.txt", O_RDONLY);
		    if(file == -1)
		    {
		    	printf("Failed to create.\n");
		    	exit(1);
		    }
		    //gettimeofday(&t1, NULL);

		    in = read(file, c, 10*k);
		    //gettimeofday(&t2, NULL);

		    //cost_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
		    //cost_time += (t2.tv_usec - t1.tv_usec) / 1000.0; //time in milliseconds

		    //printf("Time(in milliseconds): ");
		    //printf("%lf\n" ,cost_time);
		    printf(" %d bytes  were read.\n", in);

		    c[in] = '\0';  //last char to be null

		    close(file);
	   }
	}

    return 0;
}
