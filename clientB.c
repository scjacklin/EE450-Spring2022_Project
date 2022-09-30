/*

USC EE450
Final project

*/

/*
Client(TCP client)
client A

Sheng Chieh Lin 
USC ID: 2314107438


*/

////---- Log ----////
// 2022-0314 // test TCP packet 
// 2022-0314 // add Client A on screen messages
// 2022-0327 // create module file
// 2022-0327 // header file
// 2022-0327 // change name length
// 2022-0415 // client A

////
#include "header.h"

 
int sockfd_client_TCP; // client TCP socket


struct sockaddr_in ServerM_TCP_server_addr; // TCP server address
struct sockaddr_in client_TCP_addr; // TCP client address


//////////
int client_number = 1; // 0:A, 1:B
int TCP_C_port_arr[2]; //// save port


//// function
void create_client_TCP_socket_0415(int client_number_IN)
{
	// create socket 
	sockfd_client_TCP = socket(AF_INET, SOCK_STREAM , 0); // Create client TCP socket // SOCK_STREAM TCP

	if (sockfd_client_TCP == -1) 
	{
        perror("[ERROR] client TCP: can not open socket");
        exit(1);
    }
	
	// setup scheduler IP and port
	memset(&client_TCP_addr, 0, sizeof(client_TCP_addr)); //  make sure the struct is empty
    client_TCP_addr.sin_family = AF_INET; // Use IPv4 address family
    client_TCP_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    client_TCP_addr.sin_port = htons(TCP_C_port_arr[client_number_IN]); // scheduler Server port number
	

	
	
}



int main(int argc , char *argv[])
{
	char client_number_name[]= "AB";
	
	if (argc < 2)
		perror("usage: error initial"); // error input
	
	/////////////////////////////////////////////////////////
	
	TCP_C_port_arr[0] = ServerM_TCP_PORT_4CA;
	TCP_C_port_arr[1] = ServerM_TCP_PORT_4CB;
	
	// void create_client_TCP_socket_0415(int client_number)
	create_client_TCP_socket_0415(client_number);
	
	// connect
	
	if ( connect(sockfd_client_TCP, (struct sockaddr *)&client_TCP_addr,sizeof(client_TCP_addr)))
	{
        perror("[ERROR] client TCP: fail to connect");
        exit(1);
    }

    printf("The client %c is up and running \n" , client_number_name[client_number]); // !!! on screen messages !!!
	
	
	/////////////////////////////////////////////////////////
	
	TCP_C_port_arr[0] = ServerM_TCP_PORT_4CA;
	TCP_C_port_arr[1] = ServerM_TCP_PORT_4CB;
	
	
	////////==== client to serverM message ====////////
	//// decive ID
	/*	
			0 ServerM
			1 Backend ServerA
			2 Backend ServerB
			3 Backend ServerC
			4 Client A
			5 Client B
	*/
	
	//// operations
	/*						send 	Receive 
			0 check wallet	(o)
			1 TxCoins		(o)
			2 TxList		(o)
			3 Stats			(o)
	*/
	
	// create message 
	Data_C_BT_M C2M_message;
		
	C2M_message.decive_ID = 4 + client_number; //// client_number: 0: client A, 1: client b //// ID 4: client A,  5: client B
	// printf("C2M_message.decive_ID = [%d] . \n", C2M_message.decive_ID); // !!! on screen messages !!!
	
	if (argc == 2) //// Check wallet OR TXLIST
	{		
		if(strcmp(argv[1] , "TXLIST") == 0)	{
			C2M_message.operation = 2 ; // TXLIST
			strncpy(C2M_message.user_1, "0", MAX_NAME_LENGTH); 
			strncpy(C2M_message.user_2, "0", MAX_NAME_LENGTH);					
			C2M_message.amount = 0;
			// C2M_message.payload = 0;
			
			// send to serverM
			send(sockfd_client_TCP, &C2M_message, sizeof(C2M_message),0);	
			printf("\"client %c\" sent a sorted list request to the main server. \n", client_number_name[client_number]); // !!! on screen messages !!!
			
			
		
		}
		else {
			C2M_message.operation = 0 ; // Check wallet
			strncpy(C2M_message.user_1, argv[1], MAX_NAME_LENGTH); 
			strncpy(C2M_message.user_2, "0", MAX_NAME_LENGTH);					
			C2M_message.amount = 0;
			// C2M_message.payload = 0;
			
			// //// send to serverM
			send(sockfd_client_TCP, &C2M_message, sizeof(C2M_message),0);			
			printf("\"%s\" sent a balance enquiry request to the main server. \n" , argv[1]);  // !!! on screen messages !!!
		
		}
	} //// if (argc == 2)	
	else if (argc == 3) //// Stats
	{	
		if(strcmp(argv[2] , "stats") == 0) {
			C2M_message.operation = 3 ; // Stats
			strncpy(C2M_message.user_1, argv[1], MAX_NAME_LENGTH); 
			strncpy(C2M_message.user_2, "0", MAX_NAME_LENGTH);					
			C2M_message.amount = 0;
			// C2M_message.payload = 0;	
			
			
			//// send to serverM
			send(sockfd_client_TCP, &C2M_message, sizeof(C2M_message),0);
			
			printf("\"%s\" sent a statistics enquiry request to the main server. \n" , argv[1]);  // !!! on screen messages !!!
			} //// if(strcmp(argv[2] , "stats") == 0)
		else 
			printf("[ERROR] incorrect input \n"); 
	
	} //// else if (argc == 3)
	else if (argc == 4) //// TxCoins
	{
		C2M_message.operation = 1 ; // TxCoins
		strncpy(C2M_message.user_1, argv[1], MAX_NAME_LENGTH); 
		strncpy(C2M_message.user_2, argv[2], MAX_NAME_LENGTH);	
		C2M_message.amount = atoi(argv[3]);   //// char to int
		// C2M_message.payload = 0;
		
				
		//// send to serverM
		send(sockfd_client_TCP, &C2M_message, sizeof(C2M_message),0);
		
		printf("\"%s\" has requested to transfer %s coins to \"%s\". \n" , argv[1], argv[3], argv[2]);  // !!! on screen messages !!!
		
	
	} //// else if (argc == 3)
		
	//////////////////////////////////////////////////
	
	
	if(C2M_message.operation != 2) //// TXLIST don't need response
	{
		//////////////////////////
		//// wait response from serverM
		
		Data_M2C M2C_message_recv;
		recv(sockfd_client_TCP,&M2C_message_recv,sizeof(M2C_message_recv),0);
		
		// //// test print 
		// printf("======================================================\n");
		// printf("Receive from serverM \n");
		// printf("got  = %c \n", M2C_message_recv); 			
		// printf("got ID = %d \n", M2C_message_recv.decive_ID); 
		// printf("got operation = %d \n", M2C_message_recv.operation);
		// printf("got status = %d \n", M2C_message_recv.status);  	
		// printf("got balance = %d \n", M2C_message_recv.balance);				
		// printf(" SIZE = %d \n", sizeof(M2C_message_recv)); 
		
		
		//// switch 
		switch (M2C_message_recv.status)
		{
			case 0: //// display balance 
				printf("The current balance of \"%s\" is : %d alicoins. \n", C2M_message.user_1, M2C_message_recv.balance);
				
			break;
			
			case 1: //// user not on the list balance 
				printf("Unable to proceed as \"%s\" is not part of the network. \n", C2M_message.user_1);	
				
				
				
			
			break;
			
			case 2: //// trans success 
				
				printf("\"%s\" successfully transferred %d alicoins to \"%s\". \n", C2M_message.user_1, C2M_message.amount, C2M_message.user_2);
				printf("The current balance of \"%s\" is : %d alicoins. \n", C2M_message.user_1, M2C_message_recv.balance_1);
				
					
			break;
			
			case 3: //// trans fail - low balance
			
				printf("\"%s\" was unable to transfer %d alicoins to \"%s\" because of insufficient balance. \n", C2M_message.user_1, C2M_message.amount, C2M_message.user_2);
				printf("The current balance of \"%s\" is : %d alicoins. \n", C2M_message.user_1, M2C_message_recv.balance_1);
					
			break;
			
			case 4: //// trans fail - sender is not in network	
				printf("Unable to proceed with the transaction as \"%s\" is not part of the network. \n", C2M_message.user_1);			
			
			break;
			
			case 5: //// trans fail - receiver is not in network
				printf("Unable to proceed with the transaction as \"%s\" is not part of the network. \n", C2M_message.user_2);	
			
			break;
			
			case 6: //// trans fail - sender and receiver are not in network		
				printf("Unable to proceed with the transaction as \"%s\" and \"%s\" are not part of the network. \n", C2M_message.user_1,  C2M_message.user_2);
					
			break;
			
			case 7:  //// trans list 
				printf("\"%s\" statistics are the following.:\nRank--Username--NumofTransacions--Total \n", C2M_message.user_1);
				
				for(int i=0; i < M2C_message_recv.payload_number; i++ )
				{
					printf("%d--", i+1);				
					printf("%s--", M2C_message_recv.state_list[i].user);
					printf("%d--",M2C_message_recv.state_list[i].number_transactions);				
					printf("%d \n", M2C_message_recv.state_list[i].total_amount);
				}
					
			break;
			
			default:
			printf("ERROR: receiver wrong info from serverM \n");
		
		}
	
	}
	
	
	
	
	
	/////////////////// close socket and client 
	close(sockfd_client_TCP);
	
	
	
	
	
	//// on screen messages
	//// Client A
	/*
	(O) Booting Up: “The client A is up and running.”
	
	Upon sending the input to main server for checking balance: "<USERNAME> sent a balance enquiry request to the main server.”
	Upon sending the input(s) to the main server for making a transaction. : “<SENDER_USERNAME> has requested to transfer <TRANSFER_AMOUNT> coins to <RECEIVER_USERNAME>.”
	
	
	After receiving the balance information from the main server:
	“The current balance of <USERNAME> is : <BALANCE_AMOUNT> alicoins.”
	
	
	After receiving the transaction information from the main server (if successful): 
	“<SENDER_USERNAME> successfully transferred <TRANSFER_AMOUT> alicoins to <RECEIVER_USERNAME>.
		The current balance of <SENDER_USERNAME> is : <BALANCE_AMOUNT> alicoins.”
	
	
	
	After receiving the transaction information from the main server (if transaction fails due to insufficient balance): 
	“<SENDER_USERNAME> was unable to transfer <TRANSFER_AMOUT> alicoins to <RECEIVER_USERNAME> because of insufficient balance.
		The current balance of <SENDER_USERNAME> is : <BALANCE_AMOUNT> alicoins.”
		

	After receiving the transaction information from the main server (if one of the clients is not part of the network):
		“Unable to proceed with the transaction as <SENDER_USERNAME/RECEIVER_USERNAME> is not part of the network.”
	
	After receiving the transaction information from the main server (if both the clients are not part of the network):
		“Unable to proceed with the transaction as <SENDER_USERNAME> and <RECEIVER_USERNAME> are not part of the network.”
	
	Upon sending the input to main server for requesting a sorted list:
		“<USERNAME> sent a sorted list request to the main server.”
	
	Upon sending the input to main server for requesting statistics:
		"<USERNAME> sent a statistics enquiry request to the main server.”
	
	After receiving the transaction information from the main server (list of transactions):
		"<USERNAME> statistics are the following.:” Rank--Username--NumofTransacions--Total
	*/
	
	
	
	
	//// printf("\n" , ); 
	
	
	
	return 0;	
}











