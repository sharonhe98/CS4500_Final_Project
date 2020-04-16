#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include <thread>

#include "array.h"

#define PORT 3002


enum ServerState {
	STARTUP,
	LISTENING,
	SENDING,
	TERMINATE
};

int client_i = 0;
int max_clients = 3;

class Server {
	public:
		char* ip;
		IntArray* clients;
		ServerState currState;
		int server_fd;
		int currClientCount;
		int messagesRouted;

		int max_messages;

		//set of socket descriptors  
		fd_set readfds;

		struct sockaddr_in address;


	Server(char* ip_addr) {
		ip = ip_addr;
		server_fd = 0;
		clients = new IntArray();
		currState = STARTUP;
		currClientCount = 0;
		messagesRouted = 0;

		max_messages = 3;
	}

	~Server() {
		delete[] clients;
	}


  void acceptAllClients(char* ip_addr, char* port) {

	int portnum = atoi(port);

	int opt = 1;   
	int master_socket , addrlen , new_socket , client_socket[30] ,  
	activity, i , valread , sd;   
	int max_sd;         
         
	//a message  
	char *message = "REGISTERED new client\n";
	
     
	//initialise all client socket to 0 so not checked  
	for (i = 0; i < max_clients; i++)   
	{   
		clients->append(0); 
	}   
         
	//create a master socket  
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
	{   
		perror("socket failed");   
		exit(EXIT_FAILURE);   
	}   
     
	//set master socket to allow multiple connections ,  
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
		sizeof(opt)) < 0 )   
	{   
		perror("setsockopt");    
	}   
     
	//type of socket created  
	address.sin_family = AF_INET;   
	address.sin_addr.s_addr = inet_addr(ip_addr);   
	address.sin_port = htons( portnum );   
         
	//bind the socket to localhost port 8888  
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
	{   
		perror("bind failed");    
	}   


	server_fd = master_socket;
         
	//try to specify maximum of 3 pending connections for the master socket  
	if (listen(master_socket, 3) < 0)   
	{   
		perror("listen");    
	}   
         
	//accept the incoming connection  
	addrlen = sizeof(address);   
         
	while(currClientCount <= 3)   
	{   
		//clear the socket set  
		FD_ZERO(&readfds);   
     
		//add master socket to set  
		FD_SET(master_socket, &readfds);   
		max_sd = master_socket;   
             
		//add child sockets to set  
		for ( i = 0 ; i < clients->length() ; i++)   
		{   
			//socket descriptor  
			sd = clients->get(i);   
                 
			//if valid socket descriptor then add to read list  
			if(sd > 0)   
			FD_SET( sd , &readfds);   
                 
			//highest file descriptor number, need it for the select function  
			if(sd > max_sd)   
			max_sd = sd;   
		}   
     
        	//wait for an activity on one of the sockets , timeout is NULL ,  
        	//so wait indefinitely  
        	activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        	if ((activity < 0) && (errno!=EINTR))   
        	{   
			printf("select error");   
        	}   
             
        	//If something happened on the master socket ,  
        	//then its an incoming connection  
        	if (FD_ISSET(master_socket, &readfds))   
        	{   
			if ((new_socket = accept(master_socket,  
				(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
			{   
				perror("accept");   
				//      exit(EXIT_FAILURE);   
			}   
             
			//inform user of socket number - used in send and receive commands  
			printf("Connected! port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
			



           
	
			// add to current count
			currClientCount += 1;

			//printf("port set: %i\n", ports->get(i));
			//send new connection greeting message  
			if( send(new_socket, message, strlen(message), 0) == 0 )   
			{   
				perror("send");   
			}

			for (size_t i = 0; i < clients->length(); i++) {
				int oldSock = clients->get(i);
				if (oldSock != new_socket) {
					send(oldSock, message, strlen(message), 0);
				}
			}   
                 
            
     
			//add new socket to array of sockets  
			for (i = 0; i < clients->length(); i++)   
			{   
				//if position is empty  
				if( clients->get(i) == 0 )   
				{   
					clients->set(i, new_socket);   
					printf("Adding to clients at %d\n" , i);   
       
					break;   
				}   
			}   
		}


		sendFromServer();
   
         }
  }


  int sendFromServer() {
	// sourced from: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/


	currState = LISTENING;

	int addrlen = sizeof(address);
	char buffer[1025];  //data buffer of 1K  
	

//	while (messagesRouted < max_messages) {

        	//else its some IO operation on some other socket 
        	for (size_t i = 0; i < clients->length(); i++)   
        	{   
        		int sd = clients->get(i);   
                 
			int directedInd = i + 1;

			if (i == clients->length() - 1) {
				directedInd = 0;
			}

			char* message;

			switch (directedInd) {
				case 0:
					message = "hello 0!\n";
					break;
				case 1:
					message = "hello 1!\n";
					break;
				case 2:
					message = "hello 2!\n";
					break;

			}


			int sd2 = clients->get(directedInd);

         		if (FD_ISSET( sd , &readfds))   
            		{   
                		//Check if it was for closing , and also read the  
                		//incoming message  
				int valread = read( sd , buffer, 1024);
                		if (valread == 0)   
                		{   
                    			//Somebody disconnected , get his details and print  
                    			getpeername(sd , (struct sockaddr*)&address , \ 
                        		(socklen_t*)&addrlen);   
                   			printf("Host disconnected, port %d \n" ,  
                          		inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    			//Close the socket and mark as 0 in list for reuse  
                    			//close( sd );   
                    			clients->set(i, 0);   
                		}   
                     
                		//Echo back the message that came in  
                		else 
                		{   
					currState = SENDING;
                   			//set the string terminating NULL byte on the end  
                    			//of the data read  
                    			buffer[valread] = '\0';   
                    			send(sd2 , message , strlen(message) , 0 );
					messagesRouted += 1;

					printf("message: %s\n", message);

                		}   

            		}   
        	}
//	}

	if (messagesRouted > max_messages - 1) {
		for (size_t i = 0; i < clients->length(); i++) {
			int sd = clients->get(i);
			send(sd, "TERMINATE", strlen("TERMINATE"), 0);
		}
	}

	printf("messages routed so far: %i\n", messagesRouted);
      
         
  return 0;   	

  }
		
};


class Client {
	public:
		char* serv_ip;
		int messagesSent;
		KVStore kv;

	Client(char* ip_addr) {
		serv_ip = ip_addr;
		messagesSent = 0;
	}

	int runClient() {

		int sock = 0;
		struct sockaddr_in serv_addr;
		char buffer[1024] = {0};
		char *hello = "Hello from client";	

		sock = socket(AF_INET, SOCK_STREAM, 0);

		char *message = "Please route!\n"; 


		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(3002);

		if (inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr) <= 0) {
			printf("\nInvalid address\n");
			return - 1;
		}

		bzero(&(serv_addr.sin_zero), 8);


		int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

		if (connected < 0) {
			printf("Failed to connect\n");
		}

		kv = new KVStore(connected);
		message = serialize(REQUEST);
	
		/*if (send(sock, "Hello from client", strlen(hello), 0) < 0) {
			perror("send failed!\n");
		}*/
		send(sock, message, strlen(message), 0);

		messagesSent += 1;

		printf("buffer is: %i\n\n", strlen(buffer));


		while (strstr(buffer, "TERMINATE") == 0 || strlen(buffer) >= 0) {
			int value_read = read(sock, buffer, 1024);
			printf("value read from server: %i\n", strstr(buffer, "TERMINATE"));

			if (strstr(buffer, "TERMINATE") != nullptr) {
				printf("bread\n");
				break;
			}

			// in here, set data in kvstore
		}

		printf("broken from while\n");
		close(sock);
		printf("closed socket\n");
		exit(0);	


		return 0;

	}
};
