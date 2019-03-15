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

	bitCoinIdArray *bitCoins;
	struct_wallets *wallets;
	if(takeData_BitCoinBalanceFile(&bitCoins,&wallets,arguments)==-1){
		return 2;
	}

	HashTable *senderHashTable,*receiverHashTable;
	listOfTransactions *ListOfTransactions;
	int max_tr_id;//posa chars einai to megalutero (se mhkos) id
	int next_tr_id=1;//etsi an px exw ws 3char ids ta kainourgia 8a einai apo 1000 kai panw
	max_tr_id=takeData_TransactionsFile(bitCoins,wallets,&senderHashTable,&receiverHashTable,&ListOfTransactions,arguments);
	if(max_tr_id==-1){
		return 3;
	}

	while(max_tr_id>0){//dld ta nea id 8a arxizoun apo to 10^max_tr_id
		next_tr_id=next_tr_id*10;
		max_tr_id--;
	}

	//diabazw input tou xrhsth
	char *command;
	char *line;
	int line_size=2048;
	command=malloc(20*sizeof(char));
	line=malloc(line_size*sizeof(char));
	strcpy(command,"");//wste na mpei sigoura sto while

	while(strcmp(command,"exit")!=0){
		printf("\n/");

		memset(line,'\0',2048);
		fgets(line,line_size,stdin);//diabazw thn entolh tou xrhsth
		line[strlen(line)-1]='\0';

		memset(command,'\0',20);
		int i=0;
		while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
			i++;
		}
		int j=0;
		while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//diabazw to command
			command[j]=line[i];
			i++;
			j++;
		}

		if(strcmp(command,"requestTransaction")==0){
			transaction *tr;
			tr=breakTransaction(line,i,strlen(line),ListOfTransactions);//sto break transaction stelnw apo to telos tou command kai meta wste na tou dwsw mono to transaction , auto ginetai me ta oria i kai strlen(line)
			if(tr==NULL){//an brhke kapoio la8os
				continue;
			}
			if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios
				printf("Error : Sender=Receiver!!!");
				free(tr);
				continue;
			}
			if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//elegxw oti uparxoun kai oi 2 xrhstes
				printf("Error : Sender or Receiver NOT FOUND!!!");
				free(tr);
				continue;
			}
			
			if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){//an to transaction ektelesthke me epitixia tote to pros8etw sthn lista , an kapoio gia kapoio logo den einai px not enough balance tote den to bazw
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

				//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
				ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
				sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
				next_tr_id++;
			}

		}
		else if(strcmp(command,"requestTransactions")==0){
			transaction *tr;
			int numberOfTransactions=0;
			for(int qaz=i;qaz<strlen(line);qaz++){//koitazw na dw an uparxei ';' dld an o xrhsths 8a plhktrologisei request h 8a mou dwsei arxeio
				if(line[qaz]==';'){
					numberOfTransactions++;
				}
			}
			if(numberOfTransactions==0){//diabazw apo arxeio
				while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
					i++;
				}
				int start=i;
				while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//diabazw to filename
					i++;
				}
				char *filename;
				filename=malloc((i-start)*sizeof(char));
				memset(filename,'\0',i-start);
				strncpy(filename,&(line[start]),i-start);
				filename[i-start]=0;

				//check if file exists
				//https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
				if(access(filename,F_OK)==-1){
					printf("File '%s' doesn't exist!!!\n",filename);
					//return -1;
					free(filename);
					continue;
				}
printf("AAAAAA\n");
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

				string[fsize]=0;

				for(int qaz=0;qaz<strlen(string);qaz++){//posa transactions exw
					if(string[qaz]==';'){
						numberOfTransactions++;
					}
				}
				i=0;
				for(int qaz=0;qaz<numberOfTransactions;qaz++){
					transaction *tr;
					int start=i;
					while(string[i]!=';'){//pairnw ta kommatia pou einai ta transactions
						i++;
					}
					i++;
					tr=breakTransaction(string,start,i-1,ListOfTransactions);
					if(tr==NULL){
						continue;
					}

					if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//elegxw an o sender kai o receiver einai o idios
						printf("transaction '%s %s %d %d-%d-%d %d:%d' is canceled because Sender=Receiver\n",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
						free(tr);
						continue;
					}
					if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){//elegxw oti uparxoun oi users
						printf("transaction '%s %s %d %d-%d-%d %d:%d' is canceled because Sender or Receiver NOT FOUND\n",tr->senderWalletID,tr->receiverWalletID,tr->value,tr->date->day,tr->date->month,tr->date->year,tr->time->hour,tr->time->minutes);
						free(tr);
						continue;
					}
					
					if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
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

						//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
						ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
						sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
						next_tr_id++;
					}

				}
				free(filename);
				free(string);
			}
			else if(numberOfTransactions==1){//diabazw apo grammh entolwn
				int start=i;
				while(line[i]!=';'){
					i++;
				}
				tr=breakTransaction(line,start,i,ListOfTransactions);
				if(tr!=NULL){
					if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){//omoiws elegxw
						printf("Error : Sender=Receiver");
						//continue;
					}
					else if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){
						printf("Error : Sender or Receiver NOT FOUND");
						//continue;
					}
					else if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
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

						//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
						ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
						sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
						next_tr_id++;
					}
				}
				//}
				//memset(command,'\0',20);
				printf("\tIf you don't want to make any more transactions , please type 'end'\n");
				while(1){//edw perimenoume apo to xrhsth ta upoloipa transaction pou 8elei
					printf("\t/");
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
						while(line[i]!=';'){
							i++;
						}
						tr=breakTransaction(line,0,i,ListOfTransactions);
						if(tr==NULL){
							continue;
						}

						if(strcmp(tr->senderWalletID,tr->receiverWalletID)==0){
							printf("\tError : Sender=Receiver\n");
							continue;
						}
						if(findUser(wallets,tr->senderWalletID)==NULL || findUser(wallets,tr->receiverWalletID)==NULL){
							printf("\tError Sender or Receiver NOT FOUND\n");
							continue;
						}
						
						if(executeTransaction(bitCoins,wallets,senderHashTable,receiverHashTable,arguments,tr)==0){
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

							//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
							ListOfTransactions->end->transactionID=malloc(countDigits(next_tr_id)*sizeof(char));
							sprintf(ListOfTransactions->end->transactionID, "%d", next_tr_id);
							next_tr_id++;
						}
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
			while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){//diabazw to walletID
				i++;
			}
			char *walletID;
			walletID=malloc((i-start+1)*sizeof(char));
			strncpy(walletID,&(line[start]),i-start);
			walletID[i-start]=0;

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
			if(flag_time!=2 && flag_time!=0){//an px exoume mono ena time opote mia fora ':'
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

				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date1.day=0;
					while(line[i]!='-'){
						date1.day=date1.day*10+line[i]-'0';
						i++;
					}
					i++;
					date1.month=0;
					while(line[i]!='-'){
						date1.month=date1.month*10+line[i]-'0';
						i++;
					}
					i++;
					date1.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
						date1.year=date1.year*10+line[i]-'0';
						i++;
					}

				}

				//gia thn deuterh hmerominia
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

				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->receiverHashtableNumOfEntries);

				if(checkIfUserIsAlreadyInHashTable(receiverHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket)==0){//an uparxei o xrhsths h function 8a epistrepsei ta receiver_temp kai receiver_point_on_bucket
					printf("User has never received money\n");
					continue;
				}
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;

				while(temp!=NULL){
					if(check_date(temp->tr,time1,time2,date1,date2,flag_time,flag_date)==1){
						printf("%s %s %s %d %d-%d-%d %d:%d\n",temp->tr->transactionID,temp->tr->senderWalletID,temp->tr->receiverWalletID,temp->tr->value,temp->tr->date->day,temp->tr->date->month,temp->tr->date->year,temp->tr->time->hour,temp->tr->time->minutes);
					}
					temp=temp->next;
				}
			}
			else{

				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->receiverHashtableNumOfEntries);
				if(checkIfUserIsAlreadyInHashTable(receiverHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket)==0){
					printf("User has never received money\n");
					continue;
				}				
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;

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
			walletID[i-start]=0;

			start=i;
			int flag=0,flag_time=0,flag_date=0;
			while(i<strlen(line)){//elegxo an uparxei tpt allo
				if(line[i]!=' ' && line[i]!='\t' && line[i]!='\0' && line[i]!='\n'){
					flag=1;
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

				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date1.day=0;
					while(line[i]!='-'){
						date1.day=date1.day*10+line[i]-'0';
						i++;
					}
					i++;
					date1.month=0;
					while(line[i]!='-'){
						date1.month=date1.month*10+line[i]-'0';
						i++;
					}
					i++;
					date1.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
						date1.year=date1.year*10+line[i]-'0';
						i++;
					}

				}
				//gia thn deuterh hmerominia

				if(flag_time==2){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}
					time2.hour=0;
					while(line[i]!=':'){
						time2.hour=time2.hour*10+line[i]-'0';
						i++;
					}
					i++;
					time2.minutes=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
						time2.minutes=time2.minutes*10+line[i]-'0';
						i++;
					}


				}

				if(flag_date==4){

					while(line[i]==' ' || line[i]=='\t' || line[i]=='\0'){//an sthn arxh exei kena
						i++;
					}

					date2.day=0;
					while(line[i]!='-'){
						date2.day=date2.day*10+line[i]-'0';
						i++;
					}
					i++;
					date2.month=0;
					while(line[i]!='-'){
						date2.month=date2.month*10+line[i]-'0';
						i++;
					}
					i++;
					date2.year=0;
					while(line[i]!=' ' && line[i]!='\t' && line[i]!='\0'){
						date2.year=date2.year*10+line[i]-'0';
						i++;
					}

				}

				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->senderHashtableNumOfEntries);
				if(checkIfUserIsAlreadyInHashTable(senderHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket)==0){
					printf("User has never send money\n");
					continue;
				}
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
				while(temp!=NULL){
					if(check_date(temp->tr,time1,time2,date1,date2,flag_time,flag_date)==1){
						printf("%s %s %s %d %d-%d-%d %d:%d\n",temp->tr->transactionID,temp->tr->senderWalletID,temp->tr->receiverWalletID,temp->tr->value,temp->tr->date->day,temp->tr->date->month,temp->tr->date->year,temp->tr->time->hour,temp->tr->time->minutes);
					}
					temp=temp->next;
				}
			}
			else{
				bucketNode *receiver_temp;
				int receiver_point_on_bucket;
				transactionNode *temp;
				int offset=hash(walletID,arguments->senderHashtableNumOfEntries);
				if(checkIfUserIsAlreadyInHashTable(senderHashTable,offset,walletID,&receiver_temp,&receiver_point_on_bucket)==0){
					printf("User has never send money\n");
					continue;
				}
				temp=receiver_temp->arrayOfUsers[receiver_point_on_bucket].transactionList;
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
			strncpy(walletID,&(line[start]),i-start);
			walletID[i-start]=0;

			oneWallet *wallet;
			wallet=findUser(wallets,walletID);
			if(wallet==NULL){
				printf("unknown username");
			}
			else{
				printf("User: %s has %d$\n",wallet->walletID,wallet->balance);
			}
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
			strncpy(bitCoinID,&(line[start]),i-start);
			bitCoinID[i-start]=0;
			transaction *tr;
			tr=ListOfTransactions->start;
			while(tr!=NULL){//mhdenizw ta unused
				tr->unused=0;
				tr=tr->next;
			}

			onebitCoinId *bitcoin;
			bitcoin=findBitCoin(bitCoins,bitCoinID);
			if(bitcoin==NULL){
				printf("unknown bitcoin");
			}
			else{
				printf("bitCoinID: %s \n\t-number of times used in a transaction = %d\n\t-unspent = %d$\n",bitCoinID,countTransactions(bitcoin->pointTree),findUnspent(bitcoin->pointTree));
			}
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
			strncpy(bitCoinID,&(line[start]),i-start);
			bitCoinID[i-start]=0;

			transaction *tr;
			tr=ListOfTransactions->start;
			while(tr!=NULL){//mhdenizw ta unused
				tr->unused=0;
				tr=tr->next;
			}

			onebitCoinId *temp_bitcoin;
			temp_bitcoin=findBitCoin(bitCoins,bitCoinID);
			if(temp_bitcoin==NULL){
				printf("unknown bitcoin");
			}
			else{
				printTransactionHistory(temp_bitcoin->pointTree);
			}

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

			//printf("EXIT\n");
		}
		else{
			printf("INVALID COMMAND!!!\n");
		}
	}
	free(command);
}











