#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../header-files/functions.h"

int main(int argc,char **argv){
	struct_arguments *arguments;
	if(takeArguments(&arguments,argc,argv)==-1){
		return 1;
	}
////
printf("arguments->bitCoinBalanceFile\t\t= %s\n",arguments->bitCoinBalanceFile);
printf("arguments->transactionsFile\t\t= %s\n",arguments->transactionsFile);
printf("arguments->bitCoinValue\t\t\t= %d\n",arguments->bitCoinValue);
printf("arguments->senderHashtableNumOfEntries\t= %d\n",arguments->senderHashtableNumOfEntries);
printf("arguments->receiverHashtableNumOfEntries= %d\n",arguments->receiverHashtableNumOfEntries);
printf("arguments->bucketSize\t\t\t= %d\n",arguments->bucketSize);
////

	bitCoinIdArray *bitCoins;
	struct_wallets *wallets;
	if(takeData_BitCoinBalanceFile(&bitCoins,&wallets,arguments)==-1){
		return 2;
	}

////
printf("\n\nWALLETS with wallets->size=%d\n",wallets->size);
for(int i=0;i<wallets->size;i++){
	printf("wallets->users[%d].walletID=%s... wallets->users[i].balance=%d\n\t",i,wallets->users[i].walletID,wallets->users[i].balance);
	printList(wallets->users[i].list);
}

printf("\n\nBITCOINS with bitCoins->size=%d\n",bitCoins->size);
for(int i=0;i<bitCoins->size;i++){
	printf("bitCoins->array[%d].bitcoinid=%s\n",i,bitCoins->array[i].bitcoinid);
	printf("\tbitCoins->array[i].pointTree->walletID=%s\n\tbitCoins->array[i].pointTree->value=%d\n\tbitCoins->array[i].pointTree->left=%p\n\tbitCoins->array[i].pointTree->right%p\n",bitCoins->array[i].pointTree->walletID,bitCoins->array[i].pointTree->value,bitCoins->array[i].pointTree->left,bitCoins->array[i].pointTree->right);
}
////
	HashTable *senderHashTable,*receiverHashTable;
	//arrayOfTransactions *ArrayOfTransactions;
	listOfTransactions *ListOfTransactions;
	int max_tr_id;
	int next_tr_id=1;
	max_tr_id=takeData_TransactionsFile(bitCoins,wallets,&senderHashTable,&receiverHashTable,&ListOfTransactions,arguments);
	//if(takeData_TransactionsFile(bitCoins,wallets,&senderHashTable,&receiverHashTable,&ListOfTransactions,arguments)==-1){
	if(max_tr_id==-1){
		return 3;
	}

	while(max_tr_id>0){//dld ta nea id 8a arxizoun apo to 10^max_tr_id
		next_tr_id=next_tr_id*10;
		max_tr_id--;
	}

//////
/*printf("TRANSACTION from file with size=%d\n",ArrayOfTransactions->size);
for(int i=0;i<ArrayOfTransactions->size;i++){
	printf("%s %s %s %d %d-%d-%d %d:%d\n",ArrayOfTransactions->tr[i].transactionID,ArrayOfTransactions->tr[i].senderWalletID,ArrayOfTransactions->tr[i].receiverWalletID,ArrayOfTransactions->tr[i].value,ArrayOfTransactions->tr[i].date->day,ArrayOfTransactions->tr[i].date->month,ArrayOfTransactions->tr[i].date->year,ArrayOfTransactions->tr[i].time->hour,ArrayOfTransactions->tr[i].time->minutes);
}*/
transaction *temptr;
temptr=ListOfTransactions->start;
while(temptr!=NULL){
	printf("%s %s %s %d %d-%d-%d %d:%d\n",temptr->transactionID,temptr->senderWalletID,temptr->receiverWalletID,temptr->value,temptr->date->day,temptr->date->month,temptr->date->year,temptr->time->hour,temptr->time->minutes);
	temptr=temptr->next;
}


printf("\n\nWALLETS with wallets->size=%d\n",wallets->size);
for(int i=0;i<wallets->size;i++){
	printf("wallets->users[%d].walletID=%s... wallets->users[i].balance=%d\n\t",i,wallets->users[i].walletID,wallets->users[i].balance);
	printList(wallets->users[i].list);
}

for(int i=0;i<bitCoins->size;i++){
	printf("id=%s\n",bitCoins->array[i].bitcoinid);
	printTree(bitCoins->array[i].pointTree);
	printf("----------------------\n");
}
/////

	//diabazw input tou xrhsth
	char *command;
	char *line;
	int line_size=2048;
	command=malloc(20*sizeof(char));
	line=malloc(line_size*sizeof(char));
	strcpy(command,"");//wste na mpei sigoura sto while
	printf("\n\n");
	while(strcmp(command,"exit")!=0){
		printf("/");

		memset(line,'\0',2048);
		fgets(line,line_size,stdin);
		line[strlen(line)-1]='\0';

		memset(command,'\0',20);
		int i=0;
		while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
			i++;
		}
		int j=0;
		while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//command
			command[j]=line[i];
			i++;
			j++;
		}

		printf("line=%s\ncommand=%s\n",line,command);
		printf("\n\n\nnext_tr_id=%d\n\n\n",next_tr_id);

		if(strcmp(command,"requestTransaction")==0){
			transaction *tr;
			tr=breakTransaction(line,i,strlen(line),ListOfTransactions);
			if(tr==NULL){
				continue;
			}
			if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios , EDW TERMATIZEI TO PROGRAMMA???
				printf("\n\nSender=Receiver!!!\n\n\n");
				free(tr);
				continue;
			}
			if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//EDW TERMATIZEI TO PROGRAMMA???
				printf("\n\nSender or Receiver NOT FOUND!!!\n\n\n");
				free(tr);
				continue;
			}
			//MPOREI NA 8ELEI KAI ELEGXO MHN KAPOIA SYNALAGH EINAI META THN TWRINH WRA
			
			if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
				/////////////////////////
				printf("AAAAAAAAAAAAAAAAA\n");
				if(ListOfTransactions->start==NULL){
					ListOfTransactions->start=tr;
					ListOfTransactions->start->next=NULL;
					ListOfTransactions->end=ListOfTransactions->start;
				}
				else{
					ListOfTransactions->end->next=tr;
					ListOfTransactions->end=ListOfTransactions->end->next;
					ListOfTransactions->end->next=NULL;
				}
				printf("BBBBBBBBB\n");

				//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
				ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
				sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
				next_tr_id++;
				printf("ListOfTransactions->end->transactionID=%s..\n",ListOfTransactions->end->transactionID);
				/////////////////////////
			}

			/*transaction *tr;
			//tr=malloc(sizeof(transaction));
			//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
			tr=breakTransaction(line,i,strlen(line));
			tr->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
			sprintf(tr->transactionID, "%d", next_tr_id);
			next_tr_id++;
			printf("tr->transactionID=%s\n",tr->transactionID);*/

			

			//executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,ListOfTransactions->end);
		}
		else if(strcmp(command,"requestTransactions")==0){
			transaction *tr;
			int numberOfTransactions=0;
			for(int qaz=i;qaz<strlen(line);qaz++){//command  // edw to eixa i !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if(line[qaz]==';'){
					numberOfTransactions++;
				}
				//qaz++;
			}
			printf("numberOfTransactions=%d\n",numberOfTransactions);
			//tr=malloc(numberOfTransactions*sizeof(transaction *));
			if(numberOfTransactions==0){//diabazw apo arxeio
				while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
					i++;
				}
				int start=i;
				while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
					i++;
				}
				char *filename;
				filename=malloc((i-start)*sizeof(char));
				memset(filename,'\0',i-start);
				strncpy(filename,&(line[start]),i-start);
				printf("filename=%s...\n",filename);

				//openFile
				FILE *f;
				f=fopen(filename,"r");
				fseek(f,0,SEEK_END);
				long fsize=ftell(f);
				fseek(f,0,SEEK_SET);
				char *string;
				string=malloc((fsize+1)*sizeof(char));
				fread(string,fsize,1,f);
				fclose(f);

				string[fsize]=0;//teleiwnei se allagh grammhs ,mporei na 8elei allagh!!!!!!!!!!!!!!!!!!!!!!!!
				printf("\n\nstring=%s...\n",string);

				for(int qaz=0;qaz<strlen(string);qaz++){//posa transactions exw
					if(string[qaz]==';'){
						numberOfTransactions++;
					}
				}
				i=0;
				for(int qaz=0;qaz<numberOfTransactions;qaz++){
					transaction *tr;
					int start=i;
					while(string[i]!=';'){
						i++;
					}
					tr=breakTransaction(string,start,i,ListOfTransactions);
/////////////////////////////////////////////////////////////////
					if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios , EDW TERMATIZEI TO PROGRAMMA???
						printf("\n\nSender=Receiver!!!\n\n\n");
						continue;
					}
					if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//EDW TERMATIZEI TO PROGRAMMA???
						printf("\n\nSender or Receiver NOT FOUND!!!\n\n\n");
						continue;
					}
					//MPOREI NA 8ELEI KAI ELEGXO MHN KAPOIA SYNALAGH EINAI META THN TWRINH WRA
					
					if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
						/////////////////////////
						printf("AAAAAAAAAAAAAAAAA\n");
						if(ListOfTransactions->start==NULL){
							ListOfTransactions->start=tr;
							ListOfTransactions->start->next=NULL;
							ListOfTransactions->end=ListOfTransactions->start;
						}
						else{
							ListOfTransactions->end->next=tr;
							ListOfTransactions->end=ListOfTransactions->end->next;
							ListOfTransactions->end->next=NULL;
						}
						printf("BBBBBBBBB\n");

						//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
						ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
						sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
						next_tr_id++;
						printf("ListOfTransactions->end->transactionID=%s..\n",ListOfTransactions->end->transactionID);
						/////////////////////////
					}
//////////////////////////////////////////////////////////////////
					i++;
				}
				free(filename);
				free(string);
			}
			else if(numberOfTransactions==1){//diabazw apo grammh entolwn
				//for(int qaz=0;qaz<numberOfTransactions;qaz++){
					int start=i;
					while(line[i]!=';'){
						i++;
					}
					tr=breakTransaction(line,start,i,ListOfTransactions);
					//i++;
/////////////////////////////////////////////////////////////////
					if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios , EDW TERMATIZEI TO PROGRAMMA???
						printf("\n\nSender=Receiver!!!\n\n\n");
						continue;
					}
					if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//EDW TERMATIZEI TO PROGRAMMA???
						printf("\n\nSender or Receiver NOT FOUND!!!\n\n\n");
						continue;
					}
					//MPOREI NA 8ELEI KAI ELEGXO MHN KAPOIA SYNALAGH EINAI META THN TWRINH WRA
					
					if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
						/////////////////////////
						printf("AAAAAAAAAAAAAAAAA\n");
						if(ListOfTransactions->start==NULL){
							ListOfTransactions->start=tr;
							ListOfTransactions->start->next=NULL;
							ListOfTransactions->end=ListOfTransactions->start;
						}
						else{
							ListOfTransactions->end->next=tr;
							ListOfTransactions->end=ListOfTransactions->end->next;
							ListOfTransactions->end->next=NULL;
						}
						printf("BBBBBBBBB\n");

						//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
						ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
						sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
						next_tr_id++;
						printf("ListOfTransactions->end->transactionID=%s..\n",ListOfTransactions->end->transactionID);
						/////////////////////////
					}
//////////////////////////////////////////////////////////////////
				//}
				//memset(command,'\0',20);
				while(1){
					memset(line,'\0',2048);
					fgets(line,line_size,stdin);
					line[strlen(line)-1]='\0';

					memset(command,'\0',20);
					int i=0;
					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}
					int j=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//command
						command[j]=line[i];
						i++;
						j++;
					}

					if(strcmp(command,"end")==0){
						break;
					}
					else{
						int start=i;
						while(line[i]!=';'){
							i++;
						}
						tr=breakTransaction(line,0,i,ListOfTransactions);
/////////////////////////////////////////////////////////////////
						if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios , EDW TERMATIZEI TO PROGRAMMA???
							printf("\n\nSender=Receiver!!!\n\n\n");
							continue;
						}
						if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//EDW TERMATIZEI TO PROGRAMMA???
							printf("\n\nSender or Receiver NOT FOUND!!!\n\n\n");
							continue;
						}
						//MPOREI NA 8ELEI KAI ELEGXO MHN KAPOIA SYNALAGH EINAI META THN TWRINH WRA
						
						if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
							/////////////////////////
							printf("AAAAAAAAAAAAAAAAA\n");
							if(ListOfTransactions->start==NULL){
								ListOfTransactions->start=tr;
								ListOfTransactions->start->next=NULL;
								ListOfTransactions->end=ListOfTransactions->start;
							}
							else{
								ListOfTransactions->end->next=tr;
								ListOfTransactions->end=ListOfTransactions->end->next;
								ListOfTransactions->end->next=NULL;
							}
							printf("BBBBBBBBB\n");

							//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
							ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
							sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
							next_tr_id++;
							printf("ListOfTransactions->end->transactionID=%s..\n",ListOfTransactions->end->transactionID);
							/////////////////////////
						}
//////////////////////////////////////////////////////////////////
					}
				}
			}
			else{
				printf("wrong format\n");
			}
		}
		else if(strcmp(command,"findEarnings")==0){
			while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
				i++;
			}
			int start=i;
			while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
				i++;
			}
			char *walletID;
			walletID=malloc((i-start)*sizeof(char));
			//memset(walletID,'\0',i-start);
			strncpy(walletID,&(line[start]),i-start);
			walletID[i-start]=0;
			printf("walletID=%s...\n",walletID);

			start=i;
			int flag=0,flag_time=0,flag_date=0;
			while(i<strlen(line)){//elegxo an uparxei tpt allo
				if(line[i]!=' ' && line[i]!='\t' && line[i]!='\0' && line[i]!='\n'){
					flag=1;
					//break;
				}
				if(line[i]==':'){
					flag_time++;
				}
				if(line[i]=='-'){
					flag_date++;
				}

				i++;
			}
			if(flag_time!=2 && flag_time!=0){
				printf("WRONG FORMAT!!!\n");
				continue;
			}
			if(flag_date!=4 && flag_date!=0){
				printf("WRONG FORMAT!!!\n");
				continue;
			}
			if(flag==1){//uparxei euros xronou
				struct_date date1,date2;
				struct_time time1,time2;

				i=start;

				///////////
				if(flag_time==2){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}
					time1.hour=0;
					while(line[i]!=':'){
						time1.hour=time1.hour*10+line[i]-'0';
						i++;
					}
					i++;
					time1.minutes=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
						time1.minutes=time1.minutes*10+line[i]-'0';
						i++;
					}

				}
				////////////
				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date1.day=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date1.day=date1.day*10+line[i]-'0';
						i++;
					}
					i++;
					date1.month=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date1.month=date1.month*10+line[i]-'0';
						i++;
					}
					i++;
					date1.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//an sthn arxh exei kena
						date1.year=date1.year*10+line[i]-'0';
						i++;
					}

				}
				//gia thn deuterh hmerominia

				///////////
				if(flag_time==2){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}
					time2.hour=0;
					while(line[i]!=':'){//an sthn arxh exei kena
						time2.hour=time2.hour*10+line[i]-'0';
						i++;
					}
					i++;
					time2.minutes=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//an sthn arxh exei kena
						time2.minutes=time2.minutes*10+line[i]-'0';
						i++;
					}


				}
				////////////
				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date2.day=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date2.day=date2.day*10+line[i]-'0';
						i++;
					}
					i++;
					date2.month=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date2.month=date2.month*10+line[i]-'0';
						i++;
					}
					i++;
					date2.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//an sthn arxh exei kena
						date2.year=date2.year*10+line[i]-'0';
						i++;
					}

				}

				if(flag_time==2 && flag_date==4){
					printf("RANGE %d:%d %d-%d-%d   %d:%d %d-%d-%d\n",time1.hour,time1.minutes,date1.day,date1.month,date1.year,time2.hour,time2.minutes,date2.day,date2.month,date2.year);
				}
				else if(flag_date==4){
					printf("RANGE %d-%d-%d %d-%d-%d\n",date1.day,date1.month,date1.year,date2.day,date2.month,date2.year);
				}
				else{
					printf("RANGE %d:%d %d:%d\n",time1.hour,time1.minutes,time2.hour,time2.minutes);
				}

				//NA TO BALW SE MIA FUNCTION
				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->receiverHashtableNumOfEntries);//EDW MIA WHILE OPOU 8A PERNAEI TO TRANSACTION LIST
				//receiverHashTable->buckets[offset]->
				checkIfUserIsAlreadyInHashTable(receiverHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket);
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				printf("AAAAAAA\n");
				while(temp!=NULL){
					if(check_date(temp->tr,time1,time2,date1,date2,flag_time,flag_date)==1){
						printf("%s %s %s %d %d-%d-%d %d:%d\n",temp->tr->transactionID,temp->tr->senderWalletID,temp->tr->receiverWalletID,temp->tr->value,temp->tr->date->day,temp->tr->date->month,temp->tr->date->year,temp->tr->time->hour,temp->tr->time->minutes);
					}
					temp=temp->next;
				}
			}
			else{
				//NA TO BALW SE MIA FUNCTION
				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->receiverHashtableNumOfEntries);//EDW MIA WHILE OPOU 8A PERNAEI TO TRANSACTION LIST
				//receiverHashTable->buckets[offset]->
				checkIfUserIsAlreadyInHashTable(receiverHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket);
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				printf("AAAAAAA\n");
				while(temp!=NULL){
					printf("%s %s %s %d %d-%d-%d %d:%d\n",temp->tr->transactionID,temp->tr->senderWalletID,temp->tr->receiverWalletID,temp->tr->value,temp->tr->date->day,temp->tr->date->month,temp->tr->date->year,temp->tr->time->hour,temp->tr->time->minutes);
					temp=temp->next;
				}
			}
			free(walletID);
		}
		else if(strcmp(command,"findPayments")==0){
			while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
				i++;
			}
			int start=i;
			while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
				i++;
			}
			char *walletID;
			walletID=malloc((i-start)*sizeof(char));
			strncpy(walletID,&(line[start]),i-start);
			walletID[i-start]=0;//AYTO NA TO BALW KAI ALLOU EDW STIS ENTOLES
			printf("walletID=%s...\n",walletID);

			start=i;
			int flag=0,flag_time=0,flag_date=0;
			while(i<strlen(line)){//elegxo an uparxei tpt allo
				if(line[i]!=' ' && line[i]!='\t' && line[i]!='\0' && line[i]!='\n'){
					flag=1;
					//break;
				}
				if(line[i]==':'){
					flag_time++;
				}
				if(line[i]=='-'){
					flag_date++;
				}

				i++;
			}
			if(flag_time!=2 && flag_time!=0){
				printf("WRONG FORMAT!!!\n");
				continue;
			}
			if(flag_date!=4 && flag_date!=0){
				printf("WRONG FORMAT!!!\n");
				continue;
			}
			if(flag==1){//uparxei euros xronou
				struct_date date1,date2;
				struct_time time1,time2;

				i=start;

				///////////
				if(flag_time==2){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}
					time1.hour=0;
					while(line[i]!=':'){
						time1.hour=time1.hour*10+line[i]-'0';
						i++;
					}
					i++;
					time1.minutes=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
						time1.minutes=time1.minutes*10+line[i]-'0';
						i++;
					}

				}
				////////////
				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date1.day=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date1.day=date1.day*10+line[i]-'0';
						i++;
					}
					i++;
					date1.month=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date1.month=date1.month*10+line[i]-'0';
						i++;
					}
					i++;
					date1.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//an sthn arxh exei kena
						date1.year=date1.year*10+line[i]-'0';
						i++;
					}

				}
				//gia thn deuterh hmerominia

				///////////
				if(flag_time==2){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}
					time2.hour=0;
					while(line[i]!=':'){//an sthn arxh exei kena
						time2.hour=time2.hour*10+line[i]-'0';
						i++;
					}
					i++;
					time2.minutes=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//an sthn arxh exei kena
						time2.minutes=time2.minutes*10+line[i]-'0';
						i++;
					}


				}
				////////////
				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date2.day=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date2.day=date2.day*10+line[i]-'0';
						i++;
					}
					i++;
					date2.month=0;
					while(line[i]!='-'){//an sthn arxh exei kena
						date2.month=date2.month*10+line[i]-'0';
						i++;
					}
					i++;
					date2.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//an sthn arxh exei kena
						date2.year=date2.year*10+line[i]-'0';
						i++;
					}

				}

				if(flag_time==2 && flag_date==4){
					printf("RANGE %d:%d %d-%d-%d   %d:%d %d-%d-%d\n",time1.hour,time1.minutes,date1.day,date1.month,date1.year,time2.hour,time2.minutes,date2.day,date2.month,date2.year);
				}
				else if(flag_date==4){
					printf("RANGE %d-%d-%d %d-%d-%d\n",date1.day,date1.month,date1.year,date2.day,date2.month,date2.year);
				}
				else{
					printf("RANGE %d:%d %d:%d\n",time1.hour,time1.minutes,time2.hour,time2.minutes);
				}

				//NA TO BALW SE MIA FUNCTION , ALLAGH STHS METABLHTES
				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->senderHashtableNumOfEntries);//EDW MIA WHILE OPOU 8A PERNAEI TO TRANSACTION LIST
				//receiverHashTable->buckets[offset]->
				checkIfUserIsAlreadyInHashTable(senderHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket);
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				printf("AAAAAAA\n");
				while(temp!=NULL){
					if(check_date(temp->tr,time1,time2,date1,date2,flag_time,flag_date)==1){
						printf("%s %s %s %d %d-%d-%d %d:%d\n",temp->tr->transactionID,temp->tr->senderWalletID,temp->tr->receiverWalletID,temp->tr->value,temp->tr->date->day,temp->tr->date->month,temp->tr->date->year,temp->tr->time->hour,temp->tr->time->minutes);
					}
					temp=temp->next;
				}
			}
			else{
				//NA TO BALW SE MIA FUNCTION , ALLAGH STHS METABLHTES
				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->senderHashtableNumOfEntries);//EDW MIA WHILE OPOU 8A PERNAEI TO TRANSACTION LIST
				//receiverHashTable->buckets[offset]->
				checkIfUserIsAlreadyInHashTable(senderHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket);
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				printf("AAAAAAA\n");
				while(temp!=NULL){
					printf("%s %s %s %d %d-%d-%d %d:%d\n",temp->tr->transactionID,temp->tr->senderWalletID,temp->tr->receiverWalletID,temp->tr->value,temp->tr->date->day,temp->tr->date->month,temp->tr->date->year,temp->tr->time->hour,temp->tr->time->minutes);
					temp=temp->next;
				}
			}
			free(walletID);
		}
		else if(strcmp(command,"walletStatus")==0){
			while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
				i++;
			}
			int start=i;
			while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
				i++;
			}
			char *walletID;
			walletID=malloc((i-start)*sizeof(char));
			//memset(walletID,'\0',i-start);
			strncpy(walletID,&(line[start]),i-start);
			walletID[i-start]=0;
			printf("walletID=%s...\n",walletID);

			oneWallet *wallet;
			wallet=findUser(wallets,walletID);
			printf("User: %s has %d$\n",wallet->walletID,wallet->balance);
			free(walletID);
		}
		else if(strcmp(command,"bitCoinStatus")==0){
			while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
				i++;
			}
			int start=i;
			while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
				i++;
			}
			char *bitCoinID;
			bitCoinID=malloc((i-start)*sizeof(char));
			//memset(bitCoinID,'\0',i-start);
			strncpy(bitCoinID,&(line[start]),i-start);
			bitCoinID[i-start]=0;//AYTO NA TO BALW PANTOU !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			printf("bitCoinID=%s...\n",bitCoinID);

			onebitCoinId *bitcoin;
			bitcoin=findBitCoin(bitCoins,bitCoinID);
			printf("bitCoinID: %s \n\t-starting value = %d$\n\t-number of times used in a transaction = %d\n\t-unspent = %d$\n\n",bitCoinID,bitcoin->pointTree->value,bitcoin->numOfTransactions,findUnspent(bitcoin->pointTree));
			free(bitCoinID);
		}
		else if(strcmp(command,"traceCoin")==0){
			while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
				i++;
			}
			int start=i;
			while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
				i++;
			}
			char *bitCoinID;
			bitCoinID=malloc((i-start)*sizeof(char));
			//memset(bitCoinID,'\0',i-start);
			strncpy(bitCoinID,&(line[start]),i-start);
			printf("bitCoinID=%s...\n",bitCoinID);
			bitCoinID[i-start]=0;//

			transaction *tr;
			tr=ListOfTransactions->start;
			while(tr!=NULL){//mhdenizw ta unused
				tr->unused=0;
				tr=tr->next;
			}

			onebitCoinId *temp_bitcoin;
			temp_bitcoin=findBitCoin(bitCoins,bitCoinID);
			printTransactionHistory(temp_bitcoin->pointTree);

			free(bitCoinID);
		}
		else if(strcmp(command,"exit")==0){
			//free bitCoins
			for(i=0;i<bitCoins->size;i++){
				freeBitCoinTree(bitCoins->array[i].pointTree);
				free(bitCoins->array[i].bitcoinid);
			}
			free(bitCoins->array);
			free(bitCoins);

			//free wallets;
			for(i=0;i<wallets->size;i++){
				freeWalletUsersList(wallets->users[i].list);
				free(wallets->users[i].walletID);
			}
			free(wallets->users);
			free(wallets);

			//free hash tables
			for(i=0;i<arguments->senderHashtableNumOfEntries;i++){
				freeHashTableBuckets(senderHashTable->buckets[i]);
			}
			free(senderHashTable->buckets);
			free(senderHashTable);

			for(i=0;i<arguments->receiverHashtableNumOfEntries;i++){
				freeHashTableBuckets(receiverHashTable->buckets[i]);
			}
			free(receiverHashTable->buckets);
			free(receiverHashTable);

			freeListOfTransactions(ListOfTransactions->start);
			free(ListOfTransactions);

			free(line);
			free(arguments->bitCoinBalanceFile);
			free(arguments->transactionsFile);
			free(arguments);

			printf("EXIT\n");
		}
		else{
			printf("INVALID COMMAND!!!\n");
		}
	}
	free(command);
}











