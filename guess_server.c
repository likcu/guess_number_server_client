#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

static char buffer[1000];

//create a random number between 1 to 50;
int create_rand(){
	time_t t;
	srand((unsigned)time(&t));
	return ((rand() % 50) == 0)? 50 : (rand() % 50);
}

//check if the given str is a number or not.
int ifNumber(char* str){
	while(*str != '\0'){
		if(*str >= '0' && *str <= '9'){
			str++;
		}else{
			return -1;
		}
	}
	return 1;
}

//Create a socket and set the port
int do_open(){
	int s;

	// Step 1: Get Address stuff
	struct addrinfo hints, *result;

	// Setting up the hints struct...
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo("localhost", "1993", &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(1);
	}

	// Step 2: Create the socket
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Step 3: Bind the socket
	if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
		perror("bind()");
		exit(1);
	}

	// Step 4: Listen
	if (listen(sock_fd, 10) != 0) {
		perror("listen()");
		exit(1);
	}

	struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
	printf("Listening on file descriptor %d, port %d\n", sock_fd, ntohs(result_addr->sin_port));

	// Step 5: Accept a connection
	printf("Waiting for connection...\n");
	int client_fd = accept(sock_fd, NULL, NULL);
	printf("Connection made: client_fd=%d\n", client_fd);

	return client_fd; 
}

//Receive message
void recieve_message(int client_fd){
	int len = read(client_fd, buffer, sizeof(buffer) - 1);
	buffer[len] = '\0';
}

//Send message
void send_message(char *msg, int sock_fd){
	write(sock_fd, msg, strlen(msg));
}

int main(int argc, char **argv){
	int rand = create_rand();//create a random num between 1 to 50.

	int client_socket_id = do_open();

	int times;
	while(1){
		recieve_message(client_socket_id);
		if(ifNumber(buffer) == 1){
			int number = atoi(buffer);
			times++;
			if(number == rand){
				char counts[50];
				sprintf(counts,"%d",times);
				send_message(counts,client_socket_id);
				break;
			}
			else if(number < rand){
				send_message("too low\n",client_socket_id);
			}else{
				send_message("too high\n",client_socket_id);
			}
		}else{
			send_message("it's not a number,try again.\n",client_socket_id);
		}
	}
	close(client_socket_id);
	return 0;

}