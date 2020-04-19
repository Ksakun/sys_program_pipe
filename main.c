#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/syscall.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>


#define MAX_STR_SIZE 10


int main(){
	pid_t pid1, pid2;
	int fd1[ 2]; //fd1[0]= read, fd1[1] = write
	int fd2[ 2]; //fd2[0] = read, fd2[1] = write
	int input =0;
	int output = 0;
	char buf[MAX_STR_SIZE];
	int i = 0;
	ssize_t n =0;

	//create 1st pipe
	if ( pipe(fd1) < 0){
		fprintf(stderr, "1st Pipe failed");
		return 0;
	}
	if ( pipe( fd2) < 0){
		fprintf(stderr, "2nd Pipe failed");
		return 0;
	}

	//parent proccess's work
	
	if((pid1 = fork()) > 0){
		input = open( "input.txt", 0);

		close ( fd1[ 0]); //fd1 only use write

		while(( n = read(input, buf, MAX_STR_SIZE + 1)) > 0){//read data from input.txt
			printf("%s\n", buf);
			write( fd1[ 1], buf, n);
			memset( buf, 0, sizeof( buf));
		}

		close( input);
		close( fd1[ 1]); //after writing, close

		if(( pid2 = fork()) == 0){

			pid_t ch2_pid = getpid();

			close( fd1[ 0]); close(fd1[ 1]); //1st pipe done
			close( fd2 [1]); //2nd pipe only use read

			output = creat("output.txt", 0666); //create output.txt
			
			while(( n = read(fd2[ 0], buf, MAX_STR_SIZE)) != 0){//read from 2nd pipe & write output.txt
				write(output, buf, n);
			}
			close( output);
			close( fd2[ 0]); //2nd pipe reading end, so close

			sleep( 1);
			exit( 2);
		}
		else if ( pid2 > 0) {
			close( fd2[ 0]); close( fd2[ 1]);

			pid_t child_pid =0;
			pid_t parent_pid = getpid();

			for ( i = 0; i < 2; i++){
				int status = 0;
				child_pid = wait( &status);
				status = 0;
			}

		}
		
		else{
			exit( -1);
		}
	}

	else if( pid1 == 0){ // child proccess's work
		pid_t ch1_pid = getpid();

		close( fd1[ 1]); // 1st pipe's read need, so close wirte
		close( fd2[ 0]); // 2nd pipe's wirte need, so close read

		while(( n = read(fd1[ 0], buf, MAX_STR_SIZE)) > 0){ //read data from 1st pipe
			for( i = 0; i < 10 ; i++){ //read data change to capital
				buf[ i] = toupper( buf[ i]);
			}
			write(fd2 [ 1], buf, n); //capital data write to 2nd pipe
		}

		close( fd1[ 0]);
		close( fd2[ 1]);

		sleep( 1);
		exit( 1); //signal to parent proccess, child proccess done
	}
	
	else{
		exit( -1);
	}
}







