#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../header-files/functions.h"

int takeArguments(struct_arguments **arguments,int argc,char **argv){
	int flag_a=0,flag_t=0,flag_v=0,flag_h1=0,flag_h2=0,flag_b=0;
	if(argc!=13){
		printf("\n\nWRONG NUMBER OF ARGUMENTS !!!\n\n\n");
		return -1;
	}
	(*arguments)=malloc(sizeof(struct_arguments));
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-a")==0){
			(*arguments)->bitCoinBalanceFile=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy((*arguments)->bitCoinBalanceFile,argv[i+1]);
			flag_a++;
		}
		if(strcmp(argv[i],"-t")==0){
			(*arguments)->transactionsFile=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy((*arguments)->transactionsFile,argv[i+1]);
			flag_t++;
		}
		if(strcmp(argv[i],"-v")==0){
			(*arguments)->bitCoinValue=atoi(argv[i+1]);
			flag_v++;
		}
		if(strcmp(argv[i],"-h1")==0){
			(*arguments)->senderHashtableNumOfEntries=atoi(argv[i+1]);
			flag_h1++;
		}
		if(strcmp(argv[i],"-h2")==0){
			(*arguments)->receiverHashtableNumOfEntries=atoi(argv[i+1]);
			flag_h2++;
		}
		if(strcmp(argv[i],"-b")==0){
			(*arguments)->bucketSize=atoi(argv[i+1]);
			flag_b++;
		}
	}
	if(flag_a!=1 || flag_t!=1 || flag_v!=1 || flag_h1!=1 || flag_h2!=1 || flag_b!=1){
		return -1;
	}
	return 0;
}

////////////////////////

int takeData_BitCoinBalanceFile(bitCoinIdArray **bitCoins,struct_wallets **wallets,struct_arguments *arguments){
	//check if file exists
	//https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
	if(access(arguments->bitCoinBalanceFile,F_OK)==-1){
		printf("\n\nFile 'bitCoinBalanceFile=%s' doesn't exist!!!\n\n\n",arguments->bitCoinBalanceFile);
		return -1;
	}

	//read whole file
	//https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
	FILE *f;
	f=fopen(arguments->bitCoinBalanceFile,"r");
	fseek(f,0,SEEK_END);
	long fsize=ftell(f);
	fseek(f,0,SEEK_SET);
	char *string;
	string=malloc((fsize+1)*sizeof(char));
	fread(string,fsize,1,f);
	fclose(f);

	string[fsize]=0;

	//ftiaxnw ta structs
	(*bitCoins)=malloc(sizeof(bitCoinIdArray));
	(*bitCoins)->size=0;
	(*wallets)=malloc(sizeof(struct_wallets));
	(*wallets)->size=0;

	//blepw posa diaforetika bitCoinid kai users uparxoun wste na kanw akribws to malloc
	int p=0;
	while(string[p]!=0){
		(*wallets)->size++;
		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//pernaw to onoma
			p++;
		}
		while(string[p]==' ' || string[p]=='\t'){//pernaw ta kena
			p++;
		}
		if(string[p]=='\n' || string[p]==0){//tote den exei bitcoins
			if(string[p]=='\n'){
				p++;
			}
			continue;
		}
		while(1){
			while(string[p]>='0' && string[p]<='9'){//diabazw to bitcoinid
				p++;
			}
			(*bitCoins)->size++;
			while(string[p]==' ' || string[p]=='\t'){//pernaw ta kena
				p++;
			}
			if(string[p]=='\n' || string[p]==0){//tote den exei alla bitcoins
				if(string[p]=='\n'){
					p++;
				}
				break;
			}
		}
	}

	if((*wallets)->size==0){
		printf("\n\nNo users!!!\n\n\n");
		return -1;
	}

	//ftiaxnw ta dentra kai ta wallets
	(*bitCoins)->array=malloc((*bitCoins)->size*sizeof(onebitCoinId));
	(*wallets)->users=malloc((*wallets)->size*sizeof(oneWallet));
	p=0;
	int bitcoincount=0,walletcount=0;
	while(string[p]!=0){
		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}

		int start=p;//pou arxizei to onoma
		while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//pernaw to onoma
			p++;
		}
		(*wallets)->users[walletcount].walletID=malloc((p-start+1)*sizeof(char));
		strncpy((*wallets)->users[walletcount].walletID,&(string[start]),p-start);//bazw to onoma
		(*wallets)->users[walletcount].walletID[p-start]=0;
		if(checkForDuplicateWalletID((*wallets),walletcount)==-1){
			printf("\n\nDuplicateWalletID!!!\n\n\n");
			return -1;
		}
		(*wallets)->users[walletcount].balance=0;
		(*wallets)->users[walletcount].list=NULL;

		while(string[p]==' ' || string[p]=='\t'){//pernaw ta kena
			p++;
		}
		if(string[p]=='\n' || string[p]==0){//tote den exei bitcoins
			if(string[p]=='\n'){
				p++;
			}
			walletcount++;//gt an den exei 8a ginei continue kai den 8a auksh8ei auto edw
			continue;
		}
		while(1){
			start=p;//pou arxizei to bitcoin id
			while(string[p]>='0' && string[p]<='9'){//diabazw to bitcoinid
				p++;
			}
			(*bitCoins)->array[bitcoincount].bitcoinid=malloc((p-start+1)*sizeof(char));
			strncpy((*bitCoins)->array[bitcoincount].bitcoinid,&(string[start]),p-start);//bazw to onoma
			(*bitCoins)->array[bitcoincount].bitcoinid[p-start]=0;

			(*bitCoins)->array[bitcoincount].pointTree=malloc(sizeof(bitCoinIdTreeNode));
			(*bitCoins)->array[bitcoincount].pointTree->walletID=malloc((strlen((*wallets)->users[walletcount].walletID)+1)*sizeof(char));
			strncpy((*bitCoins)->array[bitcoincount].pointTree->walletID,(*wallets)->users[walletcount].walletID,strlen((*wallets)->users[walletcount].walletID)+1);
			(*bitCoins)->array[bitcoincount].pointTree->value=arguments->bitCoinValue;
			(*bitCoins)->array[bitcoincount].pointTree->tr=NULL;
			(*bitCoins)->array[bitcoincount].pointTree->left=NULL;
			(*bitCoins)->array[bitcoincount].pointTree->right=NULL;
			if(checkForDuplicatebitCoinID((*bitCoins),bitcoincount)==-1){
				printf("\n\nDuplicatebitCoinID!!!\n\n\n");
				return -1;
			}
			(*wallets)->users[walletcount].balance+=arguments->bitCoinValue;//pros8etw sto balance thn aksia tou bitcoin pou molis diabasa
			insertList(&((*wallets)->users[walletcount].list),(*bitCoins)->array[bitcoincount].pointTree);
			bitcoincount++;

			while(string[p]==' ' || string[p]=='\t'){//pernaw ta kena
				p++;
			}
			if(string[p]=='\n' || string[p]==0){//tote den exei alla bitcoins
				if(string[p]=='\n'){
					p++;
				}
				break;
			}
		}

		walletcount++;
	}
	free(string);
	return 0;
}

int checkForDuplicateWalletID(struct_wallets *wallets,int point){//to point einai pou balame to teleutaio stoixeio , sugkrinw ola ta prohgoumena (0,point-1) me to point
	for(int i=0;i<point;i++){
		if(strcmp(wallets->users[i].walletID,wallets->users[point].walletID)==0){
				return -1;
		}
	}
	return 0;
}

int checkForDuplicatebitCoinID(bitCoinIdArray *bitCoins,int point){//to point einai pou balame to teleutaio stoixeio
	for(int i=0;i<point;i++){
		if(strcmp(bitCoins->array[i].bitcoinid,bitCoins->array[point].bitcoinid)==0){
				return -1;
		}
	}
	return 0;
}


//list functions for usersBitCoinsNode
void printList(usersBitCoinsNode *list){
	printf("LIST :");
	usersBitCoinsNode *temp;

	temp=list;
	while(temp!=NULL){
		printf(" %s %d //",temp->treenode->walletID,temp->treenode->value);
		temp=temp->next;
	}
	printf("\n");
}

void insertList(usersBitCoinsNode **list,bitCoinIdTreeNode *node){
	usersBitCoinsNode *temp;
	if((*list)==NULL){
		(*list)=malloc(sizeof(usersBitCoinsNode));
		(*list)->treenode=node;
		(*list)->next=NULL;
	}
	else{
		temp=(*list);
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next=malloc(sizeof(usersBitCoinsNode));
		temp->next->treenode=node;
		temp->next->next=NULL;
	}
}


void deleteList_node(usersBitCoinsNode **list){//delete ton prwto kombo
	usersBitCoinsNode *del;

	if((*list)!=NULL){
		del=(*list);
		(*list)=(*list)->next;
		free(del);
	}
}


int takeData_TransactionsFile(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable **senderHashTable,HashTable **receiverHashTable,listOfTransactions **ListOfTransactions,struct_arguments *arguments){
	int i,max_tr_id=0;
	//check if file exists
	//https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
	if(access(arguments->transactionsFile,F_OK)==-1){
		printf("\n\nFile 'transactionsFile=%s' doesn't exist!!!\n\n\n",arguments->transactionsFile);
		return -1;
	}

	//read whole file
	//https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
	FILE *f;
	f=fopen(arguments->transactionsFile,"r");
	fseek(f,0,SEEK_END);
	long fsize=ftell(f);
	fseek(f,0,SEEK_SET);
	char *string;
	string=malloc((fsize+1)*sizeof(char));
	fread(string,fsize,1,f);
	fclose(f);

	string[fsize]=0;
	
	//ftiaxnw ton ListOfTransactions
	(*ListOfTransactions)=malloc(sizeof(listOfTransactions));
	(*ListOfTransactions)->start=NULL;
	(*ListOfTransactions)->end=NULL;

	//ftiaxnw ta hashTables , sthn arxh ola ta buckets tous einai NULL kai den uparxei kanena bucketNode
	(*senderHashTable)=malloc(sizeof(HashTable));
	(*senderHashTable)->numOfUsersPerBucket=(arguments->bucketSize-sizeof(int)-sizeof(bucketNode *))/sizeof(bucketElement *);
	(*senderHashTable)->buckets=malloc(arguments->senderHashtableNumOfEntries*sizeof(bucketNode *));
	for(i=0;i<arguments->senderHashtableNumOfEntries;i++){
		(*senderHashTable)->buckets[i]=NULL;
	}


	(*receiverHashTable)=malloc(sizeof(HashTable));
	(*receiverHashTable)->numOfUsersPerBucket=(arguments->bucketSize-sizeof(int)-sizeof(bucketNode *))/sizeof(bucketElement *);
	(*receiverHashTable)->buckets=malloc(arguments->receiverHashtableNumOfEntries*sizeof(bucketNode *));
	for(i=0;i<arguments->receiverHashtableNumOfEntries;i++){
		(*receiverHashTable)->buckets[i]=NULL;
	}


	//diabazw ta transactions
	int p=0;
	while(string[p]!=0){
		transaction *tr;
		tr=malloc(sizeof(transaction));

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		int start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw transactionID
			p++;
		}
		tr->transactionID=malloc((p-start+1)*sizeof(char));
		strncpy(tr->transactionID,&(string[start]),p-start);
		tr->transactionID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw senderWalletID
			p++;
		}
		tr->senderWalletID=malloc((p-start+1)*sizeof(char));
		strncpy(tr->senderWalletID,&(string[start]),p-start);
		tr->senderWalletID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			p++;
		}
		tr->receiverWalletID=malloc((p-start+1)*sizeof(char));
		strncpy(tr->receiverWalletID,&(string[start]),p-start);
		tr->receiverWalletID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->value=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw value
			tr->value=tr->value*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->date=malloc(sizeof(struct_date));
		tr->date->day=0;
		while(string[p]!='-'){//diabazw day
			tr->date->day=tr->date->day*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->month=0;
		while(string[p]!='-'){//diabazw month
			tr->date->month=tr->date->month*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->year=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw year
			tr->date->year=tr->date->year*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->time=malloc(sizeof(struct_time));
		tr->time->hour=0;
		while(string[p]!=':'){//diabazw hour
			tr->time->hour=tr->time->hour*10+string[p]-'0';
			p++;
		}
		p++;
		tr->time->minutes=0;
		while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//diabazw minutes
			tr->time->minutes=tr->time->minutes*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}

		if(checkForDuplicateTransactionID((*ListOfTransactions))==-1){
			printf("\n\nDuplicateTransactionID!!!\n\n\n");
			return -1;
		}
		if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios
			printf("transaction '%s %s %s %d %d-%d-%d %d:%d' is canceled because Sender==Receiver\n",tr->transactionID,tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
			free(tr);
			continue;
		}
		if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){
			printf("transaction '%s %s %s %d %d-%d-%d %d:%d' is canceled because Sender or Receiver NOT FOUND\n",tr->transactionID,tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
			free(tr);
			continue;
		}

		if(max_tr_id<strlen(tr->transactionID)){
			max_tr_id=strlen(tr->transactionID);
		}

		if(executeTransaction(bitCoins,wallets,(*senderHashTable),(*receiverHashTable),arguments,tr)==0){//0 dld exei ektelestei me epitixia
			if((*ListOfTransactions)->start==NULL){
				(*ListOfTransactions)->start=tr;
				(*ListOfTransactions)->start->next=NULL;
				(*ListOfTransactions)->end=(*ListOfTransactions)->start;
			}
			else{
				(*ListOfTransactions)->end->next=tr;				
				(*ListOfTransactions)->end=(*ListOfTransactions)->end->next;
				(*ListOfTransactions)->end->next=NULL;
			}
		}
		else{
			free(tr->transactionID);
			free(tr->senderWalletID);
			free(tr->receiverWalletID);
			free(tr->date);
			free(tr->time);
			free(tr);
		}
	}
	free(string);
	return max_tr_id;//kanw return to mhkos tou pio megalou id , me strlen
}

int executeTransaction(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable *senderHashTable,HashTable *receiverHashTable,struct_arguments *arguments,transaction *tr){

		if(checkIfSenderHasEnoughBalance(wallets,tr)==-1){
			printf("transaction '%s %s %d %d-%d-%d %d:%d' is canceled because Sender doesn't have enought balance\n",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
			return -1;
		}

		//briskw se poia 8esh tou hash prepei na pane oi users tou transaction
		int sender_offset=hash(tr->senderWalletID,arguments->senderHashtableNumOfEntries);
		int receiver_offset=hash(tr->receiverWalletID,arguments->receiverHashtableNumOfEntries);

		//gia ton sender
		bucketNode *sender_temp;
		int sender_point_on_bucket;

		if(senderHashTable->buckets[sender_offset]==NULL){
			senderHashTable->buckets[sender_offset]=malloc(sizeof(bucketNode));
			senderHashTable->buckets[sender_offset]->arrayOfUsers=malloc(senderHashTable->numOfUsersPerBucket*sizeof(bucketElement));
			senderHashTable->buckets[sender_offset]->last_entry=0;
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].users=findUser(wallets,tr->senderWalletID);//sunarthsh pou epistrefei to deikth sto wallet
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].transactionList->tr=tr;
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].transactionList->next=NULL;
			senderHashTable->buckets[sender_offset]->next=NULL;

			sender_temp=senderHashTable->buckets[sender_offset];
			sender_point_on_bucket=0;
		}
		else{
			if(checkIfUserIsAlreadyInHashTable(senderHashTable,sender_offset,tr->senderWalletID,&sender_temp,&sender_point_on_bucket)){//epistrefei to deikth ekei pou ton brhke
				transactionNode *temp;
				temp=sender_temp->arrayOfUsers[sender_point_on_bucket].transactionList;
				while(temp->next!=NULL){
					temp=temp->next;
				}
				temp->next=malloc(sizeof(transactionNode));
				temp->next->tr=tr;
				temp->next->next=NULL;
			}
			else{//den uparxei sto hash table
				sender_temp=senderHashTable->buckets[sender_offset];
				while(sender_temp->last_entry+1>=senderHashTable->numOfUsersPerBucket && sender_temp->next!=NULL){//paw sto telos tis lista apo buckets
					sender_temp=sender_temp->next;
				}

				if(sender_temp->last_entry+1>=senderHashTable->numOfUsersPerBucket){//ftiaxnw neo bucket
					sender_temp->next=malloc(sizeof(bucketNode));
					sender_temp=sender_temp->next;

					sender_temp->arrayOfUsers=malloc(senderHashTable->numOfUsersPerBucket*sizeof(bucketElement));
					sender_temp->last_entry=0;
					sender_temp->arrayOfUsers[0].users=findUser(wallets,tr->senderWalletID);//epistrefei to deikth sto wallet
					sender_temp->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
					sender_temp->arrayOfUsers[0].transactionList->tr=tr;
					sender_temp->arrayOfUsers[0].transactionList->next=NULL;
					sender_temp->next=NULL;

					sender_point_on_bucket=0;
				}
				else{
					sender_temp->last_entry++;
					int lentry=sender_temp->last_entry;
					sender_temp->arrayOfUsers[lentry].users=findUser(wallets,tr->senderWalletID);
					sender_temp->arrayOfUsers[lentry].transactionList=malloc(sizeof(transactionNode));
					sender_temp->arrayOfUsers[lentry].transactionList->tr=tr;
					sender_temp->arrayOfUsers[lentry].transactionList->next=NULL;

					sender_point_on_bucket=lentry;
				}
			}
		}

		//omoiws gia ton receiver
		bucketNode *receiver_temp;
		int receiver_point_on_bucket;

		if(receiverHashTable->buckets[receiver_offset]==NULL){
			receiverHashTable->buckets[receiver_offset]=malloc(sizeof(bucketNode));
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers=malloc(receiverHashTable->numOfUsersPerBucket*sizeof(bucketElement));
			receiverHashTable->buckets[receiver_offset]->last_entry=0;
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].users=findUser(wallets,tr->receiverWalletID);
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].transactionList->tr=tr;
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].transactionList->next=NULL;
			receiverHashTable->buckets[receiver_offset]->next=NULL;

			receiver_temp=receiverHashTable->buckets[receiver_offset];
			receiver_point_on_bucket=0;
		}
		else{
			if(checkIfUserIsAlreadyInHashTable(receiverHashTable,receiver_offset,tr->receiverWalletID,&receiver_temp,&receiver_point_on_bucket)){
				transactionNode *temp;
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				while(temp->next!=NULL){
					temp=temp->next;
				}
				temp->next=malloc(sizeof(transactionNode));
				temp->next->tr=tr;
				temp->next->next=NULL;
			}
			else{//den uparxei sto hash table
				receiver_temp=receiverHashTable->buckets[receiver_offset];
				while(receiver_temp->last_entry+1>=receiverHashTable->numOfUsersPerBucket && receiver_temp->next!=NULL){//paw sto telos tis lista apo buckets
					receiver_temp=receiver_temp->next;
				}

				if(receiver_temp->last_entry+1>=receiverHashTable->numOfUsersPerBucket){//ftiaxnw neo bucket
					receiver_temp->next=malloc(sizeof(bucketNode));
					receiver_temp=receiver_temp->next;

					receiver_temp->arrayOfUsers=malloc(receiverHashTable->numOfUsersPerBucket*sizeof(bucketElement));
					receiver_temp->last_entry=0;
					receiver_temp->arrayOfUsers[0].users=findUser(wallets,tr->receiverWalletID);//sunarthsh pou na epistrefei to deikth sto wallet
					receiver_temp->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
					receiver_temp->arrayOfUsers[0].transactionList->tr=tr;
					receiver_temp->arrayOfUsers[0].transactionList->next=NULL;
					receiver_temp->next=NULL;

					receiver_point_on_bucket=0;
				}
				else{
					receiver_temp->last_entry++;
					int lentry=receiver_temp->last_entry;
					receiver_temp->arrayOfUsers[lentry].users=findUser(wallets,tr->receiverWalletID);
					receiver_temp->arrayOfUsers[lentry].transactionList=malloc(sizeof(transactionNode));
					receiver_temp->arrayOfUsers[lentry].transactionList->tr=tr;
					receiver_temp->arrayOfUsers[lentry].transactionList->next=NULL;

					receiver_point_on_bucket=lentry;
				}
			}
		}

		//ananewnw to balance tou sender kai tou receiver
		sender_temp->arrayOfUsers[sender_point_on_bucket].users->balance-=tr->value;
		receiver_temp->arrayOfUsers[receiver_point_on_bucket].users->balance+=tr->value;

		int remaining=tr->value;
		while(remaining>0){//metaferw ta bitcoins
			sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->tr=tr;//tou sundew to tr , TA FULLA DEN DEIXNOUN SE TR !!!
			if(remaining<sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value){//afhnei upoloipo ston kombo ara 2 komboi
				//left
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left=malloc(sizeof(bitCoinIdTreeNode));//ekei ta edwse
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID=malloc((strlen(tr->receiverWalletID)+1)*sizeof(char));
				strcpy(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID,tr->receiverWalletID);
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->value=remaining;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->tr=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->left=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->right=NULL;

				insertList(&(receiver_temp->arrayOfUsers[receiver_point_on_bucket].users->list),sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left);

				//right
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right=malloc(sizeof(bitCoinIdTreeNode));
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->walletID=malloc((strlen(tr->senderWalletID)+1)*sizeof(char));
				strcpy(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->walletID,tr->senderWalletID);
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->value=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value-remaining;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->tr=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->left=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->right=NULL;

				insertList(&(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list),sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right);
			}
			else{//ton xreiazomaste olo , enan kombo , (gia tipikous logous dialegw ton left)
				//left
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left=malloc(sizeof(bitCoinIdTreeNode));//ekei ta edwse
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID=malloc((strlen(tr->receiverWalletID)+1)*sizeof(char));
				strcpy(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID,tr->receiverWalletID);
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->value=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->tr=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->left=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->right=NULL;

				insertList(&(receiver_temp->arrayOfUsers[receiver_point_on_bucket].users->list),sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left);

			}

			remaining-=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value;

			//digrafw ton kombo tou bitcoin pou molis metefeta lefta
			deleteList_node(&(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list));
		}

	return 0;
}

int checkIfSenderHasEnoughBalance(struct_wallets *wallets,transaction *tr){
	for(int i=0;i<wallets->size;i++){
		if(strcmp(wallets->users[i].walletID,tr->senderWalletID)==0){//psaxnw ton xrhsth
			if(wallets->users[i].balance>=tr->value){
				return 0;
			}
			else{
				return -1;
			}
		}
	}
	return -1;
}

oneWallet *findUser(struct_wallets *wallets,char *str){
	for(int i=0;i<wallets->size;i++){
		if(strcmp(wallets->users[i].walletID,str)==0){//psaxnw ton xrhsth
			return &(wallets->users[i]);
		}
	}
	return NULL;//an den ton brw epistrefw NULL
}

onebitCoinId *findBitCoin(bitCoinIdArray *bitCoins,char *str){
	for(int i=0;i<bitCoins->size;i++){
		if(strcmp(bitCoins->array[i].bitcoinid,str)==0){//psaxnw ton xrhsth
			return &(bitCoins->array[i]);
		}
	}
	return NULL;//an den ton brw epistrefw NULL
}

int hash(char *str,int mod){
	int i,sum=0,temp=strlen(str);

	for(i=0;i<temp;i++){
		sum+=str[i];
	}
	sum=sum%mod;
	return sum;
}

transaction *breakTransaction(char *string,int begin,int end,listOfTransactions *ListOfTransactions){
	int p=begin;
		transaction *tr;
		tr=malloc(sizeof(transaction));

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		int start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw senderWalletID
			p++;
		}
		tr->senderWalletID=malloc((p-start+1)*sizeof(char));
		strncpy(tr->senderWalletID,&(string[start]),p-start);
		tr->senderWalletID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			p++;
		}
		tr->receiverWalletID=malloc((p-start+1)*sizeof(char));
		strncpy(tr->receiverWalletID,&(string[start]),p-start);
		tr->receiverWalletID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->value=0;
		while(string[p]!=' ' && string[p]!='\t' && p<end){//diabazw value
			tr->value=tr->value*10+string[p]-'0';
			p++;
		}

		start=p;//pou exw menei , koitazw an uparxei hmerominia gia na diabasw
		int flag_date=0,flag_time=0;
		while(p<end){
			if(string[p]=='-'){
				flag_date++;
			}
			if(string[p]==':'){
				flag_time++;
			}
			p++;
		}

		if(flag_date==2 && flag_time==1){//diabaseta katw
			p=start;
		}
		else if(flag_date==0 && flag_time==0){//dinw thn twrinh wra
			//https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program
			time_t t=time(NULL);
			struct tm tm=*localtime(&t);

			tr->date=malloc(sizeof(struct_date));
			tr->date->year=tm.tm_year+1900;
			tr->date->month=tm.tm_mon+1;
			tr->date->day=tm.tm_mday;
			tr->time=malloc(sizeof(struct_time));
			tr->time->hour=tm.tm_hour;
			tr->time->minutes=tm.tm_min;

			return tr;
		}
		else{//exei dwsei la8os px mono time
			return NULL;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->date=malloc(sizeof(struct_date));
		tr->date->day=0;
		while(string[p]!='-'){//diabazw day
			tr->date->day=tr->date->day*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->month=0;
		while(string[p]!='-'){//diabazw month
			tr->date->month=tr->date->month*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->year=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw year
			tr->date->year=tr->date->year*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->time=malloc(sizeof(struct_time));
		tr->time->hour=0;
		while(string[p]!=':'){//diabazw hour
			tr->time->hour=tr->time->hour*10+string[p]-'0';
			p++;
		}
		p++;
		tr->time->minutes=0;//diabazw minutes
		while(p<end && string[p]>='0' && string[p]<='9'){
			tr->time->minutes=tr->time->minutes*10+string[p]-'0';
			p++;
		}

		if(check_time(tr,ListOfTransactions)==1){
			printf("transaction '%s %s %d %d-%d-%d %d:%d' is canceled because of ",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
			printf("invalid time of transaction\n");
			free(tr->date);
			free(tr->time);
			free(tr);
			return NULL;
		}
		else{
			return tr;
		}
}

int checkForDuplicateTransactionID(listOfTransactions *lot){//sugkrinw ola me to end
	if(lot->start==lot->end){
		return 0;//pali, an einai mono 1(h 0 dld null) den to elegxw
	}
	transaction *temp;
	temp=lot->start;
	while(temp->next!=NULL){//blepw ola ta id kai ta checkarw me to teleutaio
		if(strcmp(lot->end->transactionID,temp->transactionID)==0){
				return -1;
		}
		temp=temp->next;
	}
	return 0;
}

int checkIfUserIsAlreadyInHashTable(HashTable *ht,int offset,char *str,bucketNode **temp_to_bucket,int *point_on_bucket){
	bucketNode *temp;
	temp=ht->buckets[offset];//phgainw sthn arxh thn listas apo buckets
	while(temp!=NULL){
		for(int i=0;i<=temp->last_entry;i++){
			if(strcmp(temp->arrayOfUsers[i].users->walletID,str)==0){
				(*temp_to_bucket)=temp;//epistrefw to bucket kai thn 8esh tou mesa se auto
				*point_on_bucket=i;
				return 1;//uparxei
			}
		}
		temp=temp->next;
	}

	return 0;//den uparxei
}


void printTree(bitCoinIdTreeNode *node){
	if(node==NULL)return;
	printf("%s %d",node->walletID,node->value);
	printf("\n");

	printTree(node->left);
	printTree(node->right);
}

int findUnspent(bitCoinIdTreeNode *node){//phgainw olo deksia , an o teleutaios kombos exei aristero kai den exei deksi paidi dhmainei oti olo to poso exei metafer8ei (ara 0) alliws epistrefw to value tou sugkekrimenou kombou
	while(node->right!=NULL){
		node=node->right;
	}
	if(node->left!=NULL){//ara ta teleutaia xrhmata phgan ola se kapoion allon
		return 0;
	}
	else{
		return node->value;
	}
}

int countDigits(int x){
	int c=0;
	while(x>0){
		c++;
		x=x/10;
	}
	return c;
}

void printTransactionHistory(bitCoinIdTreeNode *node){
	if(node->left!=NULL){//an einai NULL to left paidi tote autos o KOMBOS den exei labei meros se transaction
		if(node->tr->unused==0){
			printf("%s %s %s %d %d-%d-%d %d:%d\n",node->tr->transactionID,node->tr->senderWalletID,node->tr->receiverWalletID,node->tr->value,node->tr->date->day,node->tr->date->month,node->tr->date->year,node->tr->time->hour,node->tr->time->minutes);
			node->tr->unused=1;
		}
		printTransactionHistory(node->left);
		if(node->right!=NULL){
			printTransactionHistory(node->right);
		}
	}
}




int check_date(transaction *tr,struct_time time1,struct_time time2,struct_date date1,struct_date date2,int flag_time,int flag_date){
	if(flag_date==4){
		int qaz_tr=tr->date->year*10000+tr->date->month*100+tr->date->day;
		int qaz_1=date1.year*10000+date1.month*100+date1.day;
		int qaz_2=date2.year*10000+date2.month*100+date2.day;

		if(qaz_tr>qaz_1 && qaz_tr<qaz_2){
			return 1;
		}
		else if(qaz_tr==qaz_1 && qaz_tr==qaz_2){//dld milame gia thn idia mera
			if(flag_time==2){
				int qwe_tr=tr->time->hour*100+tr->time->minutes;
				int qwe_1=time1.hour*100+time1.minutes;
				int qwe_2=time2.hour*100+time2.minutes;

				if(qwe_tr>=qwe_1 && qwe_tr<=qwe_2){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 1;
			}
		}
		else if(qaz_tr==qaz_1){
			if(flag_time==2){
				int qwe_tr=tr->time->hour*100+tr->time->minutes;
				int qwe_1=time1.hour*100+time1.minutes;

				if(qwe_tr>=qwe_1){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 1;
			}
		}
		else if(qaz_tr==qaz_2){
			if(flag_time==2){
				int qwe_tr=tr->time->hour*100+tr->time->minutes;
				int qwe_2=time2.hour*100+time2.minutes;

				if(qwe_tr<=qwe_2){
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 1;
			}
		}
	}
	else if(flag_time==2){
		int qwe_tr=tr->time->hour*100+tr->time->minutes;
		int qwe_1=time1.hour*100+time1.minutes;
		int qwe_2=time2.hour*100+time2.minutes;

		if(qwe_tr>=qwe_1 && qwe_tr<=qwe_2){
			return 1;
		}
		else{
			return 0;
		}
	}
	return 0;
}

void freeBitCoinTree(bitCoinIdTreeNode *node){
	if(node!=NULL){
		freeBitCoinTree(node->left);
		freeBitCoinTree(node->right);
		free(node->walletID);
		free(node);
	}
}

void freeWalletUsersList(usersBitCoinsNode *node){
	if(node!=NULL){
		freeWalletUsersList(node->next);
		free(node);
	}
}

void freeHashTableBuckets(bucketNode *node){
	if(node!=NULL){
		freeHashTableBuckets(node->next);
		for(int i=0;i<=node->last_entry;i++){
			freeHashTableBucketsTrList(node->arrayOfUsers[i].transactionList);
		}
		free(node->arrayOfUsers);
		free(node);
	}
}

void freeHashTableBucketsTrList(transactionNode *node){
	if(node!=NULL){
		freeHashTableBucketsTrList(node->next);
		free(node);
	}
}

void freeListOfTransactions(transaction *node){
	if(node!=NULL){
		freeListOfTransactions(node->next);
		free(node->transactionID);
		free(node->senderWalletID);
		free(node->receiverWalletID);
		free(node->date);
		free(node->time);
		free(node);
	}
}


int check_time(transaction *tr,listOfTransactions *ListOfTransactions){
	if(ListOfTransactions->end==NULL){//einai adeio opote exoume "8etiko return"
		return 0;
	}

	int qaz_tr=tr->date->year*10000+tr->date->month*100+tr->date->day;
	int qaz_list=ListOfTransactions->end->date->year*10000+ListOfTransactions->end->date->month*100+ListOfTransactions->end->date->day;

	if(qaz_tr < qaz_list){
		return 1;
	}
	else if(qaz_tr > qaz_list){
		return 0;
	}
	else{//==
		int qwe_tr=tr->time->hour*100+tr->time->minutes;
		int qwe_list=ListOfTransactions->end->time->hour*100+ListOfTransactions->end->time->minutes;

		if(qwe_tr < qwe_list){
			return 1;
		}
		else{
			return 0;
		}
	}
}

int countTransactions(bitCoinIdTreeNode *node){
	int count=0;
	if(node->left!=NULL){//an einai NULL to left paidi tote autos o KOMBOS den exei labei meros se transaction
		if(node->tr->unused==0){
			node->tr->unused=1;
			count=countTransactions(node->left);
			if(node->right!=NULL){
				count+=countTransactions(node->right);
			}
			return count+1;//+1 gia ton kombo pou eimai twra
		}
		else{
			count=countTransactions(node->left);
			if(node->right!=NULL){
				count+=countTransactions(node->right);
			}
			return count;
		}
	}
	else{
		return 0;//einai null kai den exei kanei transactions
	}
}





