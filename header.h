

/*

header

Sheng Chieh Lin 
USC ID: 2314107438


*/

////---- Log ----////

#ifndef HEADER_FILE
#define HEADER_FILE


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>

////---- Defined Constants ----//// 


#define LOCAL_HOST "127.0.0.1" // Host IP address 

#define MAX_DATA_SIZE 1024 // max number of bytes we can get at once
#define MAX_NAME_LENGTH 20 //  max length of bytes we can type for name 
#define MAX_PAYLOAD_LENGTH 1024 //  max length of bytes we can type for name
#define MAX_TRANS_DATA 1000 //  max number of transactions for backend server 


// server TCP port
#define ServerM_TCP_PORT_4CA 25438 //  serverM port number 25XXX for clientA
#define ServerM_TCP_PORT_4CB 26438 //  serverM port number 25XXX for clientB

// UDP port
#define serverA_UDP_PORT 21438 // serverA server port number 21XXX
#define serverB_UDP_PORT 22438 // serverB server port number 22XXX
#define serverC_UDP_PORT 23438 // serverC server port number 23XXX

#define serverM_UDP_PORT 24438 // serverM server port number 24XXX


//// message 
typedef struct data_4_search_transactions
{
   char user[MAX_NAME_LENGTH];   
   int number_transactions;
   int send_amount;
   int receive_amount;
   int total_amount;
   
   

}Data_user_transaction;

typedef struct data_4_normal_C2M
{
   int decive_ID;
   int operation;
   char user_1[MAX_NAME_LENGTH];
   char user_2[MAX_NAME_LENGTH];
   int amount;   
   // int payload;


}Data_C_BT_M;

typedef struct data_4_normal_M2C
{
   int decive_ID;
   int operation;
   int status;
   int payload_number;
   int balance;
   
   int balance_1;
   int balance_2;
   
   char payload[MAX_PAYLOAD_LENGTH];
   Data_user_transaction state_list[MAX_TRANS_DATA];


}Data_M2C;


typedef struct data_4_read_transactions
{
   int number;
   char sender[MAX_NAME_LENGTH];
   char receiver[MAX_NAME_LENGTH];
   int trans_amount;

}Data_transaction;




typedef struct data_4_normal_M_2_BackServer
{
   int decive_ID;
   int operation;
   int status;   
   char user_1[MAX_NAME_LENGTH];
   char user_2[MAX_NAME_LENGTH];
   int amount;


}Data_M2BackServer;

typedef struct data_4_normal_BackServer_2_M
{
   int decive_ID;
   int operation;
   char user_1[MAX_NAME_LENGTH];
   int total_trans_number;
   Data_transaction match_transaction[MAX_TRANS_DATA];

}Data_BackServer_2M_balance;

typedef struct Data_BackServer_2M_TxCoins_ST
{
   int decive_ID;
   int operation;
   int status;
   
   char user_1[MAX_NAME_LENGTH];   
   char user_2[MAX_NAME_LENGTH];
   
   int user_1_amount;
   int user_2_amount;
   
   int user_1_match_count;
   int user_2_match_count;
   
   int max_tran_number;
   

}Data_BackServer_2M_TxCoins;

typedef struct Data_Backend_SearchTxCoin_trans_ST
{   
   char sender[MAX_NAME_LENGTH];
   int sender_amount;
   int sender_match_count;

   char receiver[MAX_NAME_LENGTH];
   int receiver_amount;
   int receiver_match_count;
   
   int max_trans_number;
   
   int status;
   
	
}Data_Backend_SearchTxCoin_trans;


char separate[]="|";
char space_char[]=" ";

int initial_amount = 1000;
Data_transaction TXLIS_list[MAX_TRANS_DATA*3];


//////////////

int H_read_transactions_0329_func(char *file_name, Data_transaction *data_tran_IN)
{
	// printf("file_name: %s \n", file_name);
	//// https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
	
	char name;
	int amount;
	
	int row = 0;

    fp = fopen(file_name, "r"); //// different backend server different file name 
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s", line);
		
		if (read == 1)
		{
			 // printf("Read complete \n");
			 // printf("total row = %d:\n", row);
			 break;
			
		}
		
		// get number name1 name2 and amount from line
		char *token;
		token = strtok(line, space_char);
		
		int counter = 0;
				
		while( token != NULL ) {
			
			// printf( " %s\n", token );
			if (counter%4 == 0) //// number 
			{
				// printf("row = %d:\n", row);
				data_tran_IN[row].number = atoi(token);
				// printf("data_tran_IN[row].number = %d:\n", data_tran_IN[row].number);
				
			}
			else if (counter%4 == 1) //// sender
			{
				strncpy(data_tran_IN[row].sender, token, MAX_NAME_LENGTH);
				// printf("data_tran_IN[row].sender = %s:\n", data_tran_IN[row].sender);
			}
			else if (counter%4 == 2) //// receiver
			{
				strncpy(data_tran_IN[row].receiver, token, MAX_NAME_LENGTH);
				// printf("data_tran_IN[row].receiver = %s:\n", data_tran_IN[row].receiver);
			}
			else if (counter%4 == 3) //// trans amount
			{
				data_tran_IN[row].trans_amount = atoi(token);
				// printf("data_tran_IN[row].trans_amount = %d:\n", data_tran_IN[row].trans_amount);	
				// printf("-------------------------------------------\n");						
			}

			token = strtok(NULL, space_char);
			counter++;
		}//// while( token != NULL )
		
		row++;
		
    } //// ((read = getline(&line, &len, fp)) != -1) 
	
	return row;

    // printf("Close file \n");
	fclose(fp);
    if (line)
        free(line);
   
}

int search_transactions_0410_func(char *name_IN, int total_row_IN, Data_transaction *data_tran_IN, Data_transaction *match_tran_OUT)
{
	// printf("search_transactions_0410_func \n");
	// printf("total_row_IN = %d:\n", total_row_IN);
	// printf("serch name_IN = %s:\n", name_IN);
	
	
	int search_status_OUT = -1;
	
	int send_match_number = 0;
	int send_amount =0;
	
	int receive_match_number = 0;
	int receive_amount =0;
	
	int number_transactions = 0;
	
		
	for(int i=0; i< total_row_IN; i++)
	{
		if (strcmp(name_IN , data_tran_IN[i].sender) == 0) //// sender 
		{
			// printf("send match \n");
			match_tran_OUT[number_transactions].number = data_tran_IN[i].number;
			
			strncpy(match_tran_OUT[number_transactions].sender,  data_tran_IN[i].sender, MAX_NAME_LENGTH);
			strncpy(match_tran_OUT[number_transactions].receiver, data_tran_IN[i].receiver, MAX_NAME_LENGTH);
			
			match_tran_OUT[number_transactions].trans_amount = data_tran_IN[i].trans_amount;
			
			
			number_transactions++;
			
			
		}
		if (strcmp(name_IN , data_tran_IN[i].receiver) == 0) //// receiver 
		{
			// printf("receive match \n");
			match_tran_OUT[number_transactions].number = data_tran_IN[i].number;
			
			strncpy(match_tran_OUT[number_transactions].sender,  data_tran_IN[i].sender, MAX_NAME_LENGTH);
			strncpy(match_tran_OUT[number_transactions].receiver, data_tran_IN[i].receiver, MAX_NAME_LENGTH);
			
			match_tran_OUT[number_transactions].trans_amount = data_tran_IN[i].trans_amount;
			
			
			number_transactions++;
			
			
		}
		
		
	}//// for(int i=0; i< total_row_IN; i++)
		
	
	
	search_status_OUT = number_transactions;
	
	// for (int i=0; i< search_status_OUT; i++)
	// {
		// printf("-------------------------------------------\n");
		// printf("number = %d \n", match_tran_OUT[i].number);
		// printf("sender = %s \n", match_tran_OUT[i].sender);
		// printf("receiver = %s \n", match_tran_OUT[i].receiver);
		// printf("trans_amount = %d \n", match_tran_OUT[i].trans_amount);
	// }
	
	// printf("RETURN 2 main \n");
	// printf("=================================================\n");

	return search_status_OUT;
}

void compute_balance_0410_func(char *name_IN, int total_row_IN_A, Data_transaction *match_tran_A_IN
											, int total_row_IN_B, Data_transaction *match_tran_B_IN
											, int total_row_IN_C, Data_transaction *match_tran_C_IN
											, Data_user_transaction *data_user_tran_OUT)
{
	// printf("compute_balance_0410_func \n");
	// printf("total_row_IN_A = %d  \n", total_row_IN_A);
	// printf("total_row_IN_B = %d  \n", total_row_IN_B);
	// printf("total_row_IN_C = %d  \n", total_row_IN_C);
	
	// printf("serch name_IN = %s:\n", name_IN);
	
	
	
	
	int send_match_number = 0;
	int send_amount =0;
	
	int receive_match_number = 0;
	int receive_amount =0;
	
	int number_transactions = 0;
	
	for(int i=0; i< total_row_IN_A; i++)
	{
		if (strcmp(name_IN , match_tran_A_IN[i].sender) == 0) //// sender 
		{
			// printf("send match \n");
			send_amount+= match_tran_A_IN[i].trans_amount;
			send_match_number++;
			number_transactions++;			
		}
		if (strcmp(name_IN , match_tran_A_IN[i].receiver) == 0) //// receiver 
		{
			// printf("receive match \n");
			receive_amount+= match_tran_A_IN[i].trans_amount;
			receive_match_number++;
			number_transactions++;			
		}		
	}//// for(int i=0; i< total_row_IN; i++)
		
	for(int i=0; i< total_row_IN_B; i++)
	{
		if (strcmp(name_IN , match_tran_B_IN[i].sender) == 0) //// sender 
		{
			// printf("send match \n");
			send_amount+= match_tran_B_IN[i].trans_amount;
			send_match_number++;
			number_transactions++;			
		}
		if (strcmp(name_IN , match_tran_B_IN[i].receiver) == 0) //// receiver 
		{
			// printf("receive match \n");
			receive_amount+= match_tran_B_IN[i].trans_amount;
			receive_match_number++;
			number_transactions++;			
		}		
	}//// for(int i=0; i< total_row_IN; i++)
	
	for(int i=0; i< total_row_IN_C; i++)
	{
		if (strcmp(name_IN , match_tran_C_IN[i].sender) == 0) //// sender 
		{
			// printf("send match \n");
			send_amount+= match_tran_C_IN[i].trans_amount;
			send_match_number++;
			number_transactions++;			
		}
		if (strcmp(name_IN , match_tran_C_IN[i].receiver) == 0) //// receiver 
		{
			// printf("receive match \n");
			receive_amount+= match_tran_C_IN[i].trans_amount;
			receive_match_number++;
			number_transactions++;			
		}		
	}//// for(int i=0; i< total_row_IN; i++)
	
	Data_user_transaction data_user_tran;	

   
	strncpy(data_user_tran.user, name_IN, MAX_NAME_LENGTH);
	data_user_tran.number_transactions = number_transactions;
	data_user_tran.send_amount = send_amount;
	data_user_tran.receive_amount = receive_amount;
	data_user_tran.total_amount = initial_amount + receive_amount - send_amount; //// initial_amount + receive - send	
	
	*data_user_tran_OUT = data_user_tran;
	
	// printf("=================================================\n");
	// printf("user = %s:\n", data_user_tran_OUT[0].user);
	// printf("number_transactions = %d:\n", data_user_tran_OUT[0].number_transactions);
	// printf("send_amount = %d:\n", data_user_tran_OUT[0].send_amount);
	// printf("receive_amount = %d:\n", data_user_tran_OUT[0].receive_amount);
	// printf("total_amount = %d:\n", data_user_tran_OUT[0].total_amount);
	// printf("RETURN 2 main \n");
	// printf("=================================================\n");
	

}

void UDP_check_wallet_func_0417();
void create_ServerM_TCP_server_socket_0415(int client_number);
void serverM_Check_wallet_func();
int serverM_TxCoins_search_func();
void serverM_TxCoins_updata_func(int max_serial_NO_IN);
void serverM_TXLIS_func();
void serverM_Stats_func();
void create_serverM_UDP_server_socket();
void initial_connect_backend(int backend_server_number);
void serverM_UDP_setup();
void UDP_check_wallet_func_0417();
int UDP_TxCoins_func_0417(int* sender_amount_OUT, int* receiver_amount_OUT, int* max_serial_NO_OUT);
void UDP_Update_TxCoins_func_0417();
void UDP_TXLIS_func_0418();
void name_list_func_0418_v2(char *user_name_IN, Data_transaction *data_tran_IN, int total_row_IN);
void UDP_Stats_func_0418_v2();






#endif