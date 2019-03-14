#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../header-files/functions.h"

int takeArguments(struct_arguments **arguments,int argc,char **argv){
	if(argc!=13){
		printf("\n\nWRONG NUMBER OF ARGUMENTS !!!\n\n\n");
		return -1;
	}
	(*arguments)=malloc(sizeof(struct_arguments));
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-a")==0){
			(*arguments)->bitCoinBalanceFile=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy((*arguments)->bitCoinBalanceFile,argv[i+1]);
		}
		if(strcmp(argv[i],"-t")==0){
			(*arguments)->transactionsFile=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy((*arguments)->transactionsFile,argv[i+1]);
		}
		if(strcmp(argv[i],"-v")==0){
			(*arguments)->bitCoinValue=atoi(argv[i+1]);
		}
		if(strcmp(argv[i],"-h1")==0){
			(*arguments)->senderHashtableNumOfEntries=atoi(argv[i+1]);
		}
		if(strcmp(argv[i],"-h2")==0){
			(*arguments)->receiverHashtableNumOfEntries=atoi(argv[i+1]);
		}
		if(strcmp(argv[i],"-b")==0){
			(*arguments)->bucketSize=atoi(argv[i+1]);
		}
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

	string[fsize]=0;//teleiwnei se allagh grammhs ,mporei na 8elei allagh!!!!!!!!!!!!!!!!!!!!!!!!
	//printf("\n\nstring=%s...\n",string);

	//ftiaxnw ta structs
	(*bitCoins)=malloc(sizeof(bitCoinIdArray));
	(*bitCoins)->size=0;
	(*wallets)=malloc(sizeof(struct_wallets));
	(*wallets)->size=0;

	//blepw posa diaforetika bitCoinid kai users uparxoun wste na kanw akribws to malloc
	int p=0;
	while(string[p]!=0){//MPOREI NA EXW 8EMA AN EXW KENA STO TELOS TOU ARXEIOU !!!!!!!!!!!!!!!
		(*wallets)->size++;
		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena , ebala kai \n
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
	//printf("(*bitCoins)->size=%d (*wallets)->size=%d\n",(*bitCoins)->size,(*wallets)->size);

	if((*wallets)->size==0){
		//printf error
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
		//printf("(*wallets)->users[walletcount].walletID=%s...\n",(*wallets)->users[walletcount].walletID);
		if(checkForDuplicateWalletID((*wallets),walletcount)==-1){
			printf("\n\nDuplicateWalletID!!!\n\n\n");
			return -1;
		}
		(*wallets)->users[walletcount].balance=0;
		(*wallets)->users[walletcount].list=NULL;
		//walletcount++;

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
			//printf("(*bitCoins)->array[bitcoincount].bitcoinid=%s...\n",(*bitCoins)->array[bitcoincount].bitcoinid);

			//(*bitCoins)->array[bitcoincount].numOfTransactions=0;//

			(*bitCoins)->array[bitcoincount].pointTree=malloc(sizeof(bitCoinIdTreeNode));
			(*bitCoins)->array[bitcoincount].pointTree->walletID=malloc((strlen((*wallets)->users[walletcount].walletID)+1)*sizeof(char));
			strncpy((*bitCoins)->array[bitcoincount].pointTree->walletID,(*wallets)->users[walletcount].walletID,strlen((*wallets)->users[walletcount].walletID)+1);
			(*bitCoins)->array[bitcoincount].pointTree->value=arguments->bitCoinValue;
			(*bitCoins)->array[bitcoincount].pointTree->tr=NULL;/////
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

int checkForDuplicateWalletID(struct_wallets *wallets,int point){//to point einai pou balame to teleutaio stoixeio
	for(int i=0;i<point;i++){
		if(strcmp(wallets->users[i].walletID,wallets->users[point].walletID)==0){
				//printf("\n\nDUPLICAT USERNAME!!!\n\n\n");
				return -1;
		}
	}
	return 0;
}

int checkForDuplicatebitCoinID(bitCoinIdArray *bitCoins,int point){//to point einai pou balame to teleutaio stoixeio
	for(int i=0;i<point;i++){
		if(strcmp(bitCoins->array[i].bitcoinid,bitCoins->array[point].bitcoinid)==0){
				//printf("\n\nDUPLICAT USERNAME!!!\n\n\n");
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
		//printf(" %2d %2d //",temp->usersPartOfBitCoin,temp->offsetOfBitCoin);
		printf(" %s %d //",temp->treenode->walletID,temp->treenode->value);
		temp=temp->next;
	}
	printf("\n");
}

void insertList(usersBitCoinsNode **list,bitCoinIdTreeNode *node){//MPORW NA TO BAZW STHN ARXH THS LISTA WSTE NA MHN XANW XRONO NA DIASXIZW THN LISTA
	usersBitCoinsNode *temp;
	if((*list)==NULL){
		(*list)=malloc(sizeof(usersBitCoinsNode));
		//(*list)->usersPartOfBitCoin=value;
		//(*list)->offsetOfBitCoin=offset;
		(*list)->treenode=node;
		(*list)->next=NULL;
	}
	else{
		temp=(*list);
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next=malloc(sizeof(usersBitCoinsNode));
		//temp->next->usersPartOfBitCoin=value;
		//temp->next->offsetOfBitCoin=offset;
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

/////////////////////////

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

	string[fsize]=0;//teleiwnei se allagh grammhs ,mporei na 8elei allagh
	//printf("\n\nstring=%s...\n",string);
	
	//ftiaxnw ton ListOfTransactions
	(*ListOfTransactions)=malloc(sizeof(listOfTransactions));
	(*ListOfTransactions)->start=NULL;
	(*ListOfTransactions)->end=NULL;

	//blepw posa transaction exw , metraw ta ':' , na to tsekarw pali
	/*(*ArrayOfTransactions)->size=0;
	for(i=0;i<fsize;i++){
		if(string[i]==':'){
			(*ArrayOfTransactions)->size++;
		}
	}

	(*ArrayOfTransactions)->tr=malloc((*ArrayOfTransactions)->size*sizeof(transaction));*/

	//MPOREI TA HASH NA PREPEI NA TA KANW AFOU ELEKSW OTI EINAI VALID TA ARXEIA

	//ftiaxnw ta hashTables , sthn arxh ola ta buckets tous einai NULL kai den uparxei kanena bucketNode
	(*senderHashTable)=malloc(sizeof(HashTable));
	(*senderHashTable)->numOfUsersPerBucket=(arguments->bucketSize-sizeof(int)-sizeof(bucketNode *))/sizeof(bucketElement *);
	//printf("arguments->bucketSize=%d sizeof(transactionNode *)=%ld sizeof(bucketNode *)=%ld sizeof(oneWallet *)=%ld (*senderHashTable)->numOfUsersPerBucket=%d\n",arguments->bucketSize,sizeof(transactionNode *),sizeof(bucketNode *),sizeof(oneWallet *),(*senderHashTable)->numOfUsersPerBucket);
	(*senderHashTable)->buckets=malloc(arguments->senderHashtableNumOfEntries*sizeof(bucketNode *));
	for(i=0;i<arguments->senderHashtableNumOfEntries;i++){
		(*senderHashTable)->buckets[i]=NULL;
	}


	(*receiverHashTable)=malloc(sizeof(HashTable));
	(*receiverHashTable)->numOfUsersPerBucket=(arguments->bucketSize-sizeof(int)-sizeof(bucketNode *))/sizeof(bucketElement *);
	//printf("arguments->bucketSize=%d sizeof(transactionNode *)=%ld sizeof(bucketNode *)=%ld sizeof(oneWallet *)=%ld (*receiverHashTable)->numOfUsersPerBucket=%d\n",arguments->bucketSize,sizeof(transactionNode *),sizeof(bucketNode *),sizeof(oneWallet *),(*receiverHashTable)->numOfUsersPerBucket);
	(*receiverHashTable)->buckets=malloc(arguments->receiverHashtableNumOfEntries*sizeof(bucketNode *));
	for(i=0;i<arguments->receiverHashtableNumOfEntries;i++){
		(*receiverHashTable)->buckets[i]=NULL;
	}


	//diabazw ta transactions
	int p=0;
	//int paron_tr=0;
	while(string[p]!=0){//8ELEI DIOR8WSH
		/*
/////////////////////////
		printf("AAAAAAAAAAAAAAAAA\n");
		if((*ListOfTransactions)->start==NULL){
			(*ListOfTransactions)->start=malloc(sizeof(transaction));
			(*ListOfTransactions)->start->next=NULL;
			(*ListOfTransactions)->end=(*ListOfTransactions)->start;
		}
		else{
			(*ListOfTransactions)->end->next=malloc(sizeof(transaction));
			(*ListOfTransactions)->end=(*ListOfTransactions)->end->next;
			(*ListOfTransactions)->end->next=NULL;
		}
		printf("BBBBBBBBB\n");
/////////////////////////

		
		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		int start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw transactionID
			p++;
		}
		(*ListOfTransactions)->end->transactionID=malloc((p-start+1)*sizeof(char));
		strncpy((*ListOfTransactions)->end->transactionID,&(string[start]),p-start);
		(*ListOfTransactions)->end->transactionID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw senderWalletID
			p++;
		}
		(*ListOfTransactions)->end->senderWalletID=malloc((p-start+1)*sizeof(char));
		strncpy((*ListOfTransactions)->end->senderWalletID,&(string[start]),p-start);
		(*ListOfTransactions)->end->senderWalletID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		start=p;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			p++;
		}
		(*ListOfTransactions)->end->receiverWalletID=malloc((p-start+1)*sizeof(char));
		strncpy((*ListOfTransactions)->end->receiverWalletID,&(string[start]),p-start);
		(*ListOfTransactions)->end->receiverWalletID[p-start]=0;

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		(*ListOfTransactions)->end->value=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			(*ListOfTransactions)->end->value=(*ListOfTransactions)->end->value*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		(*ListOfTransactions)->end->date=malloc(sizeof(struct_date));
		(*ListOfTransactions)->end->date->day=0;
		while(string[p]!='-'){//diabazw receiverWalletID
			(*ListOfTransactions)->end->date->day=(*ListOfTransactions)->end->date->day*10+string[p]-'0';
			p++;
		}
		p++;
		(*ListOfTransactions)->end->date->month=0;
		while(string[p]!='-'){//diabazw receiverWalletID
			(*ListOfTransactions)->end->date->month=(*ListOfTransactions)->end->date->month*10+string[p]-'0';
			p++;
		}
		p++;
		(*ListOfTransactions)->end->date->year=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			(*ListOfTransactions)->end->date->year=(*ListOfTransactions)->end->date->year*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		(*ListOfTransactions)->end->time=malloc(sizeof(struct_time));
		(*ListOfTransactions)->end->time->hour=0;
		while(string[p]!=':'){//diabazw receiverWalletID
			(*ListOfTransactions)->end->time->hour=(*ListOfTransactions)->end->time->hour*10+string[p]-'0';
			p++;
		}
		p++;
		(*ListOfTransactions)->end->time->minutes=0;
		while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//diabazw receiverWalletID
			(*ListOfTransactions)->end->time->minutes=(*ListOfTransactions)->end->time->minutes*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}

		printf("ABC %s %s %s %d %d-%d-%d %d:%d\n",(*ListOfTransactions)->end->transactionID,(*ListOfTransactions)->end->senderWalletID,(*ListOfTransactions)->end->receiverWalletID,(*ListOfTransactions)->end->value,(*ListOfTransactions)->end->date->day,(*ListOfTransactions)->end->date->month,(*ListOfTransactions)->end->date->year,(*ListOfTransactions)->end->time->hour,(*ListOfTransactions)->end->time->minutes);

		if(max_tr_id<strlen((*ListOfTransactions)->end->transactionID)){
			max_tr_id=strlen((*ListOfTransactions)->end->transactionID);
		}

		if(checkForDuplicateTransactionID((*ListOfTransactions))==-1){
			printf("\n\nDuplicateTransactionID!!!\n\n\n");
			return -1;
		}
		if(strcmp((*ListOfTransactions)->end->senderWalletID,(*ListOfTransactions)->end->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios , EDW TERMATIZEI TO PROGRAMMA???
			printf("\n\nSender=Receiver!!!\n\n\n");
			return -1;
		}
		if(findUser(wallets,(*ListOfTransactions)->end->senderWalletID)==NULL || findUser(wallets,(*ListOfTransactions)->end->receiverWalletID)==NULL){//EDW TERMATIZEI TO PROGRAMMA???
			printf("\n\nSender or Receiver NOT FOUND!!!\n\n\n");
			return -1;
		}*/
		//MPOREI NA 8ELEI KAI ELEGXO MHN KAPOIA SYNALAGH EINAI META THN TWRINH WRA



		//paron_tr++;

		
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
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			tr->value=tr->value*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->date=malloc(sizeof(struct_date));
		tr->date->day=0;
		while(string[p]!='-'){//diabazw receiverWalletID
			tr->date->day=tr->date->day*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->month=0;
		while(string[p]!='-'){//diabazw receiverWalletID
			tr->date->month=tr->date->month*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->year=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			tr->date->year=tr->date->year*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->time=malloc(sizeof(struct_time));
		tr->time->hour=0;
		while(string[p]!=':'){//diabazw receiverWalletID
			tr->time->hour=tr->time->hour*10+string[p]-'0';
			p++;
		}
		p++;
		tr->time->minutes=0;
		while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//diabazw receiverWalletID
			tr->time->minutes=tr->time->minutes*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}

		//printf("%s %s %s %d %d-%d-%d %d:%d\n",tr->transactionID,tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
		//if(max_tr_id<strlen(tr->transactionID)){//to phga ligo pio katw
		//	max_tr_id=strlen(tr->transactionID);
		//}

		if(checkForDuplicateTransactionID((*ListOfTransactions))==-1){
			printf("\n\nDuplicateTransactionID!!!\n\n\n");
			return -1;
		}
		if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios , EDW TERMATIZEI TO PROGRAMMA???
			printf("\n\nSender=Receiver!!!\n\n\n");
			//return -1;
			free(tr);//
			continue;//
		}
		if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//EDW TERMATIZEI TO PROGRAMMA???
			printf("\n\nSender or Receiver NOT FOUND!!!\n\n\n");
			//return -1;
			free(tr);//
			continue;//
		}

		if(max_tr_id<strlen(tr->transactionID)){
			max_tr_id=strlen(tr->transactionID);
		}
		//MPOREI NA 8ELEI KAI ELEGXO MHN KAPOIA SYNALAGH EINAI META THN TWRINH WRA

		//metaferw to paron transaction ston pinaka me ta transaction
		//(*ArrayOfTransactions)->tr[paron_tr];

		if(executeTransaction(bitCoins,wallets,(*senderHashTable),(*receiverHashTable),arguments,tr)==0){//0 dld exei ektelestei me epitixia
			/////////////////////////
			//printf("AAAAAAAAAAAAAAAAA\n");
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
			//printf("BBBBBBBBB\n");
			/////////////////////////
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
	//return 0;
	return max_tr_id;//kanw return to mhkos tou pio megalou id , me strlen
}

int executeTransaction(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable *senderHashTable,HashTable *receiverHashTable,struct_arguments *arguments,transaction *tr){

		if(checkIfSenderHasEnoughBalance(wallets,tr)==-1){//mporei na mhn prepei na mpainei sthn lista me ta transactions
			printf("NOT ENOUGHT BALANCE!!!\n");
			return -1;
		}

		//spaw to bitcoin , den 8a spaw to bitCoin , alla 8a phgainw sto wallet tou sender kai 8a spaw ton kombo apo to bitCoin pou exei
		//oneWallet *sender;
		//sender=findUser(wallets,tr->senderWalletID);
		//sender->list->

		//ft
		int sender_offset=hash(tr->senderWalletID,arguments->senderHashtableNumOfEntries);
		int receiver_offset=hash(tr->receiverWalletID,arguments->receiverHashtableNumOfEntries);//EDW TO EIXA senderWalletID anti gia receiverWalletID

		//gia ton sender
		bucketNode *sender_temp;
		int sender_point_on_bucket;

		if(senderHashTable->buckets[sender_offset]==NULL){
			senderHashTable->buckets[sender_offset]=malloc(sizeof(bucketNode));
			senderHashTable->buckets[sender_offset]->arrayOfUsers=malloc(senderHashTable->numOfUsersPerBucket*sizeof(bucketElement));
			senderHashTable->buckets[sender_offset]->last_entry=0;
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].users=findUser(wallets,tr->senderWalletID);//sunarthsh pou epistrefei to deikth sto wallet
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
			//senderHashTable->buckets[sender_offset]->transactionList->withUser=
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].transactionList->tr=tr;
			senderHashTable->buckets[sender_offset]->arrayOfUsers[0].transactionList->next=NULL;
			senderHashTable->buckets[sender_offset]->next=NULL;
//printf("NULL senderHashTable->numOfUsersPerBucket=%d\n",senderHashTable->numOfUsersPerBucket);

			sender_temp=senderHashTable->buckets[sender_offset];
			sender_point_on_bucket=0;
		}
		else{
			//bucketNode *sender_temp;
			if(checkIfUserIsAlreadyInHashTable(senderHashTable,sender_offset,tr->senderWalletID,&sender_temp,&sender_point_on_bucket)){//na epistrefei kai to deikth ekei pou ton brhke
				transactionNode *temp;
				temp=sender_temp->arrayOfUsers[sender_point_on_bucket].transactionList;
				while(temp->next!=NULL){
					temp=temp->next;
				}
				temp->next=malloc(sizeof(transactionNode));
				//temp->next->withuser
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
					sender_temp->arrayOfUsers[0].users=findUser(wallets,tr->senderWalletID);//sunarthsh pou na epistrefei to deikth sto wallet
					sender_temp->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
					//sender_temp->transactionList->withUser=
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
					//sender_temp->transactionList->withUser=
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
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].users=findUser(wallets,tr->receiverWalletID);//sunarthsh pou epistrefei to deikth sto wallet
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].transactionList=malloc(sizeof(transactionNode));
			//receiverHashTable->buckets[receiver_offset]->transactionList->withUser=
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].transactionList->tr=tr;
			receiverHashTable->buckets[receiver_offset]->arrayOfUsers[0].transactionList->next=NULL;
			receiverHashTable->buckets[receiver_offset]->next=NULL;
//printf("NULL receiverHashTable->numOfUsersPerBucket=%d\n",receiverHashTable->numOfUsersPerBucket);

			receiver_temp=receiverHashTable->buckets[receiver_offset];
			receiver_point_on_bucket=0;
		}
		else{
			//bucketNode *receiver_temp;
			if(checkIfUserIsAlreadyInHashTable(receiverHashTable,receiver_offset,tr->receiverWalletID,&receiver_temp,&receiver_point_on_bucket)){
				transactionNode *temp;
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				while(temp->next!=NULL){
					temp=temp->next;
				}
				temp->next=malloc(sizeof(transactionNode));
				//temp->next->withuser
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
					//receiver_temp->transactionList->withUser=
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
					//receiver_temp->transactionList->withUser=
					receiver_temp->arrayOfUsers[lentry].transactionList->tr=tr;
					receiver_temp->arrayOfUsers[lentry].transactionList->next=NULL;

					receiver_point_on_bucket=lentry;
				}
			}
		}

		//blepw poia bitCoin exei o sender wste na kanw thn sunalagh, exw tsekarei oti o sender exei ta xrhmata pou xreiazontai
		//to last entry mou leei 
		///////////////////////printf("AAA %d %d %d %d\n",sender_offset,sender_point_on_bucket,receiver_offset,receiver_point_on_bucket);
		//ara exw ta 	sender_offset/receiver_offset ->se poio kouti apo to ht , MALLON AYTA DEN 8A TA XREIASTW
		//		sender_point_on_bucket/receiver_point_on_bucket ->se poio kouti mesa sto bucket
		//		sender_temp/receiver_temp ->deikth sto bucket pou anoikei o sender h o receiver antistoixa


		//ananewnw to balance tou sender kai tou receiver
		sender_temp->arrayOfUsers[sender_point_on_bucket].users->balance-=tr->value;
		receiver_temp->arrayOfUsers[receiver_point_on_bucket].users->balance+=tr->value;

		//paw kai spaw ta dentra , mporw na kanw kai traverse to dentro
		//bgazw ton kombo me to bit coin kai an exei upoloipo , dld den exw point sthn riza

		//usersBitCoinsNode *sender_bit_coin_list;
		//sender_bit_coin_list=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list;
		int remaining=tr->value;
		while(remaining>0){//metaferw ta bitcoins
			sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->tr=tr;//tou sundew to tr , TA FULLA DEN DEIXNOUN SE TR !!
			if(remaining<sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value){//afhnei upoloipo ston kombo ara 2 komboi
				//left
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left=malloc(sizeof(bitCoinIdTreeNode));//ekei ta edwse
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID=malloc((strlen(tr->receiverWalletID)+1)*sizeof(char));
				strcpy(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID,tr->receiverWalletID);
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->value=remaining;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->tr=NULL;////
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->left=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->right=NULL;

				insertList(&(receiver_temp->arrayOfUsers[receiver_point_on_bucket].users->list),sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left);

				//right
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right=malloc(sizeof(bitCoinIdTreeNode));
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->walletID=malloc((strlen(tr->senderWalletID)+1)*sizeof(char));
				strcpy(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->walletID,tr->senderWalletID);
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->value=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value-remaining;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->tr=NULL;////
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->left=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right->right=NULL;

				insertList(&(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list),sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->right);
			}
			else{//ton xreiazomaste olo , enan kombo , (gia tipikous logous ton left)
				//left
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left=malloc(sizeof(bitCoinIdTreeNode));//ekei ta edwse
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID=malloc((strlen(tr->receiverWalletID)+1)*sizeof(char));
				strcpy(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->walletID,tr->receiverWalletID);
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->value=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->tr=NULL;////
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->left=NULL;
				sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left->right=NULL;

				insertList(&(receiver_temp->arrayOfUsers[receiver_point_on_bucket].users->list),sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->left);

			}

			remaining-=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->treenode->value;
			//printf("remaining=%d\n",remaining);

			//numOfTransactions++
			/*if(remaining<=0 || sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->offsetOfBitCoin!=sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->next->offsetOfBitCoin){
			bitCoins->array[sender_temp->arrayOfUsers[sender_point_on_bucket].users->list->offsetOfBitCoin].numOfTransactions++;
			}*/

			//digrafw ton kombo apo thn lista kai bazw ta upoloipa, na ta bazw panw kai na bgazw thn "riza edw" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			deleteList_node(&(sender_temp->arrayOfUsers[sender_point_on_bucket].users->list));
		}

	return 0;
}

int checkIfSenderHasEnoughBalance(struct_wallets *wallets,transaction *tr){
	for(int i=0;i<wallets->size;i++){
		if(strcmp(wallets->users[i].walletID,tr->senderWalletID)==0){//psaxnw ton xrhsth
			//printf("ASAAAAAAAAAAA\n");
			if(wallets->users[i].balance>=tr->value){
				//printf("YES\n");
				return 0;
			}
			else{
				//printf("NO\n");
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
	return NULL;
}

onebitCoinId *findBitCoin(bitCoinIdArray *bitCoins,char *str){
	for(int i=0;i<bitCoins->size;i++){
		if(strcmp(bitCoins->array[i].bitcoinid,str)==0){//psaxnw ton xrhsth
			return &(bitCoins->array[i]);
		}
	}
	return NULL;
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
		/*while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//pernaw to ID
			p++;
		}
		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}*/
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

		//////////////////
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
		//p=start;

		if(flag_date==2 && flag_time==1){//diabaseta katw
			p=start;
		}
		else if(flag_date==0 && flag_time==0){//dinw thn twrinh wra
			//https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program
			time_t t=time(NULL);
			struct tm tm=*localtime(&t);

			printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

			tr->date=malloc(sizeof(struct_date));
			tr->date->year=tm.tm_year+1900;
			tr->date->month=tm.tm_mon+1;
			tr->date->day=tm.tm_mday;
			tr->time=malloc(sizeof(struct_time));
			tr->time->hour=tm.tm_hour;
			tr->time->minutes=tm.tm_min;

			printf("RTY %s %s %d %d-%d-%d %d:%d\n",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);

			return tr;
		}
		else{//exei dwsei la8os px mono time
			return NULL;
		}
		
		//////////////////

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->date=malloc(sizeof(struct_date));
		tr->date->day=0;
		while(string[p]!='-'){//diabazw receiverWalletID
			tr->date->day=tr->date->day*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->month=0;
		while(string[p]!='-'){//diabazw receiverWalletID
			tr->date->month=tr->date->month*10+string[p]-'0';
			p++;
		}
		p++;
		tr->date->year=0;
		while(string[p]!=' ' && string[p]!='\t'){//diabazw receiverWalletID
			tr->date->year=tr->date->year*10+string[p]-'0';
			p++;
		}

		while(string[p]==' ' || string[p]=='\t' || string[p]=='\n'){//pernaw ta kena
			p++;
		}
		tr->time=malloc(sizeof(struct_time));
		tr->time->hour=0;
		while(string[p]!=':'){//diabazw receiverWalletID
			tr->time->hour=tr->time->hour*10+string[p]-'0';
			p++;
		}
		p++;
		tr->time->minutes=0;
		//while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//diabazw receiverWalletID
		while(p<end && string[p]>='0' && string[p]<='9'){
			tr->time->minutes=tr->time->minutes*10+string[p]-'0';
			p++;
		}

		printf("break %s %s %d %d-%d-%d %d:%d\n",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
		if(check_time(tr,ListOfTransactions)==1){
			printf("invalid time of transaction\n");
			free(tr->date);
			free(tr->time);
			free(tr);
			return NULL;
		}
		else{
			return tr;
		}
		/*if(executeTransaction(bitCoins,wallets,(*senderHashTable),(*receiverHashTable),arguments,tr)==-1){//mallon den xreiazetai if auth h sunarthsh na to bgalw !!!!!!!!!!!!!!!!!!!!!!!!!!!!
			return -1;
		}*/
}

int checkForDuplicateTransactionID(listOfTransactions *lot){//to point einai pou balame to teleutaio stoixeio
	if(lot->start==lot->end){
		//printf("PRWTO STHN LISTA\n");
		return 0;//pali, an einai mono 1 den to elegxw
	}
	transaction *temp;
	temp=lot->start;
	while(temp->next!=NULL){
		if(strcmp(lot->end->transactionID,temp->transactionID)==0){
				//printf("\n\nDUPLICAT USERNAME!!!\n\n\n");
				return -1;
		}
		temp=temp->next;
	}
	return 0;
}

int checkIfUserIsAlreadyInHashTable(HashTable *ht,int offset,char *str,bucketNode **temp_to_bucket,int *point_on_bucket){
	bucketNode *temp;
	temp=ht->buckets[offset];
	while(temp!=NULL){
		for(int i=0;i<=temp->last_entry;i++){
			if(strcmp(temp->arrayOfUsers[i].users->walletID,str)==0){
				//printf("O %s htan apo prin sto hash\n",temp->arrayOfUsers[i].users->walletID);
				(*temp_to_bucket)=temp;
				*point_on_bucket=i;
				//sleep(5);
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

int findUnspent(bitCoinIdTreeNode *node){
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
		//}
	}
}




int check_date(transaction *tr,struct_time time1,struct_time time2,struct_date date1,struct_date date2,int flag_time,int flag_date){
	if(flag_date==4){
		int qaz_tr=tr->date->year*10000+tr->date->month*100+tr->date->day;
		int qaz_1=date1.year*10000+date1.month*100+date1.day;
		int qaz_2=date2.year*10000+date2.month*100+date2.day;

		printf("ZZZZ   %d %d %d\n",qaz_tr,qaz_1,qaz_2);

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

	/*if(flag_date==4){
		if(tr->date->year > date1.year && tr->date->year < date2.year){
			return 1;
		}
		else if(tr->date->year == date1.year){
			if(tr->date->month > date1.month){
				return 1;
			}
			else if(tr->date->month == date1.month){
				if(tr->date->day > date1.day){
					return 1;
				}
				else if(tr->date->day == date1.day){
					if(flag_time==2){
						if(tr->time->hour > time1.hour){
							return 1;
						}
						else if(tr->time->hour == time1.hour){
							if(tr->time->minutes >= time1.minutes){
								return 1;
							}
						}
					}
					else{
						return 1;
					}
				}
			}
		}
		else if(tr->date->year == date2.year){
			if(tr->date->month < date2.month){
				return 1;
			}
			else if(tr->date->month == date2.month){
				if(tr->date->day < date2.day){
					return 1;
				}
				else if(tr->date->day == date2.day){
					if(flag_time==2){
						if(tr->time->hour < time2.hour){
							return 1;
						}
						else if(tr->time->hour == time2.hour){
							if(tr->time->minutes <= time2.minutes){
								return 1;
							}
						}
					}
					else{
						return 1;
					}
				}
			}
		}
	}
	else if(flag_time==2){
		if(tr->time->hour > time1.hour && tr->time->hour < time2.hour){
			return 1;
		}
		else if(tr->time->hour == time1.hour){
			if(tr->time->minutes >= time1.minutes){
				return 1;
			}
		}
		else if(tr->time->hour == time2.hour){
			if(tr->time->minutes <= time2.minutes){
				return 1;
			}
		}
	}
	return 0;*/
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
	if(ListOfTransactions->end==NULL){//einai adeio opote 
		return 0;
	}

	//time_t t=time(NULL);
	//struct tm tm=*localtime(&t);
	//printf("check time now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

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





