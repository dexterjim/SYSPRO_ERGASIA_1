typedef struct struct_arguments struct_arguments;
typedef struct transaction transaction;
typedef struct bitCoinIdArray bitCoinIdArray;
typedef struct onebitCoinId onebitCoinId;
typedef struct bitCoinIdTreeNode bitCoinIdTreeNode;
typedef struct struct_wallets struct_wallets;
typedef struct oneWallet oneWallet;
typedef struct usersBitCoinsNode usersBitCoinsNode;
typedef struct HashTable HashTable;
typedef struct bucketNode bucketNode;
typedef struct bucketElement bucketElement;
typedef struct transactionNode transactionNode;
typedef struct listOfTransactions listOfTransactions;
typedef struct struct_date struct_date;
typedef struct struct_time struct_time;

struct struct_arguments{
	char *bitCoinBalanceFile;
	char *transactionsFile;
	int bitCoinValue;
	int senderHashtableNumOfEntries;
	int receiverHashtableNumOfEntries;
	int bucketSize;
};

////////////

struct bitCoinIdArray{
	int size;
	onebitCoinId *array;
};

struct onebitCoinId{
	char *bitcoinid;
	bitCoinIdTreeNode *pointTree;
};

struct bitCoinIdTreeNode{
	char *walletID;//se poion anhkei o kombos
	int value;

	transaction *tr;

	bitCoinIdTreeNode *left;
	bitCoinIdTreeNode *right;
};
//
struct struct_wallets{
	int size;//posous xrhstes exw
	oneWallet *users;
};

struct oneWallet{
	char *walletID;//=userID
	int balance;
	usersBitCoinsNode *list;//mia lista apo ta kommatia apo bitcoin pou exei o xrhsths
};

struct usersBitCoinsNode{
	bitCoinIdTreeNode *treenode;
	usersBitCoinsNode *next;
};

//////////////////

struct HashTable{
	//exw to size apo ta arguments (to size tou hash table mporei na einai diaforetiko analoga ti 8a dwsei o xrhsths)
	int numOfUsersPerBucket;
	bucketNode **buckets;
};

struct bucketNode{
	int last_entry;
	bucketElement *arrayOfUsers;
	bucketNode *next;
};

struct bucketElement{
	oneWallet *users;//deixnw sto wallet tou user
	transactionNode *transactionList;//h lista me ta transaction tou xrhsth
};

struct transactionNode{
	transaction *tr;
	transactionNode *next;
};

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
	transaction *next;
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

int takeArguments(struct_arguments **arguments,int argc,char **argv);

int takeData_BitCoinBalanceFile(bitCoinIdArray **bitCoins,struct_wallets **wallets,struct_arguments *arguments);
int checkForDuplicateWalletID(struct_wallets *wallets,int point);
int checkForDuplicatebitCoinID(bitCoinIdArray *bitCoins,int point);

void printList(usersBitCoinsNode *list);
void insertList(usersBitCoinsNode **list,bitCoinIdTreeNode *node);
void deleteList_node(usersBitCoinsNode **list);

int takeData_TransactionsFile(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable **senderHashTable,HashTable **receiverHashTable,listOfTransactions **ListOfTransactions,struct_arguments *arguments);
int executeTransaction(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable *senderHashTable,HashTable *receiverHashTable,struct_arguments *arguments,transaction *tr);
int checkIfSenderHasEnoughBalance(struct_wallets *wallets,transaction *tr);
oneWallet *findUser(struct_wallets *wallets,char *str);
onebitCoinId *findBitCoin(bitCoinIdArray *bitCoins,char *str);
int hash(char *str,int mod);
transaction *breakTransaction(char *string,int begin,int end,listOfTransactions *ListOfTransactions);
int checkForDuplicateTransactionID(listOfTransactions *lot);
int checkIfUserIsAlreadyInHashTable(HashTable *ht,int offset,char *str,bucketNode **temp,int *point_on_bucket);
void printTree(bitCoinIdTreeNode *node);
int findUnspent(bitCoinIdTreeNode *node);
int countDigits(int x);

void printTransactionHistory(bitCoinIdTreeNode *node);
int check_date(transaction *tr,struct_time time1,struct_time time2,struct_date date1,struct_date date2,int flag_time,int flag_date);
void freeBitCoinTree(bitCoinIdTreeNode *node);
void freeWalletUsersList(usersBitCoinsNode *node);
void freeHashTableBuckets(bucketNode *node);
void freeHashTableBucketsTrList(transactionNode *node);
void freeListOfTransactions(transaction *node);

int check_time(transaction *tr,listOfTransactions *ListOfTransactions);
int countTransactions(bitCoinIdTreeNode *node);
