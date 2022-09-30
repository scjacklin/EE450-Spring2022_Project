/*

USC EE450
Final project

*/

/*
serverM(UDP server)


Sheng Chieh Lin 
USC ID: 2314107438


*/

////---- Log ----////
// 2022-0324 // test UDP connection
// 2022-0329 // header
// 2022-0329 // search

///////////////////////////
#include "header.h"


//////////
int backend_server_number = 0; // 0: A, 1:B, 2:C 
// char *backend_server_name[3][10];

int port_arr[3]; //// save port



int total_trans_row;

Data_M2BackServer M_2_BA_message_recv;

Data_transaction data_tran[MAX_TRANS_DATA];
// Data_transaction match_transaction[MAX_TRANS_DATA];


//// UDP 
struct sockaddr_in ServerM_UDP_server_addr; // UDP server address
struct sockaddr_in backend_addr; // UDP server address

int sockfd_backend_Server_UDP; //  Server datagram socket




void create_backend_UDP_server_socket()
{
	// backend_addr
	memset(&backend_addr, 0, sizeof(backend_addr));
	backend_addr.sin_family = AF_INET;
	backend_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
	backend_addr.sin_port = htons(port_arr[backend_server_number]);	
	
	// serverM 
	//ServerM_UDP_server_addr
	memset(&ServerM_UDP_server_addr, 0, sizeof(ServerM_UDP_server_addr));
    ServerM_UDP_server_addr.sin_family = AF_INET;
    ServerM_UDP_server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    ServerM_UDP_server_addr.sin_port = htons(serverM_UDP_PORT);
	
	
	////---- UDP to Scheduler Server  ----////
	sockfd_backend_Server_UDP = socket(AF_INET, SOCK_DGRAM, 0); // UDP datagram socket
    if (sockfd_backend_Server_UDP == -1) 
	{
        perror("backend Server [fail to create UDP socket]"); 
        exit(1);
    }
	
	//// bind
	if (bind(sockfd_backend_Server_UDP, (struct sockaddr *) &backend_addr, sizeof(backend_addr)) == -1)
	{
		perror("backend Server [fail to bind socket]");
		exit(1);
	}
	
	
	
}


void check_balance_func_0416()
{
	// printf("======================================================\n");
	// printf("check_balance_func_0416 \n");
	
	
	// Data_user_transaction user_transaction;
	// memset(&user_transaction, 0, sizeof(user_transaction));
	
	//// send tans to serverM
	Data_BackServer_2M_balance re_check_balence_msg;
	memset(&re_check_balence_msg, 0, sizeof(re_check_balence_msg));

	// int search_transactions_0410_func(char *name_IN, int total_row_IN, Data_transaction *data_tran_IN, Data_transaction *match_tran_OUT)

	int search_match_number = search_transactions_0410_func(M_2_BA_message_recv.user_1, total_trans_row, data_tran, re_check_balence_msg.match_transaction);
		
	// printf("-----------------------------------------------------------------------------------------\n");
	// printf("search complete \n");
	// printf("search_match_number: %d \n", search_match_number);
	
	re_check_balence_msg.decive_ID = 1 + backend_server_number; //// 
	re_check_balence_msg.operation = M_2_BA_message_recv.operation;		
	strncpy(re_check_balence_msg.user_1, M_2_BA_message_recv.user_1, MAX_NAME_LENGTH);
	re_check_balence_msg.total_trans_number = search_match_number;
	
	// printf("-----------------------------------------------------------------------------------------\n"); 
	// printf("send tans number to serverM \n");
	// printf("ID = %d \n", re_check_balence_msg.decive_ID); 
	// printf("operation = %d \n", re_check_balence_msg.operation);	
	// printf("user_1 = %s \n", re_check_balence_msg.user_1);
	// printf("total_trans_number = %d \n", re_check_balence_msg.total_trans_number);
	// printf("match_transaction[1].sender = %s \n", re_check_balence_msg.match_transaction[1].sender);


	
	// printf("SIZE = %d \n", sizeof(re_check_balence_msg));
	// printf("-----------------------------------------------------------------------------------------\n"); 
	
	if (sendto(sockfd_backend_Server_UDP, &re_check_balence_msg, sizeof(re_check_balence_msg), 0, (const struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr)) == -1) 
	{
		perror("fail to send match trans number to serverM");
		exit(1);
	}

	// printf("Finish check_balance_func_0416 \n");
	// printf("======================================================\n");
	
	
	
	
}

int Backend_search_transactions_4TxCoins_0417_func(char *name_IN_0, char *name_IN_1, int total_row_IN, Data_transaction *data_tran_IN, Data_Backend_SearchTxCoin_trans *search_TxCoins_OUT)
{
	// Data_Backend_SearchTxCoin_trans *search_TxCoins_OUT
		// char sender[MAX_NAME_LENGTH];
		// int sender_amount;

		// char receiver[MAX_NAME_LENGTH];
		// int receiver_amount;
		// int max_trans_number;
	

		
	// printf("======================================================\n");
	// printf("Backend_search_transactions_4TxCoins_0417_func \n");
	// printf("total_row_IN = %d:\n", total_row_IN);
	// printf("serch name_IN_0 = %s:\n", name_IN_0);
	// printf("serch name_IN_1 = %s:\n", name_IN_1);
	
	int search_status_OUT = -1;
	
	int sender_match =0;	
	int sender_send_amount =0;		
	int sender_receive_amount =0;
	
	int receiver_match =0;	
	int receiver_send_amount =0;		
	int receiver_receive_amount =0;
	
	
	int max_trans_number = 0;
	
	strncpy(search_TxCoins_OUT[0].sender,  name_IN_0, MAX_NAME_LENGTH);
	strncpy(search_TxCoins_OUT[0].receiver, name_IN_1, MAX_NAME_LENGTH);
	
		
	for(int i=0; i< total_row_IN; i++)
	{
		if (max_trans_number < data_tran_IN[i].number)
		{
			max_trans_number = data_tran_IN[i].number;
		}
	
		if (strcmp(name_IN_0 , data_tran_IN[i].sender) == 0) //// sender match sender 
		{
			// printf("sender match sender  \n");
			sender_send_amount += data_tran_IN[i].trans_amount;		
			sender_match++;
		}
		if (strcmp(name_IN_0 , data_tran_IN[i].receiver) == 0) //// sender match receiver 
		{
			// printf("sender match receiver  \n");
			sender_receive_amount += data_tran_IN[i].trans_amount;
			sender_match++;
		}
		
		if (strcmp(name_IN_1 , data_tran_IN[i].sender) == 0) //// receiver match sender 
		{
			// printf("receiver match sender   \n");
			receiver_send_amount += data_tran_IN[i].trans_amount;
			receiver_match++;
		}
		if (strcmp(name_IN_1 , data_tran_IN[i].receiver) == 0) //// receiver match receiver 
		{
			// printf("receiver match receiver   \n");
			receiver_receive_amount += data_tran_IN[i].trans_amount;
			receiver_match++;
		}
		
		
	}//// for(int i=0; i< total_row_IN; i++)
		
	search_TxCoins_OUT[0].sender_amount = sender_receive_amount - sender_send_amount;
	search_TxCoins_OUT[0].receiver_amount = receiver_receive_amount - receiver_send_amount;
	search_TxCoins_OUT[0].max_trans_number = max_trans_number;
	
	search_TxCoins_OUT[0].sender_match_count = sender_match;
	search_TxCoins_OUT[0].receiver_match_count = receiver_match;
	
	
	
	if(sender_match == 0 && receiver_match == 0)
	{
		search_status_OUT = -2;
	}
	else if(sender_match == 0 )
	{
		search_status_OUT = -1;
	}
	else if(receiver_match == 0 )
	{
		search_status_OUT = 0;
	}
	else
	{
		search_status_OUT = 1;
	}
	
	// printf("RETURN 2 main \n");
	// printf("=================================================\n");

	return search_status_OUT;
}

void check_TxCoins_func_0417()
{
	// printf("======================================================\n");
	// printf("check_TxCoins_func_0417 \n");
	
	Data_Backend_SearchTxCoin_trans search_TxCoins;
	
	// int Backend_search_transactions_4TxCoins_0417_func(char *name_IN_0, char *name_IN_1, int total_row_IN, Data_transaction *data_tran_IN, Data_Backend_SearchTxCoin_trans *search_TxCoins_OUT)
	int search_transactions_4TxCoins_status = Backend_search_transactions_4TxCoins_0417_func(M_2_BA_message_recv.user_1, M_2_BA_message_recv.user_2, total_trans_row, data_tran, &search_TxCoins);
	
	// printf("-----------------------------------------------------------------------------------------\n");
	// printf("TxCoins search complete \n");
	// printf("search_transactions_4TxCoins_status: %d \n", search_transactions_4TxCoins_status);
	
	Data_BackServer_2M_TxCoins re_TxCoins_msg;
	
	re_TxCoins_msg.decive_ID = 1 + backend_server_number; //// 
	re_TxCoins_msg.operation = M_2_BA_message_recv.operation;
	re_TxCoins_msg.status = search_transactions_4TxCoins_status;
	
	strncpy(re_TxCoins_msg.user_1, M_2_BA_message_recv.user_1, MAX_NAME_LENGTH);
	strncpy(re_TxCoins_msg.user_2, M_2_BA_message_recv.user_2, MAX_NAME_LENGTH);
	
	re_TxCoins_msg.user_1_amount = search_TxCoins.sender_amount;
	re_TxCoins_msg.user_2_amount = search_TxCoins.receiver_amount;
	
	re_TxCoins_msg.user_1_match_count = search_TxCoins.sender_match_count;
	re_TxCoins_msg.user_2_match_count = search_TxCoins.receiver_match_count;
	
	re_TxCoins_msg.max_tran_number = search_TxCoins.max_trans_number;
	
	// printf("send TxCoins search to serverM \n");
	// printf("ID = %d \n", re_TxCoins_msg.decive_ID); 
	// printf("operation = %d \n", re_TxCoins_msg.operation);
	// printf("status = %d \n", re_TxCoins_msg.status);
	
	// printf("user_1 = %s \n", re_TxCoins_msg.user_1);
	// printf("user_1_amount = %d \n", re_TxCoins_msg.user_1_amount);
	// printf("user_1_match_count = %d \n", re_TxCoins_msg.user_1_match_count);
	
	
	// printf("user_2 = %s \n", re_TxCoins_msg.user_2);
	// printf("user_2_amount = %d \n", re_TxCoins_msg.user_2_amount);
	// printf("user_2_match_count = %d \n", re_TxCoins_msg.user_2_match_count);
	
	
	// printf("max_tran_number = %d \n", re_TxCoins_msg.max_tran_number);	
	
	// printf("SIZE = %d \n", sizeof(re_TxCoins_msg));
	// printf("-----------------------------------------------------------------------------------------\n"); 
	
	
	
	if (sendto(sockfd_backend_Server_UDP, &re_TxCoins_msg, sizeof(re_TxCoins_msg), 0, (const struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr)) == -1) 
	{
		perror("fail to send TxCoins search to serverM");
		exit(1);
	}

	// printf("Finish check_TxCoins_func_0417 \n");
	// printf("======================================================\n");
	
}


void backend_TXLIS_func_0418()
{
	// printf("======================================================\n");
	// printf("backend_TXLIS_func_0418 \n");
	
	
	// Data_user_transaction user_transaction;
	// memset(&user_transaction, 0, sizeof(user_transaction));
	
	//// send tans to serverM
	Data_BackServer_2M_balance re_TXLIS_msg;
	memset(&re_TXLIS_msg, 0, sizeof(re_TXLIS_msg));
	
	
	re_TXLIS_msg.decive_ID = 1 + backend_server_number; //// 
	re_TXLIS_msg.operation = M_2_BA_message_recv.operation;		
	
	re_TXLIS_msg.total_trans_number = total_trans_row;
	// re_TXLIS_msg.match_transaction = *data_tran;
	
	for (int i=0; i< total_trans_row; i++)
	{
		re_TXLIS_msg.match_transaction[i] = data_tran[i];		

	}
	
	
	
	
	// printf("-----------------------------------------------------------------------------------------\n"); 
	// printf("send tans number to serverM \n");
	// printf("ID = %d \n", re_TXLIS_msg.decive_ID); 
	// printf("operation = %d \n", re_TXLIS_msg.operation);	
	// printf("user_1 = %s \n", re_TXLIS_msg.user_1);
	// printf("total_trans_number = %d \n", re_TXLIS_msg.total_trans_number);
	// printf("match_transaction[1].sender = %s \n", re_TXLIS_msg.match_transaction[1].sender);	
	// printf("SIZE = %d \n", sizeof(re_TXLIS_msg));
	// printf("-----------------------------------------------------------------------------------------\n"); 
	
	if (sendto(sockfd_backend_Server_UDP, &re_TXLIS_msg, sizeof(re_TXLIS_msg), 0, (const struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr)) == -1) 
	{
		perror("fail to send match trans number to serverM");
		exit(1);
	}

	// printf("Finish backend_TXLIS_func_0418 \n");
	// printf("======================================================\n");
	
	
	
	
}

int main(int argc, char *argv[])
{
	///////////////////
	///// define backend server number	
	char backend_server_name[] = "ABC";
	// printf ("backend_server_name_arr: %s \n",backend_server_name); 
	// printf ("backend_server_name: %c \n",backend_server_name[backend_server_number]); 
	
	
	//// iniatial UDP socket port 
	port_arr[0] = serverA_UDP_PORT;
	port_arr[1] = serverB_UDP_PORT;
	port_arr[2] = serverC_UDP_PORT;
	
	// printf ("port_arr[0]: %d \n", port_arr[0]); 
	// printf ("port_arr[1]: %d \n", port_arr[1]); 
	// printf ("port_arr[2]: %d \n", port_arr[2]); 
	
	
	// backend_server_name[0][0] = "A";
	// backend_server_name[1][0] = "B";
	// backend_server_name[2][0] = "C";
	
	/////////////============= Read txt file 
	char *read_file_name[3][100];
	
	 // read_file_name[0][0] = "block_test.txt"; //// block1.txt
	 read_file_name[0][0] = "block1.txt"; //// block1.txt
	 read_file_name[1][0] = "block2.txt"; //// block2.txt
	 read_file_name[2][0] = "block3.txt"; //// block3.txt
	
	
	total_trans_row = H_read_transactions_0329_func(*read_file_name[backend_server_number], data_tran);
	// printf(" read file complete \n");
	
	
	// printf("-------------------------------------------\n");	
	// for (int i=0; i< total_trans_row; i++)
	// {
		// printf("data_tran[row].number = %d:\n", data_tran[i].number);
		// printf("data_tran[row].sender = %s:\n", data_tran[i].sender);
		// printf("data_tran[row].receiver = %s:\n", data_tran[i].receiver);
		// printf("data_tran[row].trans_amount = %d:\n", data_tran[i].trans_amount);
		// printf("-------------------------------------------\n");	

	// }
	/////////////============= Finish read txt 
	
	
	/////////////============= create UDP socket
	create_backend_UDP_server_socket();	
	/////////////============= Finish create UDP socket
	
	
	/////////////============= send startup signal serverM message 
	int startup_signal = 1;
	if (sendto(sockfd_backend_Server_UDP, &startup_signal, sizeof(startup_signal), 0, (const struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr)) == -1) 
	{
		perror("fail to send startup signal to serverM");
		exit(1);
	}	
	/////////////============= Finish send startup signal serverM message 
	
	//// !!! on screen !!!
	// Booting Up (Only while starting):
	// “The ServerA is up and running using UDP on port <port number>.”
	printf("The Server %c is up and running using UDP on port %d. \n", backend_server_name[backend_server_number], port_arr[backend_server_number]); 
	//// !!! on screen !!!
	
	
	//// wait serverM message 	
	while(1)
	{	
		// printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		// printf("UDP backend waiting... \n"); 
		//// M_2_BA_message_recv
		//// reset 
		memset(&M_2_BA_message_recv, 0, sizeof(M_2_BA_message_recv));
		socklen_t ServerM_UDP_server_addr_size = sizeof(ServerM_UDP_server_addr);
		//// receive 
		// recvfrom(sockfd_backend_Server_UDP, &M_2_BA_message_recv, sizeof(M_2_BA_message_recv), 0,(struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr));
		recvfrom(sockfd_backend_Server_UDP, &M_2_BA_message_recv, sizeof(M_2_BA_message_recv), 0,(struct sockaddr *) &ServerM_UDP_server_addr, &ServerM_UDP_server_addr_size);

		

		//// !!! on screen !!!
		// Upon Receiving the request from main server
		// “The ServerA received a request from the Main Server.”
		printf("The Server %c received a request from the Main Server. \n", backend_server_name[backend_server_number]); 
		//// !!! on screen !!!


		// printf("UDP backend recv \n"); 
		// printf(" ID = %d \n", M_2_BA_message_recv.decive_ID); 
		// printf(" operation = %d \n", M_2_BA_message_recv.operation);
		// printf(" status = %d \n", M_2_BA_message_recv.status);
		
		// printf(" user_1 = %s \n", M_2_BA_message_recv.user_1);
		// printf(" user_2 = %s \n", M_2_BA_message_recv.user_2);		
		
		// printf(" SIZE = %d \n", sizeof(M_2_BA_message_recv));
		
		
		/////////////============= take action 
		
		switch(M_2_BA_message_recv.operation)
		{
			case 0: //// check balance
				check_balance_func_0416();			
			break;
			
			case 1: //// TxCoins
				if(M_2_BA_message_recv.status == 0) //// search match trans
				{
					check_TxCoins_func_0417();
				}
				else //// update trans
				{
					data_tran[total_trans_row].number =M_2_BA_message_recv.status; //// stutus is # of trans 					
					strncpy(data_tran[total_trans_row].sender, M_2_BA_message_recv.user_1, MAX_NAME_LENGTH);
					strncpy(data_tran[total_trans_row].receiver, M_2_BA_message_recv.user_2, MAX_NAME_LENGTH);					
					data_tran[total_trans_row].trans_amount = M_2_BA_message_recv.amount;
					
					total_trans_row++; 
					
					
					//// send comfirmation to serverM
					
					int comfirmatin_signal = 1;					
					if (sendto(sockfd_backend_Server_UDP, &comfirmatin_signal, sizeof(comfirmatin_signal), 0, (const struct sockaddr *) &ServerM_UDP_server_addr, sizeof(ServerM_UDP_server_addr)) == -1) 
					{
						perror("fail to send comfirmatin_signal to serverM");
						exit(1);
					}					
					
					// printf("======================================================\n");
					// printf("backend server updata trans record \n"); 
					// printf("ori total_trans_row = %d\n", total_trans_row-1);
					// printf("total_trans_row = %d\n", total_trans_row);					
					// printf("data_tran[row].number = %d\n", data_tran[total_trans_row  - 1].number);
					// printf("data_tran[row].sender = %s\n", data_tran[total_trans_row- 1].sender);
					// printf("data_tran[row].receiver = %s\n", data_tran[total_trans_row- 1].receiver);
					// printf("data_tran[row].trans_amount = %d\n", data_tran[total_trans_row- 1].trans_amount);
					// printf("======================================================\n");
				}
				
			
			break;
			
			case 2: //// TXLIS
				// void backend_TXLIS_func_0418()
				backend_TXLIS_func_0418();
			
			break;
			
			case 3: //// state
				// void backend_TXLIS_func_0418()
				check_balance_func_0416();			
			break;
			
			case 4:
			
			break;
			
			default:
			printf(" operation EERROR \n");			
			
		}
		
		
		
		//// !!! on screen !!!
		// After sending the results to the main server:
		// “The ServerA finished sending the response to the Main Server.”
		printf("The Server %c finished sending the response to the Main Server. \n", backend_server_name[backend_server_number]); 
		//// !!! on screen !!!
		
	} //// while(1)
		
	
	
	/*  //// !!! on screen !!!
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	O	Booting Up (Only while starting):
		“The ServerA is up and running using UDP on port <port number>.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	O	Upon Receiving the request from main server
		“The ServerA received a request from the Main Server.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	O	After sending the results to the main server:
		“The ServerA finished sending the response to the Main Server.”
	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	*/
	
}