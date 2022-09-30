# EE450-Computer Networks-2022 spring project 
● Name: Sheng Chieh Lin

● USC ID: **********

● I have done:
	In this network project, four servers, one main server and three backend server, are bulit to implement the Alichain system.
	I completed Phase 1 to 4 include exta point.	

       
● my code files: 
1. serverM.c: Main server, which charge to receive query from client, send transaction request to backend server A to C.
		-> handle filter query from client
		-> create request message
		-> send message to backend server A/B/C through UDP
		-> compute user's balance
		-> update transaction
		-> create transaction list
		-> creat stats list 
		-> response query to client

2. serverA/B/C.c: Backend server read transaction txt file and response transaction request to main server.
		-> read .txt file
		-> search match transaction with user
		-> create response message
		-> send response message to main server through UDP
		
3. clientA/B.c: Client send query to main server
		-> filter command 
		-> create request message
		-> send request message to main server through TCP
		-> display result 

4. header.h: header file
		-> defined data types
		-> defined functions
		-> define system wide parameters
		-> some of functions
		

● The format of all the messages exchanged.
I used different data types, which are defined by myself in the header.h. 

0. Common information:
	Most of messages contain some common information:
	a. decive_ID: the message source ID:
					0: serverM
					1: serverA
					2: serverB
					3: serverC
					4: clientA
					5: clientB				
	
	b. operation: the message operation:
					0: check wallet	
					1: TxCoins		
					2: TxList		
					3: Stats	

	c. user_1: first name in the common.
	d. user_2: second name in the common.	

1. client => serverM :

	Data_C_BT_M:
		->int decive_ID;  				=> description as Common information
		->int operation;				=> description as Common information
		->char user_1;  				=> description as Common information
		->char user_2;  				=> description as Common information
		->int amount;  => used to describe amount of transfer   				


2. client <= serverM :


	Data_M2C:
		int decive_ID;  				=> description as Common information
		int operation;  				=> description as Common information
		int status; 	=> used to describe search result:
								O: check balance successful 
								1: check balance failed and user is not in the network
								2: transfer successful
								3: transfer failed and the sender balance below the transaction amount
								4: transfer failed and the sender is not in the network
								5: transfer failed and the receiver is not in the network
								6: transfer failed and the sender and receiver are not in the network
		
		int balance;	=> used to describe transfer amount
		int balance_1;	=> used to describe user1's balance
		int balance_2;	=> used to describe user2's balance		
		Data_user_transaction state_list[MAX_TRANS_DATA]; 	=> used to save transaction record		
		
			Data_user_transaction: 
				char user[MAX_NAME_LENGTH];		=> user name
				int number_transactions;		=> total number of transaction in the network
				int send_amount;				=> amount of sending in the network
				int receive_amount;				=> amount of receiving in the network
				int total_amount;				=> user balance without initial amount(1000 coins)
   
3. serverM => serverA/B/C :

	Data_M2BackServer;
		int decive_ID;					=> description as Common information
		int operation;					=> description as Common information
		int status;   					=> description as Common information
		char user_1[MAX_NAME_LENGTH];	=> description as Common information
		char user_2[MAX_NAME_LENGTH]; 	=> description as Common information
		int amount;						=> description as Common information






3. serverM => serverA/B/C:

	Data_M2BackServer:
		->int decive_ID;  				=> description as Common information
		->int operation;  				=> description as Common information
		->int status;   => used to describe status of transaction						
		->char user_1;  				=> description as Common information
		->char user_2;  				=> description as Common information
		->int amount;  	=> used to describe amount of transfer   



4. serverM <= serverA/B/C: three different data types used to response to serverM due to different inquire.
	
	[1]. Data_BackServer_2M_balance:
		->int decive_ID;  				=> description as Common information
		->int operation;  				=> description as Common information
		->char user_1[MAX_NAME_LENGTH]; => description as Common information
		->int total_trans_number;		=> description as Common information
		->Data_transaction match_transaction[MAX_TRANS_DATA]; => description as before

	[2]. Data_BackServer_2M_TxCoins: 
		->int decive_ID;				=> description as Common information
		->int operation;				=> description as Common information
		->int status;					=> description as Common information
		->char user_1[MAX_NAME_LENGTH];	=> description as Common information   
		->char user_2[MAX_NAME_LENGTH];	=> description as Common information
		->int user_1_amount;			=> description as Common information
		->int user_2_amount;			=> description as Common information
		->int user_1_match_count;		=> description as Common information
		->int user_2_match_count;		=> description as Common information
		->int max_tran_number;			=> description as Common information
   
	[3]. Data_Backend_SearchTxCoin_trans:
  
		->char sender[MAX_NAME_LENGTH]; => description as Common information
		->int sender_amount;			=> description as Common information
		->int sender_match_count;	=> number of transaction involved by sender 
		->char receiver[MAX_NAME_LENGTH];	=> description as Common information
		->int receiver_amount;			=> description as Common information
		->int receiver_match_count;	=> number of transaction involved by receiver
		->int max_trans_number;		=> number of transaction save in backend server
		->int status;					=> description as Common information
	

● Reused Code: 
1. Beej's Code: http://www.beej.us/guide/bgnet/: to bulit socket 
