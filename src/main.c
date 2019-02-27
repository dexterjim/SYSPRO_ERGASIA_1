#include <stdio.h>
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

	if(takeData_TransactionsFile()==-1){
		return 3;
	}
}
