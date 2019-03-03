#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
	printf("\n\nstring=%s...\n",string);

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
	printf("(*bitCoins)->size=%d (*wallets)->size=%d\n",(*bitCoins)->size,(*wallets)->size);

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
		printf("(*wallets)->users[walletcount].walletID=%s...\n",(*wallets)->users[walletcount].walletID);
		if(checkForDuplicateWalletID((*wallets),walletcount)==-1){
			printf("\n\nDuplicateWalletID!!!\n\n\n");
			return -1;
		}
		(*wallets)->users[walletcount].balance=0;
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
			printf("(*bitCoins)->array[bitcoincount].bitcoinid=%s...\n",(*bitCoins)->array[bitcoincount].bitcoinid);
			(*bitCoins)->array[bitcoincount].pointTree=malloc(sizeof(bitCoinIdTreeNode));
			(*bitCoins)->array[bitcoincount].pointTree->walletID=malloc((strlen((*wallets)->users[walletcount].walletID)+1)*sizeof(char));
			strncpy((*bitCoins)->array[bitcoincount].pointTree->walletID,(*wallets)->users[walletcount].walletID,strlen((*wallets)->users[walletcount].walletID)+1);
			(*bitCoins)->array[bitcoincount].pointTree->value=arguments->bitCoinValue;
			(*bitCoins)->array[bitcoincount].pointTree->left=NULL;
			(*bitCoins)->array[bitcoincount].pointTree->right=NULL;
			if(checkForDuplicatebitCoinID((*bitCoins),bitcoincount)==-1){
				printf("\n\nDuplicatebitCoinID!!!\n\n\n");
				return -1;
			}
			(*wallets)->users[walletcount].balance+=arguments->bitCoinValue;//pros8etw sto balance thn aksia tou bitcoin pou molis diabasa
			insertList(&((*wallets)->users[walletcount].list),arguments->bitCoinValue,bitcoincount);
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
		printf(" %2d %2d //",temp->usersPartOfBitCoin,temp->offsetOfBitCoin);
		temp=temp->next;
	}
	printf("\n");
}

void insertList(usersBitCoinsNode **list,int value,int offset){
	usersBitCoinsNode *temp;
	if((*list)==NULL){
		(*list)=malloc(sizeof(usersBitCoinsNode));
		(*list)->usersPartOfBitCoin=value;
		(*list)->offsetOfBitCoin=offset;
		(*list)->next=NULL;
	}
	else{
		temp=(*list);
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next=malloc(sizeof(usersBitCoinsNode));
		temp->next->usersPartOfBitCoin=value;
		temp->next->offsetOfBitCoin=offset;
		temp->next->next=NULL;
	}
}

/*
void deleteList_node(listNode **list,int num){//delete ton tade kombo
	listNode *temp,*prev,*del;

	if(num==1){//diargafw thn riza
		if((*list)!=NULL){
			del=(*list);
			(*list)=(*list)->next;
			free(del);
		}
	}
	else if(num>=2){
		temp=(*list);
		while(temp!=NULL){
			if(num==1){
				prev->next=temp->next;
				free(temp);
				return;
			}
			num--;
			prev=temp;
			temp=temp->next;
		}
	}
}
*/

/////////////////////////

int takeData_TransactionsFile(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable **senderHashTable,HashTable **receiverHashTable,struct_arguments *arguments){
	int i;
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
	printf("\n\nstring=%s...\n",string);
	

	//ftiaxnw ta hashTables , sthn arxh ola ta buckets tous einai NULL kai den uparxei kanena bucketNode
	(*senderHashTable)=malloc(sizeof(HashTable));
	(*senderHashTable)->numOfUsersPerBucket=(arguments->bucketSize-sizeof(transactionNode *)-sizeof(bucketNode *))/sizeof(oneWallet *);
	//printf("arguments->bucketSize=%d sizeof(transactionNode *)=%ld sizeof(bucketNode *)=%ld sizeof(oneWallet *)=%ld (*senderHashTable)->numOfUsersPerBucket=%d\n",arguments->bucketSize,sizeof(transactionNode *),sizeof(bucketNode *),sizeof(oneWallet *),(*senderHashTable)->numOfUsersPerBucket);
	(*senderHashTable)->buckets=malloc(arguments->senderHashtableNumOfEntries*sizeof(bucketNode *));
	for(i=0;i<arguments->senderHashtableNumOfEntries;i++){
		(*senderHashTable)->buckets[i]=NULL;
	}


	(*receiverHashTable)=malloc(sizeof(HashTable));
	(*receiverHashTable)->numOfUsersPerBucket=(arguments->bucketSize-sizeof(transactionNode *)-sizeof(bucketNode *))/sizeof(oneWallet *);
	//printf("arguments->bucketSize=%d sizeof(transactionNode *)=%ld sizeof(bucketNode *)=%ld sizeof(oneWallet *)=%ld (*receiverHashTable)->numOfUsersPerBucket=%d\n",arguments->bucketSize,sizeof(transactionNode *),sizeof(bucketNode *),sizeof(oneWallet *),(*receiverHashTable)->numOfUsersPerBucket);
	(*receiverHashTable)->buckets=malloc(arguments->receiverHashtableNumOfEntries*sizeof(bucketNode *));
	for(i=0;i<arguments->receiverHashtableNumOfEntries;i++){
		(*receiverHashTable)->buckets[i]=NULL;
	}


	//diabazw ta transactions
	int p=0;
	while(string[p]!=0){//8ELEI DIOR8WSH

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

		printf("%s %s %s %d %d-%d-%d %d:%d\n",tr->transactionID,tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);

		/*if(executeTransaction(bitCoins,wallets,(*senderHashTable),(*receiverHashTable),arguments,tr)==-1){//mallon den xreiazetai if auth h sunarthsh na to bgalw !!!!!!!!!!!!!!!!!!!!!!!!!!!!
			return -1;
		}*/
	}

	return 0;
}

/*int executeTransaction(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable *senderHashTable,HashTable *receiverHashTable,struct_arguments *arguments,transaction *tr){
		if(checkIfSenderHasEnoughBalance(wallets,tr)==-1){
			printf("NOT ENOUGHT BALANCE!!!\n");
			return 0;
		}

		//spaw to bitcoin
		oneWallet *sender;
		sender=findUser(wallets,tr->senderWalletID);
		sender->list->

		//ft
		int sender_offset=hash(tr->senderWalletID,arguments->senderHashtableNumOfEntries);
		int receiver_offset=hash(tr->senderWalletID,arguments->receiverHashtableNumOfEntries);

		if(senderHashTable->buckets[sender_offset]==NULL){
			senderHashTable->buckets[sender_offset]=malloc(sizeof(bucketNode));
			senderHashTable->buckets[sender_offset]->users=malloc(senderHashTable->numOfUsersPerBucket*sizeof(oneWallet *));
			senderHashTable->buckets[sender_offset]->last_entry=0;
			senderHashTable->buckets[sender_offset]->users[0]=findUser(wallets,tr->senderWalletID);//sunarthsh pou na epistrefei to deikth sto wallet
			senderHashTable->buckets[sender_offset]->transactionList=malloc(sizeof(transactionNode));
			//senderHashTable->buckets[sender_offset]->transactionList->withUser=
			senderHashTable->buckets[sender_offset]->transactionList->next=NULL;
			senderHashTable->buckets[sender_offset]->next=NULL;
		}
		else{
			bucketNode *temp;
			if(checkIfUserIsAlreadyInHashTable()){
								
			}
			else{//den uparxei sto hash table
				temp=senderHashTable->buckets[sender_offset];
				while(temp->last_entry+1>=senderHashTable->numOfUsersPerBucket && temp->next!=NULL){
					temp=temp->next;
				}

				if(temp->last_entry+1>=senderHashTable->numOfUsersPerBucket){//ftiaxnw neo bucket
					temp->next=malloc(sizeof(bucketNode));
					temp=temp->next;

					temp->users=malloc(senderHashTable->numOfUsersPerBucket*sizeof(oneWallet *));
					temp->last_entry=0;
					temp->users[0]=findUser(wallets,tr->senderWalletID);//sunarthsh pou na epistrefei to deikth sto wallet
					temp->transactionList=malloc(sizeof(transactionNode));
					//temp->transactionList->withUser=
					temp->transactionList->next=NULL;
					temp->next=NULL;
				}
				else{
					temp->last_entry++;
					int lentry=temp->last_entry;
					temp->users[lentry]=findUser(wallets,tr->senderWalletID);
					temp->transactionList=malloc(sizeof(transactionNode));
					//temp->transactionList->withUser=
					temp->transactionList->next=NULL;
				}
			}
		}
}*/

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

int hash(char *str,int mod){
	int i,sum=0,temp=strlen(str);

	for(i=0;i<temp;i++){
		sum+=str[i];
	}
	sum=sum%mod;
	return sum;
}

transaction *breakTransaction(char *string,int begin,int end){
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
		//while(string[p]!=' ' && string[p]!='\t' && string[p]!='\n'){//diabazw receiverWalletID
		while(p<end && string[p]>='0' && string[p]<='9'){
			tr->time->minutes=tr->time->minutes*10+string[p]-'0';
			p++;
		}

		printf("break %s %s %d %d-%d-%d %d:%d\n",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
		return tr;
		/*if(executeTransaction(bitCoins,wallets,(*senderHashTable),(*receiverHashTable),arguments,tr)==-1){//mallon den xreiazetai if auth h sunarthsh na to bgalw !!!!!!!!!!!!!!!!!!!!!!!!!!!!
			return -1;
		}*/
}
