typedef struct struct_arguments struct_arguments;

struct struct_arguments{
	char *bitCoinBalanceFile;
	char *transactionsFile;
	int bitCoinValue;
	int senderHashtableNumOfEntries;
	int receiverHashtableNumOfEntries;
	int bucketSize;
};

int takeArguments(struct_arguments **arguments,int argc,char **argv);

////////////
typedef struct transaction transaction;

typedef struct bitCoinIdArray bitCoinIdArray;
typedef struct onebitCoinId onebitCoinId;
typedef struct bitCoinIdTreeNode bitCoinIdTreeNode;
typedef struct struct_wallets struct_wallets;
typedef struct oneWallet oneWallet;
typedef struct usersBitCoinsNode usersBitCoinsNode;

//
struct bitCoinIdArray{
	int size;
	onebitCoinId *array;
};

struct onebitCoinId{
	char *bitcoinid;
	int numOfTransactions;
	bitCoinIdTreeNode *pointTree;
};

struct bitCoinIdTreeNode{
	char *walletID;
	int value;

	transaction *tr;

	bitCoinIdTreeNode *left;
	bitCoinIdTreeNode *right;
};
//
struct struct_wallets{
	int size;//posous xrhstes exw
	oneWallet *users;
	//+++ na sundew ta bitcoin me tous katoxous
};

struct oneWallet{
	char *walletID;//=userID
	int balance;
	usersBitCoinsNode *list;//mia lista/pinaka me deiktes sto bitCoinIdArray , mporei na balw kai allh domh gia na krataw ti poso apo to sugkekrimeno bitcoin exei o xrhsths
};

struct usersBitCoinsNode{
	//int usersPartOfBitCoin;//MALLON DEN XREIAZETAI
	int offsetOfBitCoin;//to offset ston pinaka bitCoinIdArray , to exw gia to bitCoinStatus !!!!!!!!!!!!!!
	bitCoinIdTreeNode *treenode;
	usersBitCoinsNode *next;
};
//

int takeData_BitCoinBalanceFile(bitCoinIdArray **bitCoins,struct_wallets **wallets,struct_arguments *arguments);
int checkForDuplicateWalletID(struct_wallets *wallets,int point);
int checkForDuplicatebitCoinID(bitCoinIdArray *bitCoins,int point);

void printList(usersBitCoinsNode *list);
void insertList(usersBitCoinsNode **list,bitCoinIdTreeNode *node,int offset);//pros8esa to offset gia na peirazw to numberOfTransaction
void deleteList_node(usersBitCoinsNode **list);

//////////////////
typedef struct HashTable HashTable;
typedef struct bucketNode bucketNode;
typedef struct bucketElement bucketElement;
typedef struct transactionNode transactionNode;

//typedef struct transaction transaction;

struct HashTable{
	//exw to size apo ta arguments (to size tou hash table)
	int numOfUsersPerBucket;
	bucketNode **buckets;
};

struct bucketNode{
	//oneWallet **users;//deixnw sto wallet tou user wste na mhn exw kai edw to onoma  //char **users; NA TO TSEKARW MPOREI NA EINAI LA8OS , EPILPEWN NA DW AN YPARXEI PROBLHMA TON UPOLOGISMO TWN numOfUsersPerBucket
	int last_entry;
	bucketElement *arrayOfUsers;//
	//transactionNode *transactionList;//edw 8elw enan pinaka apo tetoia !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! cid=17
	bucketNode *next;
};

struct bucketElement{
	oneWallet *users;//deixnw sto wallet tou user wste na mhn exw kai edw to onoma  //char **users; NA TO TSEKARW MPOREI NA EINAI LA8OS , EPILPEWN NA DW AN YPARXEI PROBLHMA TON UPOLOGISMO TWN numOfUsersPerBucket
	transactionNode *transactionList;//edw 8elw enan pinaka apo tetoia !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! cid=17
};

struct transactionNode{
	usersBitCoinsNode *withUser;//me poion ekane thn sunalagh , eite tou edwse eite tou phre bitcoins, ousiastika einai o deikths sto dentro ,MPOREI NA MHN XREIAZETAI MIAS KAI TO FILAW STO tr
	transaction *tr;//MALLON PREPEI NA BALW EDW NA FILAW KAPWS KAI TO TRANSACTION , MPORW ME DEIKTH OPWS KAI BUCKETS
	transactionNode *next;
};

//typedef struct arrayOfTransactions arrayOfTransactions;
typedef struct listOfTransactions listOfTransactions;
//typedef struct transaction transaction;//endiktikh domh ws na dw ti paizei me to arxeio transactionsFile
typedef struct struct_date struct_date;
typedef struct struct_time struct_time;
/*
struct arrayOfTransactions{
	int size;
	transaction *tr;
};
*/
struct listOfTransactions{
	transaction *start;
	transaction *end;
};

struct transaction{
	char *transactionID;
	char *senderWalletID;
	char *receiverWalletID;
	int value;
	struct_date *date;
	struct_time *time;
	transaction *next;//
	int unused;
};
struct struct_date{
	int day;
	int month;
	int year;
};
struct struct_time{
	int hour;
	int minutes;
};

int takeData_TransactionsFile(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable **senderHashTable,HashTable **receiverHashTable,listOfTransactions **ListOfTransactions,struct_arguments *arguments);
int executeTransaction(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable *senderHashTable,HashTable *receiverHashTable,struct_arguments *arguments,transaction *tr);
int checkIfSenderHasEnoughBalance(struct_wallets *wallets,transaction *tr);
oneWallet *findUser(struct_wallets *wallets,char *str);
onebitCoinId *findBitCoin(bitCoinIdArray *bitCoins,char *str);
int hash(char *str,int mod);
transaction *breakTransaction(char *string,int begin,int end);
int checkForDuplicateTransactionID(listOfTransactions *lot);
int checkIfUserIsAlreadyInHashTable(HashTable *ht,int offset,char *str,bucketNode **temp,int *point_on_bucket);
void printTree(bitCoinIdTreeNode *node);
int findUnspent(bitCoinIdTreeNode *node);
int countDigits(int x);

void printTransactionHistory(bitCoinIdTreeNode *node);
