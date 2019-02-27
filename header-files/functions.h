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
	bitCoinIdTreeNode *pointTree;
};

struct bitCoinIdTreeNode{
	char *walletID;
	int value;

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
	int usersPartOfBitCoin;
	int offsetOfBitCoin;//to offset ston pinaka bitCoinIdArray
	usersBitCoinsNode *next;
};
//

int takeData_BitCoinBalanceFile(bitCoinIdArray **bitCoins,struct_wallets **wallets,struct_arguments *arguments);
int checkForDuplicateWalletID(struct_wallets *wallets,int point);
int checkForDuplicatebitCoinID(bitCoinIdArray *bitCoins,int point);

void printList(usersBitCoinsNode *list);
void insertList(usersBitCoinsNode **list,int value,int offset);

//////////////////

int takeData_TransactionsFile(void);
