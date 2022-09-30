/*

USC EE450
Final project

*/

/*
(TCP server)


Sheng Chieh Lin 
USC ID: 2314107438


*/

////---- Log ----////
// 2022-0314 // test TCP packet 
// 2022-0327 // create module
// 2022-0327 // test 2 C payload

///////////////////////////
#include "header.h"

//////// !!!!!!!!  TEST 
int Backend_Server_number = 3;



//// TCP
int sockfd_ServerM_Server_TCP[2]; //  Server TCP socket
int sockfd_ServerM_Server_TCP_Accept; //  Server TCP socket

int sockfd_TCP_child; // scheduler Server TCP socket
struct sockaddr_in ServerM_TCP_server_addr; // TCP server address
struct sockaddr_in ServerM_TCP_server_addr_arr[2]; // TCP server address

struct sockaddr_in client_TCP_addr; // TCP client address

char TCP_recv_buffer[MAX_DATA_SIZE]={0}; // Date received from TCP client
char TCP_send_buffer[MAX_DATA_SIZE]={0}; // Date send to TCP client

////////
int client_port_arr[2];
int TCP_connect_idx;
char *TCP_client_name[2][10];
////////////
Data_C_BT_M C2M_message_recv;
Data_M2BackServer M_2_BA_message;

/////////////// UDP 
//// UDP 
int sockfd_serverM_Server_UDP;
struct sockaddr_in ServerM_UDP_server_addr; // UDP server address
struct sockaddr_in Backend_Server_UDP_addr[3];

char *Backend_Server_name[3][10];


char UDP_recv_buffer[MAX_DATA_SIZE]={0}; // Date received from UDP
char UDP_send_buffer[MAX_DATA_SIZE]={0}; // Data sent by UDP

////////
int backed_server_port_arr[3];
//// addr size 
socklen_t backend_A_addr_size[3];

//// MSG
Data_M2BackServer M_2_BA_message;

//// Data 
Data_user_transaction balance_user_transaction;

int name_list_count = 0;
Data_user_transaction name_list_record[MAX_TRANS_DATA];	 //// for state
Data_user_transaction name_list_record_sort[MAX_TRANS_DATA]; //// for state sort
////////////////////////////////////////////////////////////




//// TCP function
void create_ServerM_TCP_server_socket_0415(int client_number)
{	
	// printf("client_number [%d] \n", client_number); 
	// create socket 
	sockfd_ServerM_Server_TCP[client_number] = socket(AF_INET, SOCK_STREAM , 0); // Create TCP server socket // SOCK_STREAM TCP

	if (sockfd_ServerM_Server_TCP[client_number] == -1) 
	{
        perror("ServerM TCP server [can not open socket]");
        exit(1);
    }
	
	// setup scheduler IP and port
	memset(&ServerM_TCP_server_addr_arr[client_number], 0, sizeof(ServerM_TCP_server_addr_arr[client_number])); //  make sure the struct is empty
    ServerM_TCP_server_addr_arr[client_number].sin_family = AF_INET; // Use IPv4 address family
    ServerM_TCP_server_addr_arr[client_number].sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    ServerM_TCP_server_addr_arr[client_number].sin_port = htons(client_port_arr[client_number]); // Server port number
	
	// bind sockaddr
	
	if (bind(sockfd_ServerM_Server_TCP[client_number], (struct sockaddr *) &ServerM_TCP_server_addr_arr[client_number], sizeof(ServerM_TCP_server_addr_arr[client_number])) == -1)
	{
        perror("ServerM TCP server [fail to bind socket]");
        exit(1);
    }
	
	//// listen one client
	listen(sockfd_ServerM_Server_TCP[client_number],10);
}



void serverM_Check_wallet_func()
{
	// printf("===== serverM_Check_wallet_func ===== \n");
	
	//// send to backend server 
	memset(&M_2_BA_message, 0, sizeof(M_2_BA_message));
	
	M_2_BA_message.decive_ID = 0;
	M_2_BA_message.operation = 0; //// 0: balence 
	strncpy(M_2_BA_message.user_1, C2M_message_recv.user_1, MAX_NAME_LENGTH);		

	// printf("======================================================\n");
	// printf("UDP serverM send to backend \n"); 
	// printf("ID = %d \n", M_2_BA_message.decive_ID); 
	// printf("operation = %d \n", M_2_BA_message.operation);
	// printf("user_1 = %s \n", M_2_BA_message.user_1);	
	
	// printf("SIZE = %d \n", sizeof(M_2_BA_message)); 
	// printf("======================================================\n");
	
	// void UDP_check_wallet_func_0417()
	UDP_check_wallet_func_0417();
	
	
		// ////// !!!! TEST  
		// // void UDP_check_wallet_func_0417()
		// // output 
		// Data_user_transaction balance_user_transaction;	
		
		// strncpy(balance_user_transaction.user, "Chinmay", MAX_NAME_LENGTH);
		
		// balance_user_transaction.number_transactions = 0;
		// balance_user_transaction.send_amount = 2000;
		// balance_user_transaction.receive_amount = 2000;
		// balance_user_transaction.total_amount = 2000;
		// ////// !!!! END TEST  
	
	
	// printf("======================================================\n");
	// printf("UDP serverM got balance check from backend \n"); 
	// printf("user = %s:\n", balance_user_transaction.user);
	// printf("number_transactions = %d:\n", balance_user_transaction.number_transactions);
	// printf("send_amount = %d:\n", balance_user_transaction.send_amount);
	// printf("receive_amount = %d:\n", balance_user_transaction.receive_amount);
	// printf("total_amount = %d:\n", balance_user_transaction.total_amount);
	// printf("======================================================\n");
	
	//// send to client 
	Data_M2C M2C_message_send;	
	memset(&M2C_message_send, 0, sizeof(M2C_message_send));
	
	M2C_message_send.decive_ID = 0; //// 0: serverM
	M2C_message_send.operation = 0; //// 0: check wallet
	
	if (balance_user_transaction.number_transactions == 0)
	{
		//// user not found 
		M2C_message_send.status = 1; //// 1: user not found
		
	}
	else 
	{		
		M2C_message_send.status = 0; //// 0: user in the network
	}
	
	M2C_message_send.balance = balance_user_transaction.total_amount;
		
	// send to TCP 0421
	sendto(sockfd_TCP_child, &M2C_message_send, sizeof(M2C_message_send), 0, (const struct sockaddr *) &client_TCP_addr, sizeof(client_TCP_addr));
	
	//// !!! on screen !!!
	// After sending the current balance to the client j (j is either A or B):
		// “The main server sent the current balance to client <j>.”
	
	printf("The main server sent the current balance to client %s. \n", TCP_client_name[TCP_connect_idx][0]); 
	//// !!! on screen !!!
	
	
	// printf("serverM: send to client [ Check_wallet ]\n"); 

	// printf("ID = %d \n", M2C_message_send.decive_ID); 
	// printf("operation = %d \n", M2C_message_send.operation);
	// printf("status = %d \n", M2C_message_send.status);  
	// printf("balance = %d \n", M2C_message_send.balance);
	
	// printf("SIZE = %d \n", sizeof(M2C_message_send)); 
	// printf("The has sent data to client \n");
	
}

int serverM_TxCoins_search_func()
{
	// printf("===== serverM_TxCoins_search_func ===== \n");
	
	//// send to backend server 
	memset(&M_2_BA_message, 0, sizeof(M_2_BA_message));
	
	M_2_BA_message.decive_ID = 0;
	M_2_BA_message.operation = 1; //// 1: TxCoins
	strncpy(M_2_BA_message.user_1, C2M_message_recv.user_1, MAX_NAME_LENGTH);
	strncpy(M_2_BA_message.user_2, C2M_message_recv.user_2, MAX_NAME_LENGTH);		

	// printf("======================================================\n");
	// printf("UDP serverM send to backend \n"); 
	// printf("ID = %d \n", M_2_BA_message.decive_ID); 
	// printf("operation = %d \n", M_2_BA_message.operation);
	// printf("user_1 = %s \n", M_2_BA_message.user_1);	
	// printf("user_2 = %s \n", M_2_BA_message.user_2);
	// printf("SIZE = %d \n", sizeof(M_2_BA_message)); 
	// printf("======================================================\n");
	
	
	
		
	/////////// TxCoins func
	// int int UDP_TxCoins_func_0417(int* sender_amount_OUT, int* receiver_amount_OUT, int* max_serial_NO_OUT)
	int user_1_amount, user_2_amount, max_serial_NO;
	int TxCoins_status = UDP_TxCoins_func_0417(&user_1_amount, &user_2_amount, &max_serial_NO);
	
	//// send to client 
	Data_M2C M2C_message_send;
	memset(&M2C_message_send, 0, sizeof(M2C_message_send));
	
	M2C_message_send.decive_ID = 0;
	M2C_message_send.operation = 1;
	M2C_message_send.status = TxCoins_status;
	M2C_message_send.balance = 0;
	
	if (TxCoins_status == 2 )
	{
		//// compute new amount
		M2C_message_send.balance_1 = user_1_amount - C2M_message_recv.amount;
		M2C_message_send.balance_2 = user_2_amount + C2M_message_recv.amount;
		
	}
	else 
	{
		//// keep amount
		M2C_message_send.balance_1 = user_1_amount;
		M2C_message_send.balance_2 = user_2_amount;
	}
		
	// send to TCP 0421
	sendto(sockfd_TCP_child, &M2C_message_send, sizeof(M2C_message_send), 0, (const struct sockaddr *) &client_TCP_addr, sizeof(client_TCP_addr));
	
	//// !!! on screen !!!
	// After sending the result of transaction to the client j (j is either A or B):
		// “The main server sent the result of the transaction to client <j>.”
	
	printf("The main server sent the result of the transaction to client %s.\n", TCP_client_name[TCP_connect_idx][0]);  //// !!! on screen !!!
	//// !!! on screen !!!
	
	
	// printf("serverM: send to client [ TxCoins ]\n"); 

	// printf(" ID = %d \n", M2C_message_send.decive_ID); 
	// printf(" operation = %d \n", M2C_message_send.operation);
	// printf(" status = %d \n", M2C_message_send.status); 
	
	// printf(" balance_1 = %d \n", M2C_message_send.balance_1);
	// printf(" balance_2 = %d \n", M2C_message_send.balance_2);
	// printf(" balance = %d \n", M2C_message_send.balance);
	
	
	
	
	// printf(" SIZE = %d \n", sizeof(M2C_message_send)); 
	// printf("The has sent data to client \n");
	
	int return_int;
	if (TxCoins_status == 2)
	{
		return_int = max_serial_NO + 1;		
	}
	else 
	{
		return_int = 0;
		
	}
	return return_int;
	
}

void serverM_TxCoins_updata_func(int max_serial_NO_IN)
{
	// printf("===== serverM_TxCoins_updata_func ===== \n");
	
	//// send to backend server 
	memset(&M_2_BA_message, 0, sizeof(M_2_BA_message));
	
	M_2_BA_message.decive_ID = 0;
	M_2_BA_message.operation = 1; //// 1: TxCoins
	M_2_BA_message.status = max_serial_NO_IN; //// 1: TxCoins, other: max_serial_NO_IN
	M_2_BA_message.amount = C2M_message_recv.amount; 
	
	strncpy(M_2_BA_message.user_1, C2M_message_recv.user_1, MAX_NAME_LENGTH);		
	strncpy(M_2_BA_message.user_2, C2M_message_recv.user_2, MAX_NAME_LENGTH);
	
	// printf("======================================================\n");
	// printf("UDP serverM send to backend \n"); 
	// printf("ID = %d \n", M_2_BA_message.decive_ID); 
	// printf("operation = %d \n", M_2_BA_message.operation);
	// printf("status = %d \n", M_2_BA_message.status);
	
	
	// printf("user_1 = %s \n", M_2_BA_message.user_1);
	// printf("user_2 = %s \n", M_2_BA_message.user_2);
	// printf("amount = %d \n", M_2_BA_message.amount);
	
	// printf("SIZE = %d \n", sizeof(M_2_BA_message)); 
	// printf("======================================================\n");
	
	
	//// void UDP_Update_TxCoins_func_0417()
	UDP_Update_TxCoins_func_0417();
	
	
	
	
	
}


void serverM_TXLIS_func()
{
	// printf("===== serverM_TXLIS_func ===== \n");
	
	//// send to backend server 
	memset(&M_2_BA_message, 0, sizeof(M_2_BA_message));
	
	M_2_BA_message.decive_ID = 0;
	M_2_BA_message.operation = 2; //// 2: TXLIS	

	// printf("======================================================\n");
	// printf("UDP serverM send to backend \n"); 
	// printf("ID = %d \n", M_2_BA_message.decive_ID); 
	// printf("operation = %d \n", M_2_BA_message.operation);
	
	// printf("SIZE = %d \n", sizeof(M_2_BA_message)); 
	// printf("======================================================\n");
	
	//// send to backend server 
	// void UDP_TXLIS_func_0418()
	UDP_TXLIS_func_0418();
	
	
	
}

void serverM_Stats_func()
{
	//// send to backend server 
	// printf("===== serverM_TXLIS_func ===== \n");
	
	//// reset
	name_list_count = 0; 
	
	//// send to backend server 
	memset(&M_2_BA_message, 0, sizeof(M_2_BA_message));
	
	M_2_BA_message.decive_ID = 0;
	M_2_BA_message.operation = 3; //// 3: state
	strncpy(M_2_BA_message.user_1, C2M_message_recv.user_1, MAX_NAME_LENGTH);

	// printf("======================================================\n");
	// printf("UDP serverM send to backend \n"); 
	// printf("ID = %d \n", M_2_BA_message.decive_ID); 
	// printf("operation = %d \n", M_2_BA_message.operation);
	// printf("user_1 = %s \n", M_2_BA_message.user_1);
	
	// printf("SIZE = %d \n", sizeof(M_2_BA_message)); 
	// printf("======================================================\n");
	
	//// send to backend server 
	// // void UDP_Stats_func_0418_v2()
	UDP_Stats_func_0418_v2();
	
			
	
	
	// printf("======================================================\n");
	// printf("UDP serverM finish sort list \n");
	// printf("name_list_count[ %d ] \n", name_list_count);
	// for(int i=0; i<name_list_count; i++ )
	// {
		// printf("-----------------------------------------------------------------------------------------\n"); 
		// printf("name_list_record_sort[%d].user = %s \n", i, name_list_record_sort[i].user);
		// printf("name_list_record_sort[%d].number_transactions = %d \n", i, name_list_record_sort[i].number_transactions);
		// printf("name_list_record_sort[%d].send_amount = %d \n", i, name_list_record_sort[i].send_amount);
		// printf("name_list_record_sort[%d].receive_amount = %d \n", i, name_list_record_sort[i].receive_amount);
		// printf("name_list_record_sort[%d].total_amount = %d \n", i, name_list_record_sort[i].total_amount);
	// }	
	// printf("======================================================\n");
	
	 
	//// send to client 
	Data_M2C M2C_message_send;	
	M2C_message_send.decive_ID = 0; //// 0: serverM
	M2C_message_send.operation = 3; //// 3: state
	M2C_message_send.status = 7; //// 7: state
	M2C_message_send.payload_number = name_list_count; //// payload_number: name_list_count
	
	//// state_list[MAX_TRANS_DATA];
	for(int i=0; i<name_list_count; i++ )
	{
		M2C_message_send.state_list[i] = name_list_record_sort[i];
	}	
	
		
	// send to TCP 
	sendto(sockfd_TCP_child, &M2C_message_send, sizeof(M2C_message_send), 0, (const struct sockaddr *) &client_TCP_addr, sizeof(client_TCP_addr));
	
	// printf("======================================================\n");
	// printf("serverM: send to client [ state ]\n"); 

	// printf("ID = %d \n", M2C_message_send.decive_ID); 
	// printf("operation = %d \n", M2C_message_send.operation);
	// printf("payload_number = %d \n", M2C_message_send.payload_number);
	
	// printf("M2C_message_send.state_list[0].user = %s \n", M2C_message_send.state_list[0].user);
	
	// printf("SIZE = %d \n", sizeof(M2C_message_send)); 
	// printf("The has sent data to client \n");
	// printf("======================================================\n");
	
	memset(&name_list_record_sort, 0, sizeof(name_list_record_sort)); //  make sure the struct is empty
	memset(&name_list_record, 0, sizeof(name_list_record)); //  make sure the struct is empty
	
}

///////////////// UDP

///////////////////////////////////////////
void create_serverM_UDP_server_socket()
{
	// create socket 
	sockfd_serverM_Server_UDP = socket(AF_INET, SOCK_DGRAM, 0); // Create UDP server socket

	if (sockfd_serverM_Server_UDP == -1) 
	{
        perror("serverM [can not open UDP socket]");
        exit(1);
    }
	
	// setup scheduler IP and port
	memset(&ServerM_UDP_server_addr, 0, sizeof(ServerM_UDP_server_addr)); //  make sure the struct is empty
    ServerM_UDP_server_addr.sin_family = AF_INET; // Use IPv4 address family
    ServerM_UDP_server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    ServerM_UDP_server_addr.sin_port = htons(serverM_UDP_PORT); // serverM Server port number
	
	// bind sockaddr	
	if (bind(sockfd_serverM_Server_UDP, (struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr)) == -1)
	{
        perror("ServerM UDP Server [fail to bind socket]");
        exit(1);
    }	
}


void initial_connect_backend(int backend_server_number)
{
	// setup backend IP and port 
	//// switch by server number 
	memset(&Backend_Server_UDP_addr[backend_server_number], 0, sizeof(Backend_Server_UDP_addr[backend_server_number])); //  make sure the struct is empty
    Backend_Server_UDP_addr[backend_server_number].sin_family = AF_INET; // Use IPv4 address family
    Backend_Server_UDP_addr[backend_server_number].sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    Backend_Server_UDP_addr[backend_server_number].sin_port = htons(backed_server_port_arr[backend_server_number]); // backend port number
	

	// printf("TEST---backend_server_number: %d   \n", backend_server_number);
	
	// printf("TEST---backed_server_port_arr: %d   \n", backed_server_port_arr[0]);
	// printf("TEST---backed_server_port_arr htons: %d   \n", htons(backed_server_port_arr[backend_server_number]));
	
	// printf("TEST--- Backend_Server_UDP_addr: %d   \n", Backend_Server_UDP_addr[backend_server_number].sin_port);	
	// printf("setup backend IP and port \n");
	
}


void serverM_UDP_setup()
{	
	// printf("======================================================\n");
	// printf("The ServerM UDP is up and running.\n"); 
	
	//// UDP backend server 
	
	backed_server_port_arr[0] = serverA_UDP_PORT;
	backed_server_port_arr[1] = serverB_UDP_PORT;
	backed_server_port_arr[2] = serverC_UDP_PORT;
	
	// printf("TEST---backed_server_port_arr[0]: %d   \n", backed_server_port_arr[0]);
	// printf("TEST---backed_server_port_arr[1]: %d   \n", backed_server_port_arr[1]);
	// printf("TEST---backed_server_port_arr[2]: %d   \n", backed_server_port_arr[2]);
	
	
	Backend_Server_name[0][0] = "A";
	Backend_Server_name[1][0] = "B";
	Backend_Server_name[2][0] = "C";
	
	//// create UDP socket 
	create_serverM_UDP_server_socket();
	
	
	// printf("TEST--- # of Backend_Server: %d   \n", Backend_Server_number);	
	
	// connect to backend Server A B C
	for(int i=0; i < Backend_Server_number; i++) //// 3 test 1
	{	
		backend_A_addr_size[i]= sizeof(Backend_Server_UDP_addr[i]);
		initial_connect_backend(i); // 0: A, 1: B, 2: C
	}
	
	
	//// backend_startup
	int backend_startup_signal;
	for(int i=0; i < Backend_Server_number; i++) //// 3 test 1
	{
		recvfrom(sockfd_serverM_Server_UDP, &backend_startup_signal, sizeof(backend_startup_signal), 0, (struct sockaddr *) &Backend_Server_UDP_addr[i], &backend_A_addr_size[i]);

		
	} //// for(int i=0; i < 3; i++)
	// printf("======================= backend server up  \n");
	
	
}


void UDP_check_wallet_func_0417()
{
	
	//////// receive from backend 		
	Data_BackServer_2M_balance check_balence_msg_receive[Backend_Server_number];
	memset(&check_balence_msg_receive, 0, sizeof(check_balence_msg_receive));
	
	for(int i=0; i<Backend_Server_number; i++ )
	{
		if (sendto(sockfd_serverM_Server_UDP, &M_2_BA_message, sizeof(M_2_BA_message), 0, (const struct sockaddr *) &Backend_Server_UDP_addr[i], sizeof(Backend_Server_UDP_addr[i])) == -1) 
		{
			perror(" : fail to send input data to backend");
			exit(1);
		}
		
		//// !!! on screen !!!
		// The main server sent a request to server <i>.
		printf("The main server sent a request to server %s \n", Backend_Server_name[i][0]); //// !!! on screen !!!
		//// !!! on screen !!!
		
		
		recvfrom(sockfd_serverM_Server_UDP, &check_balence_msg_receive[i], sizeof(check_balence_msg_receive[i]), 0, (struct sockaddr *) &Backend_Server_UDP_addr[i], &backend_A_addr_size[i]);
		
		//// !!! on screen !!!
		// After receiving result from backend server i for checking balance ( i is one of A,B, or C):
		// “The main server received transactions from Server <i> using UDP over port <PORT_NUMBER>.”
		printf("The main server received transactions from Server %s using UDP over port %d.\n", 
													Backend_Server_name[i][0] , backed_server_port_arr[i]); //// !!! on screen !!!
		//// !!! on screen !!!
		
		
		// printf("-----------------------------------------------------------------------------------------\n"); 
		// printf("receive tans number \n");
		// printf("ID = %d \n", check_balence_msg_receive[i].decive_ID); 
		// printf("operation = %d \n", check_balence_msg_receive[i].operation);	
		// printf("user_1 = %s \n", check_balence_msg_receive[i].user_1);
		// printf("total_trans_number = %d \n", check_balence_msg_receive[i].total_trans_number);
		// printf("match_transaction[1].sender = %s \n", check_balence_msg_receive[i].match_transaction[1].sender);	
		// printf("SIZE = %d \n", sizeof(check_balence_msg_receive[i]));
		// printf("-----------------------------------------------------------------------------------------\n");
		
	} //// for(int i=0; i<Backend_Server_number; i++ )
	

	

	//// compute balance 
	//// compute balance
	
	memset(&balance_user_transaction, 0, sizeof(balance_user_transaction));
		
	// void compute_balance_0410_func(char *name_IN, int total_row_IN_A, Data_transaction *match_tran_A_IN
											// , int total_row_IN_B, Data_transaction *match_tran_B_IN
											// , int total_row_IN_C, Data_transaction *match_tran_C_IN
											// , Data_user_transaction *data_user_tran_OUT)
											
	compute_balance_0410_func(M_2_BA_message.user_1, check_balence_msg_receive[0].total_trans_number, check_balence_msg_receive[0].match_transaction
														, check_balence_msg_receive[1].total_trans_number, check_balence_msg_receive[1].match_transaction
														, check_balence_msg_receive[2].total_trans_number, check_balence_msg_receive[2].match_transaction
														, &balance_user_transaction);

	// printf("======================================================\n");
	// printf("compute complete \n");	
	
	// printf("user = %s:\n", balance_user_transaction.user);
	// printf("number_transactions = %d:\n", balance_user_transaction.number_transactions);
	// printf("send_amount = %d:\n", balance_user_transaction.send_amount);
	// printf("receive_amount = %d:\n", balance_user_transaction.receive_amount);
	// printf("total_amount = %d:\n", balance_user_transaction.total_amount);
	// printf("======================================================\n");
	
	
}

int UDP_TxCoins_func_0417(int* sender_amount_OUT, int* receiver_amount_OUT, int* max_serial_NO_OUT)
{
	
	// printf("======================================================\n");
	// printf("UDP_TxCoins_func_0417 \n");	
	
	//////// receive from backend 		
	Data_BackServer_2M_TxCoins TxCoins_msg_receive[Backend_Server_number];
	memset(&TxCoins_msg_receive, 0, sizeof(TxCoins_msg_receive));
	
	int sender_match_count = 0;
	int receiver_match_count = 0;
	int max_serial_NO = 0;
	
	
	
	int sender_amount = initial_amount;
	int receiver_amount = initial_amount;
	
	for(int i=0; i<Backend_Server_number; i++ )
	{
		if (sendto(sockfd_serverM_Server_UDP, &M_2_BA_message, sizeof(M_2_BA_message), 0, (const struct sockaddr *) &Backend_Server_UDP_addr[i], sizeof(Backend_Server_UDP_addr[i])) == -1) 
		{
			perror(" : fail to send input data to backend");
			exit(1);
		}
		
		//// !!! on screen !!!
		// After querying Backend-Server i for transferring coins ( i is one of A,B, or C):
		// “The main server sent a request to server <i>.”
		
		printf("The main server sent a request to server %s. \n", Backend_Server_name[i][0]); //// !!! on screen !!!
		//// !!! on screen !!!
		
		recvfrom(sockfd_serverM_Server_UDP, &TxCoins_msg_receive[i], sizeof(TxCoins_msg_receive[i]), 0, (struct sockaddr *) &Backend_Server_UDP_addr[i], &backend_A_addr_size[i]);
		
		
		//// !!! on screen !!!
		// After receiving result from backend server i for transferring coins (i is one of A,B, or C):
		// “The main server received the feedback from server <i> using UDP over port <PORT_NUMBER>.”
		
		printf("The main server received the feedback from server %s using UDP over port %d. \n", 
															Backend_Server_name[i][0], 	backed_server_port_arr[i]); //// !!! on screen !!!
		//// !!! on screen !!!
		
		
		
		
		sender_match_count += TxCoins_msg_receive[i].user_1_match_count;
		receiver_match_count += TxCoins_msg_receive[i].user_2_match_count;
		
		sender_amount += TxCoins_msg_receive[i].user_1_amount;
		receiver_amount += TxCoins_msg_receive[i].user_2_amount;
		
		max_serial_NO = max_serial_NO < TxCoins_msg_receive[i].max_tran_number ? TxCoins_msg_receive[i].max_tran_number:max_serial_NO;
		
		// printf("-----------------------------------------------------\n");
		// printf("receive TxCoins backend[%d] \n", i);
		
		// printf("ID = %d \n", TxCoins_msg_receive[i].decive_ID); 
		// printf("operation = %d \n", TxCoins_msg_receive[i].operation);	
		// printf("status = %d \n", TxCoins_msg_receive[i].status);
		
		// printf("user_1 = %s \n", TxCoins_msg_receive[i].user_1);
		// printf("user_1_amount = %d \n", TxCoins_msg_receive[i].user_1_amount);
		// printf("user_1_match_count = %d \n", TxCoins_msg_receive[i].user_1_match_count);
		
		
		// printf("user_2 = %s \n", TxCoins_msg_receive[i].user_2);
		// printf("user_2_amount = %d \n", TxCoins_msg_receive[i].user_2_amount);
		// printf("user_2_match_count = %d \n", TxCoins_msg_receive[i].user_2_match_count);
		
		
		// printf("max_tran_number = %d \n", TxCoins_msg_receive[i].max_tran_number);	
		
		// printf("SIZE = %d \n", sizeof(TxCoins_msg_receive[i]));		
		
		// printf("-----------------------------------------------------\n");
		
	} //// for(int i=0; i<Backend_Server_number; i++ )
	

	
	/////// check result
	// printf("sender_match_count = %d \n", sender_match_count);	
	// printf("receiver_match_count = %d \n", receiver_match_count);
	
	// printf("sender_amount = %d \n", sender_amount);	
	
	int TxCoins_status = -1;
	if(sender_match_count == 0 && receiver_match_count == 0)
	{
		//// both sender and receiver are not in the network
		TxCoins_status = 6;		
	}
	else if (sender_match_count == 0)
	{
		//// sender is not in the network
		TxCoins_status = 4;
	}
	else if (receiver_match_count == 0)
	{
		//// receiver is not in the network
		TxCoins_status = 5;
	}
	else 
	{
		//// both sender and receiver are in the network
		
		//// check balance
		if(sender_amount >= C2M_message_recv.amount ) ////!!!!!!!!!!! C2M_message_recv.amount
		{
			TxCoins_status = 2;
		}
		else
		{
			//// sender balance lower than trans amount
			TxCoins_status = 3;
		}
		
		
	}
	// printf("TxCoins_status = %d \n", TxCoins_status);
	// printf("sender_amount = %d \n", sender_amount);
	// printf("receiver_amount = %d \n", receiver_amount);
	// printf("max_serial_NO = %d \n", max_serial_NO);
	
	
	// printf("END UDP_TxCoins_func_0417 \n");
	// printf("======================================================\n");
	
	*sender_amount_OUT = sender_amount;
	*receiver_amount_OUT = receiver_amount;
	*max_serial_NO_OUT = max_serial_NO;
	
	return TxCoins_status;	
}

void UDP_Update_TxCoins_func_0417()
{
	/////// send to random backend server 
	time_t rand_t_seed;
	srand((unsigned) time(&rand_t_seed));
	
	int random_backend = rand() % Backend_Server_number;
	// printf("random_backend = %d \n", random_backend);
	if (sendto(sockfd_serverM_Server_UDP, &M_2_BA_message, sizeof(M_2_BA_message), 0, (const struct sockaddr *) &Backend_Server_UDP_addr[random_backend], sizeof(Backend_Server_UDP_addr[random_backend])) == -1) 
	{
		perror(" : fail to update to backend");
		exit(1);
	}
	
	//// receive comfirmatin signal 
	int backend_comfirmatin_signal;	
	if (recvfrom(sockfd_serverM_Server_UDP, &backend_comfirmatin_signal, sizeof(backend_comfirmatin_signal), 0, (struct sockaddr *) &Backend_Server_UDP_addr[random_backend], &backend_A_addr_size[random_backend]) == -1) 
	{
		perror(" : fail to receive comfirmatin signal");
		exit(1);
	}
	// else
	// {
		// printf("Complete update \n");
	// }
	// printf("======================================================\n");
}

void UDP_TXLIS_func_0418()
{
	// printf("======================================================\n");
	// printf("UDP_TXLIS_func_0418 \n");	
	
	/////// receive from backend 		
	Data_BackServer_2M_balance TXLIS_msg_receive[Backend_Server_number];
	memset(&TXLIS_msg_receive, 0, sizeof(TXLIS_msg_receive));
	
	int max_serial_NO = 0;
	
	for(int i=0; i<Backend_Server_number; i++ )
	{
		if (sendto(sockfd_serverM_Server_UDP, &M_2_BA_message, sizeof(M_2_BA_message), 0, (const struct sockaddr *) &Backend_Server_UDP_addr[i], sizeof(Backend_Server_UDP_addr[i])) == -1) 
		{
			perror(" : fail to send input data to backend");
			exit(1);
		}
		recvfrom(sockfd_serverM_Server_UDP, &TXLIS_msg_receive[i], sizeof(TXLIS_msg_receive[i]), 0, (struct sockaddr *) &Backend_Server_UDP_addr[i], &backend_A_addr_size[i]);
		
		
		max_serial_NO += TXLIS_msg_receive[i].total_trans_number;
		
		
		// printf("-----------------------------------------------------------------------------------------\n"); 
		// printf("receive tans number \n");
		// printf("ID = %d \n", TXLIS_msg_receive[i].decive_ID); 
		// printf("operation = %d \n", TXLIS_msg_receive[i].operation);
		
		// printf("total_trans_number = %d \n", TXLIS_msg_receive[i].total_trans_number);
		// printf("match_transaction[1].sender = %s \n", TXLIS_msg_receive[i].match_transaction[1].sender);
		
		// printf("SIZE = %d \n", sizeof(TXLIS_msg_receive[i]));
		// printf("-----------------------------------------------------------------------------------------\n");
		
	} //// for(int i=0; i<Backend_Server_number; i++ )
		
	//// create TXLIS
	// printf("max_serial_NO = %d \n", max_serial_NO);
	
	// printf("Backend_Server_number = %d \n", Backend_Server_number);
	
	int list_count = 0;
	int test_times_count = 0;
	while (list_count < max_serial_NO)
	{			
		for(int i=0; i<Backend_Server_number; i++ )
		{ //// search each backend list
			// printf("TXLIS_msg_receive[i].total_trans_number = %d \n", TXLIS_msg_receive[i].total_trans_number);
			// printf("------check i = %d ------ \n", i);
			for(int j=0; j<TXLIS_msg_receive[i].total_trans_number; j++ )
			{ //// find match list
				// printf("list_count +1 = %d \n", list_count +1);
				// printf("TXLIS_msg_receive[i].match_transaction[j].number= %d \n", TXLIS_msg_receive[i].match_transaction[j].number);
				if(TXLIS_msg_receive[i].match_transaction[j].number == list_count +1)
				{
					// printf("match  list_count= %d \n", list_count);
					TXLIS_list[list_count] = TXLIS_msg_receive[i].match_transaction[j];
					list_count++;
				}
			}
		}
		test_times_count++;
	} //// while (list_count < max_serial_NO)
		
	// printf("-----------------------------------------------------------------------------------------\n"); 
	// printf("Finish  TXLIS_list\n");
	// printf("test_times_count = %d \n", test_times_count); 
	
	// for(int i=0; i<list_count; i++ )
	// {
		// printf("-----------------------------------------------------------------------------------------\n");
		// printf("TXLIS_list[%d].number = %d \n", i , TXLIS_list[i].number);
		// printf("match_transaction[%d].sender = %s \n", i , TXLIS_list[i].sender);
		// printf("match_transaction[%d].receiver = %s \n", i , TXLIS_list[i].receiver);		
		// printf("match_transaction[%d].trans_amount = %d \n", i , TXLIS_list[i].trans_amount);
	// }	
	// printf("-----------------------------------------------------------------------------------------\n");
	
	//// reference https://www.programiz.com/c-programming/c-file-input-output
	// printf("creat alichain.txt \n" );
	FILE * fileptr;
	
	fileptr = fopen("alichain.txt", "w");
	if(fileptr == NULL)
	{
		printf("ERROR: alichain.txt \n");
		exit(1);
	}
	
	
	
	for(int i=0; i<list_count; i++ )
	{
		// printf("------------------------------------------------------------\n");
		char line[4*2+MAX_NAME_LENGTH*2];
		memset(&line, 0, sizeof(line));
		
		char number_char[10];
		sprintf(number_char,"%d",TXLIS_list[i].number); //// int to char 		
		strcat(line, number_char); 
		// printf("number_char \n");
		
		strcat(line, " "); 
		strcat(line, TXLIS_list[i].sender);
		strcat(line, " ");
		strcat(line, TXLIS_list[i].receiver);
		strcat(line, " ");
		
		char trans_amount_char[10];
		sprintf(trans_amount_char,"%d",TXLIS_list[i].trans_amount); //// int to char 		
		strcat(line, trans_amount_char);
		// printf("trans_amount_char \n");
		
		strcat(line, "\n");
		
		// printf("line= %s " , line);
		fprintf(fileptr,"%s",line);
   
		
	}
	
	fclose(fileptr);
	// printf("alichain.txt complete \n");
	
	
	
}

void name_list_func_0418_v2(char *user_name_IN, Data_transaction *data_tran_IN, int total_row_IN)
{
	// printf("======================================================\n");
	// printf("name_list_func_0418_v2 \n");	
	
		
	for(int i=0; i< total_row_IN; i++)
	{
		int sender_name_match = 0;
		int receiver_name_match = 0;
		// printf("name_list_count = %d \n",name_list_count);
		
		if ( (strcmp(data_tran_IN[i].sender, user_name_IN) == 0) ) //// sender is the user 
		{
			//// check receiver 
			for(int j=0; j <= name_list_count; j++)
			{
				
				if (strcmp(data_tran_IN[i].receiver, name_list_record[j].user) == 0)
				{
					receiver_name_match = 1;
					name_list_record[j].send_amount += data_tran_IN[i].trans_amount; //// user send to receive 
					name_list_record[j].total_amount -= data_tran_IN[i].trans_amount;
					name_list_record[j].number_transactions++;
				}
				

			}
			
			if (receiver_name_match == 0)
			{
				strncpy(name_list_record[name_list_count].user, data_tran_IN[i].receiver, MAX_NAME_LENGTH);
				// printf("name_list receiver = %s \n", name_list_record[name_list_count].user);
				name_list_record[name_list_count].send_amount = data_tran_IN[i].trans_amount;
				name_list_record[name_list_count].total_amount = (-1)*data_tran_IN[i].trans_amount;
				name_list_record[name_list_count].number_transactions = 1;
				name_list_count++;
			}
				
		} //// if ( (strcmp(data_tran_IN[i].sender, user_name_IN) == 0) ) //// sender is the user 
		
		
		if ( (strcmp(data_tran_IN[i].receiver, user_name_IN) == 0) ) //// receiver is the user
		{
			//// check sender 
			for(int j=0; j <= name_list_count; j++)
			{
				
				if (strcmp(data_tran_IN[i].sender, name_list_record[j].user) == 0)
				{
					sender_name_match = 1;
					name_list_record[j].receive_amount += data_tran_IN[i].trans_amount; //// sender send to user
					name_list_record[j].total_amount += data_tran_IN[i].trans_amount;
					name_list_record[j].number_transactions++;
				}
				
				

			}
			
			if (sender_name_match == 0)
			{
				strncpy(name_list_record[name_list_count].user, data_tran_IN[i].sender, MAX_NAME_LENGTH);
				// printf("name_list sender = %s \n", name_list_record[name_list_count].user);
				name_list_record[name_list_count].receive_amount = data_tran_IN[i].trans_amount;
				name_list_record[name_list_count].total_amount = data_tran_IN[i].trans_amount;
				name_list_record[name_list_count].number_transactions = 1;
				name_list_count++;
			}
			
		} //// if ( (strcmp(data_tran_IN[i].receiver, user_name_IN) == 0) ) //// receiver is the user
		
		
	}//// for(int i=0; i< total_row_IN; i++)
	
	//////// !!!!! test
	
	// printf("name_list_count = %d \n",name_list_count);
	// for(int i=0; i< name_list_count; i++)
	// {	
		// printf("-----------------------------------------------------------------------------------------\n"); 
		// printf("name_list[%d].user = %s \n", i, name_list_record[i].user);
		// printf("name_list[%d].number_transactions = %d \n", i, name_list_record[i].number_transactions);
		// printf("name_list[%d].send_amount = %d \n", i, name_list_record[i].send_amount);
		// printf("name_list[%d].receive_amount = %d \n", i, name_list_record[i].receive_amount);
		// printf("name_list[%d].total_amount = %d \n", i, name_list_record[i].total_amount);
	// }
	// printf("-----------------------------------------------------------------------------------------\n"); 

	
	
	
	
	// printf("END name_list_func_0418_v2 \n");	
	// printf("======================================================\n");
}



void UDP_Stats_func_0418_v2()
{
	// printf("======================================================\n");
	// printf("UDP_Stats_func_0418_v2 \n");	
	
	
	
	//////// receive from backend 		
	Data_BackServer_2M_balance Stats_TXLIS_msg_receive[Backend_Server_number];
	memset(&Stats_TXLIS_msg_receive, 0, sizeof(Stats_TXLIS_msg_receive));
	
	int max_serial_NO = 0;
	
	for(int i=0; i<Backend_Server_number; i++ )
	{
		if (sendto(sockfd_serverM_Server_UDP, &M_2_BA_message, sizeof(M_2_BA_message), 0, (const struct sockaddr *) &Backend_Server_UDP_addr[i], sizeof(Backend_Server_UDP_addr[i])) == -1) 
		{
			perror(" : fail to send input data to backend");
			exit(1);
		}
		recvfrom(sockfd_serverM_Server_UDP, &Stats_TXLIS_msg_receive[i], sizeof(Stats_TXLIS_msg_receive[i]), 0, (struct sockaddr *) &Backend_Server_UDP_addr[i], &backend_A_addr_size[i]);
		
		max_serial_NO += Stats_TXLIS_msg_receive[i].total_trans_number;
		
		// printf("-----------------------------------------------------------------------------------------\n"); 
		// printf("receive tans number \n");
		// printf("ID = %d \n", Stats_TXLIS_msg_receive[i].decive_ID); 
		// printf("operation = %d \n", Stats_TXLIS_msg_receive[i].operation);	
		// printf("user_1 = %s \n", Stats_TXLIS_msg_receive[i].user_1);
		// printf("total_trans_number = %d \n", Stats_TXLIS_msg_receive[i].total_trans_number);
		// printf("match_transaction[1].sender = %s \n", Stats_TXLIS_msg_receive[i].match_transaction[1].sender);	
		// printf("SIZE = %d \n", sizeof(Stats_TXLIS_msg_receive[i]));
		// printf("-----------------------------------------------------------------------------------------\n");
		
	} //// for(int i=0; i<Backend_Server_number; i++ )
		
	
	//// compute name list 
	for(int i=0; i<Backend_Server_number; i++ )
	{
		// void name_list_func_0418_v2(char *user_name_IN, Data_transaction *data_tran_IN, int total_row_IN)
		name_list_func_0418_v2(M_2_BA_message.user_1 ,Stats_TXLIS_msg_receive[i].match_transaction, Stats_TXLIS_msg_receive[i].total_trans_number) ; //// output name_list_record,  name_list_count
	}
	
	
	// printf("-----------------------------------------------------------------------------------------\n"); 
	// printf("name_list_count = %d \n", name_list_count); 
	// for(int i=0; i<name_list_count; i++ )
	// {
		// printf("name_list[%d].user = %s \n", i, name_list_record[i].user);
		
	// }	
	// printf("-----------------------------------------------------------------------------------------\n"); 
	
	//// sort big to small
	//// name_list_record_sort[MAX_TRANS_DATA];
	
		
	int remain_number = name_list_count;
	int sort_idx = 0;
	while (remain_number > 0)
	{
		int max_trans = 0;
		
		//// find max trans
		for(int i=0; i<name_list_count; i++ )
		{
			if (name_list_record[i].number_transactions > max_trans)
			{
				max_trans = name_list_record[i].number_transactions;
			}
		
		}
		
		//// save max trans data
		for(int i=0; i<name_list_count; i++ )
		{
			if (name_list_record[i].number_transactions == max_trans)
			{
				name_list_record_sort[sort_idx] = name_list_record[i];
				sort_idx++;
				remain_number--;
				
				//// reset max data 
				name_list_record[i].number_transactions = -1;
				
			}
		
		}
		
	} //// while (remain_number > 0)
	
	
	// printf("finish sort \n"); 
	// for(int i=0; i<name_list_count; i++ )
	// {
		// printf("-----------------------------------------------------------------------------------------\n"); 
		// printf("name_list_record_sort[%d].user = %s \n", i, name_list_record_sort[i].user);
		// printf("name_list_record_sort[%d].number_transactions = %d \n", i, name_list_record_sort[i].number_transactions);
		// printf("name_list_record_sort[%d].send_amount = %d \n", i, name_list_record_sort[i].send_amount);
		// printf("name_list_record_sort[%d].receive_amount = %d \n", i, name_list_record_sort[i].receive_amount);
		// printf("name_list_record_sort[%d].total_amount = %d \n", i, name_list_record_sort[i].total_amount);
	// }	
	// printf("-----------------------------------------------------------------------------------------\n"); 
	
	
	
	// printf("END UDP_Stats_func_0418_v2 \n");		
	// printf("======================================================\n");

	
} 


int main(int argc, char *argv[])
{
	
	//// UDP
	// void serverM_UDP_setup()
	serverM_UDP_setup();
	
	
	client_port_arr[0] = ServerM_TCP_PORT_4CA;
	client_port_arr[1] = ServerM_TCP_PORT_4CB;
	
	TCP_client_name[0][0] = "A";
	TCP_client_name[1][0] = "B";
	
	
	
	//// !!! on screen !!!
	printf("The main server is up and running.\n"); //// !!! on screen !!!
	//// !!! on screen !!!
	
	//// TCP
	//// create TCP socket
	// void create_ServerM_TCP_server_socket_0415()
	create_ServerM_TCP_server_socket_0415(0); //// 0:A 
	// printf("The ServerM TCP for A.\n"); 
	create_ServerM_TCP_server_socket_0415(1); //// 1:B 
	// printf("The ServerM TCP for B.\n"); 
	
	int client_TCP_addr_size = sizeof(client_TCP_addr); 
	
	
	//// repeat 
	while(1)
	{
		
		// printf("||||||||||||||||||||||||||||||||||||||||||||\n");			
		// printf("waiting TCP connection \n");		
		
		//// select TCP connect 
		TCP_connect_idx = -1;
		fd_set select_set;
		int max_fd;
		
		//// clear reset 
		FD_ZERO(&select_set);
		
		if (sockfd_ServerM_Server_TCP[0]>sockfd_ServerM_Server_TCP[1])
		{
			max_fd = sockfd_ServerM_Server_TCP[0] +1;			
		}
		else
		{
			max_fd = sockfd_ServerM_Server_TCP[1] +1;		
		}
		
		////
		FD_SET(sockfd_ServerM_Server_TCP[0], &select_set);
		FD_SET(sockfd_ServerM_Server_TCP[1], &select_set);
		
		//// select 
		select(max_fd, &select_set, NULL, NULL, NULL);
		
		if(FD_ISSET(sockfd_ServerM_Server_TCP[0], &select_set)) //// client A
		{
			sockfd_ServerM_Server_TCP_Accept = sockfd_ServerM_Server_TCP[0];
			TCP_connect_idx = 0;
			
			sockfd_TCP_child = accept(sockfd_ServerM_Server_TCP_Accept,(struct sockaddr*) &client_TCP_addr, &client_TCP_addr_size); //// create child socket
			// printf("A ==== TCP connected \n");
			
		}
		else //if(FD_ISSET(sockfd_ServerM_Server_TCP[1], &select_set)) //// client B
		{
			sockfd_ServerM_Server_TCP_Accept = sockfd_ServerM_Server_TCP[1];
			TCP_connect_idx = 1;
			
			sockfd_TCP_child = accept(sockfd_ServerM_Server_TCP_Accept,(struct sockaddr*) &client_TCP_addr, &client_TCP_addr_size); //// create child socket
			// printf("B ==== TCP connected \n");
			
		}
		
				
		
		
		//// received
		memset(&C2M_message_recv, 0, sizeof(C2M_message_recv));
	
		recvfrom(sockfd_TCP_child, &C2M_message_recv, sizeof(C2M_message_recv), 0, (struct sockaddr *) &client_TCP_addr,&client_TCP_addr_size);
		
		// printf("TEST --- TCP_recv \n"); 
		// printf(" ID = %d \n", C2M_message_recv.decive_ID); 
		// printf(" operation = %d \n", C2M_message_recv.operation);
		// printf(" user_1 = %s \n", C2M_message_recv.user_1);
		// printf(" user_2 = %s \n", C2M_message_recv.user_2);	
		// printf(" amount = %d \n", C2M_message_recv.amount);  	
		// // printf(" payload = %d \n", C2M_message_recv.payload);
		// printf(" SIZE = %d \n", sizeof(C2M_message_recv)); 
		
				
		switch(C2M_message_recv.operation)
		{
			case 0: //// Check wallet			
				//// !!! on screen !!!
				printf("The main server received input=\"%s\" from the client using TCP over port %d. \n",
														C2M_message_recv.user_1, client_port_arr[TCP_connect_idx]); //// !!! on screen !!!
				//// !!! on screen !!!
				serverM_Check_wallet_func();
			
			break;
			
			case 1:; //// TxCoins
				//// !!! on screen !!!
				// “The main server received from <SENDER_USERNAME> to transfer <TRANSFER_AMOUNT> coins to <RECEIVER_USERNAME> using TCP over port <port number>.”
				printf("The main server received from \"%s\" to transfer %d coins to \"%s\" using TCP over port %d. \n",
													C2M_message_recv.user_1, 		C2M_message_recv.amount, 	C2M_message_recv.user_2, 				client_port_arr[TCP_connect_idx]);
				//// !!! on screen !!!
					
				int trans_status = serverM_TxCoins_search_func();
				
				if (trans_status != 0 )
				{
					// printf(" trans success, send updata to backend \n"); 
					// printf(" trans_status = %d \n", trans_status); 
					
					serverM_TxCoins_updata_func(trans_status);
				}				
			
			break;
			
			case 2: //// TXLIST
				serverM_TXLIS_func();
			
			break;
			
			case 3: //// Stats
				serverM_Stats_func();
			
			break;
			
			default:
				printf("TEST --- operation ERROR \n"); 
				
					
		}
			
		
		
		
	} //// while(1)
	

	
	
	/* 	
		Table 7. Main Server on screen messages
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	O	Booting Up (only while starting):
		“The main server is up and running.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------			
	O	Upon Receiving the username from the clients for checking balance:
		“The main server received input=<USERNAME> from the client using TCP over port <port number>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	Upon Receiving the username from the clients for transferring coins:
		“The main server received from <SENDER_USERNAME> to transfer <TRANSFER_AMOUNT> coins to <RECEIVER_USERNAME> using TCP over port <port number>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	After querying Backend-Server i for checking balance ( i is one of A,B, or C): 
		“The main server sent a request to server <i>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	After receiving result from backend server i for checking balance ( i is one of A,B, or C):
		“The main server received transactions from Server <i> using UDP over port <PORT_NUMBER>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	After querying Backend-Server i for transferring coins ( i is one of A,B, or C):
		“The main server sent a request to server <i>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	After receiving result from backend server i for transferring coins (i is one of A,B, or C):
		“The main server received the feedback from server <i> using UDP over port <PORT_NUMBER>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	After sending the current balance to the client j (j is either A or B):
		“The main server sent the current balance to client <j>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	O	After sending the result of transaction to the client j (j is either A or B):
		“The main server sent the result of the transaction to client <j>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
		
		
		
		
		
	
	
	
	
	
	*/

}


