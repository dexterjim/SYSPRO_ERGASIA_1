ΛΑΖΑΡΙΔΗΣ ΔΗΜΗΤΡΙΟΣ 1115201400086

Compile: make
Run (παραδειγμα εκτέλεσης): ./bitcoin -a input-files/bitCoinBalanceFile.txt -t input-files/transactionsFile.txt -v 10000 -h1 20 -h2 40 -b 320

main.c :
	-Αρχικά καλώ την συνάρτηση takeArguments η οποία διαβάζει τα ορίσματα απο την γραμμή
		εντολών και σε περίπτωση λάθους επιστρέφει -1 και τερματίζει το πρόγραμμα
	-Έπειτα καλώ την συνάρτηση takeData_BitCoinBalanceFile που διαβάζει ένα αρχείο με τα
		ονομάτα των χρηστών και τα bitcoins που αντιστοιχούν στον καθένα και αρχικοποιεί
		τις αντίστιχες δομές bitCoins και wallets
	-Μετά καλώ την συνάρτηση takeData_TransactionsFile η οποία διαβάζει ένα αρχείο που
		περιέχει Transactions , αρχικοποιεί τα 2 hashTables και την ListOfTransactions τις
		ενημερώνει κατάλληλα καθώς εκτελεί τα Transactions και επιστρέφει ένα integer
		max_tr_id ο οποίος σε περίπτωση λάθους επιστρέφει -1 αλλίως επιστρέφει το μήκος
		του μεγαλύτερου σε μήκος transaction id.Αυτό μετά το μετατρέπω σε
		next_tr_id=10^max_tr_id δηλ αν εχω id με μεγιστο 2 χαρακτήρες τα νέα id θα
		ξεκινάνε από το 100.
	-Σε αυτό το σημείο οι βασικές δομές είναι έτοιμες και ο χρήστης μπορεί να εκτελέσει τις
		εντολές που επιθυμεί. Έχω ένα line και command με σκοπό να κομματίασω την είσοδο
		του χρήστη.
	-Οι εντολές είναι οι εξής :
		1) requestTransaction , αφου ξεχωρίσω την εντολή από τα ορίσματά της καλώ την
			breakTransaction με σκοπό να διαβάσω τα ορίσματα , σε περίπτωση λάθους
			επιστρέφει NULL και έπειτα ελέγχω αν ο sender και ο receiver δεν είναι ο
			ίδιος χρήστης και ότι και τα 2 ονόματα έιναι εγκυρα. Εκτελώ την συναλλαγή
			και σε περίπτωση επιτυχίας το βάζω στην λίστα με τα Transaction.
		2) requestTransactions , αρχικά κοιτάζω αν υπάρχει ';' δηλ αν ο χρήστης μου έχει
			δώσει αρχείο ή αν επιθυμεί να δώσει Transaction από την γραμμή εντολών.
			Στην περίπτωση του αρχείου με παρόμοιο τρόπο όπως στην
			takeData_TransactionsFile ελέγχω αν υπάρχει , το ανοίγω και το περνάω όλο
			σε ένα string ώστε να μην χάνω χρόνο με το να διαβάζω πολλές φορές το
			αρχείο. Μετράω τα ';' ώστε να δω πόσα transactions έχω να εκτελέσω . Μετά
			δίνω στην breakTransaction τα κομμάτια ανάμεσα στα ; ώστε να πάρω τα
			δεδομένα της εκάστοτε συναλλαγής , και μετά ακολουθώ πάλι την διαδικασία
			απο requestTransaction. Τώρα στην περίπτωση που ο χρήστης θα μου δίνει
			είσοδο από την γραμμή εντολών κάνω την ίδια διαδικασία για τον έλεγχο και
			την εκτέλεση των συναλλαγών.Ο χρήστης εκτελεί όσες επιθυμεί και αναπάσα
			στιγμή μπορέι να πατήσει 'end' για να βγει.
		3) findEarnings/findPayments , αρχικά διαβάζω το walletId και ελέγχω αν έχει δωθεί
			χρονικό όριο μέσα στο οποίο πρέπει να ψαξω. Για να το κάνω αυτό μετράω τα
			':' για το time και τα '-' για το date , πρέπει να μετρήσω 2 ή 0 για το
			time και 4 ή 0 για το date ώστε να θεωρηθεί έγκυρο.Στη συνέχεια ότι ο
			χρήστης έχει υπάρξει είτε αποστολέας είτε παραλήπτης (ανάλογα την εντολή)
			ή αν δεν υπάρχει καν. Αν υπάρχει η συνάρτηση
			checkIfUserIsAlreadyInHashTable επιστρέφει receiver_temp kai
			receiver_point_on_bucket για τον κόμβο και που μέσα σε αυτόν υπάρχει η
			λίστα με τα transaction , στη συνέχεια τρέχω την λίστα καθώς ελέγχω αν οι
			συναλλαγές βρίσκονται μέσα στα επιθυμητά χρονικά όρια (αν έχουν ζητηθεί)
			και τα εκτυπώνω.
		4) walletStatus , διαβαζω το walletID , καλώ την findUser ώστε να βρω αν υπάρχει
			και αν ναι μου επιστρέφει το προφιλ του και εκτυπώνω το balance του.
		5) bitCoinStatus , διαβαζω το bitCoinID καλώ την findBitCoin ώστε να βρω αν
			υπάρχει και αν ναι μου επιστρέφει το σημείο που είναι αποθηκευμένο.Στη
			συνέχεια καλώ τις countTransactions και findUnspent.Πριν έχω μηδενίσει τα
			unused me 0 κάτι που απαιτεί η countTransactions.
		6) traceCoin , διαβαζω το bitCoinID καλώ την findBitCoin ώστε να βρω αν υπάρχει
			και αν ναι μου επιστρέφει το σημείο που είναι αποθηκευμένο.Στη συνέχεια
			καλώ την printTransactionHistory .Πριν έχω μηδενίσει τα unused me 0 κάτι
			που απαιτεί η printTransactionHistory.
		7) exit , κάνω free όλες τις δομές και τερματίζω το προγραμμα.


functions.c :
	- int takeArguments(struct_arguments **arguments,int argc,char **argv);
		Η συνάρτηση αυτή διαβάζει τα ορίσματα και ελέγχει ότι ο χρήστης τα έδωσε όλα και
		με σωστό format.

	- int takeData_BitCoinBalanceFile(bitCoinIdArray **bitCoins,struct_wallets **wallets,struct_arguments *arguments);
		Η συνάρτηση αυτή φτιάχνει τις δομές για τα bitcoins και για τα wallets , αρχικά
		ελέγχει αν υπάρχει το αρχείο και στη συνέχεια το ανοίγει και περνάει όλα τα
		δεδομένα του σε ένα string ώστε να προσπελάσω το αρχείο μόνο μια φορά.Κοιτάζω το
		string 2 φορές , μία για να δω πόσοι είναι οι χρήστες και πόσα τα bitcoins ώστε
		να κάνω τα αντίστοιχα malloc και την δευτερη φορά τα βάζω ένα ένα στις δομές καθώς
		ελέγχω για duplicate.

	- int checkForDuplicateWalletID(struct_wallets *wallets,int point);
		Η συνάρτηση αυτή συγκρίνει όλα τα στοιχεία που υπήρχαν στην μνήμη (0,point-1) με
		το point και αν βρει ίδιο επιστρέφει -1 που σημαίνει λάθος.

	- int checkForDuplicatebitCoinID(bitCoinIdArray *bitCoins,int point);
		Ομοίως με την checkForDuplicateWalletID.

	- void printList(usersBitCoinsNode *list);
		Διασχίζει την λίστα τύπου usersBitCoinsNode και τυπώνει τα στοιχεία της

	- void insertList(usersBitCoinsNode **list,bitCoinIdTreeNode *node);
		Διασχίζει την λίστα τύπου usersBitCoinsNode και όταν φτάσει στο τέλος της
		προσθέτει έναν κόμβο.

	- void deleteList_node(usersBitCoinsNode **list);
		Διαγράφει τον πρώτο κόμβο της λίστας.

	- int takeData_TransactionsFile(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable **senderHashTable,HashTable **receiverHashTable,listOfTransactions **ListOfTransactions,struct_arguments *arguments);
		Η συνάρτηση αυτή φτιάχνει τις δομές για τα hashTables και για την
		ListOfTransactions , αρχικά ελέγχει αν υπάρχει το αρχείο και στη συνέχεια το
		ανοίγει και περνάει όλα τα δεδομένα του σε ένα string .Φτιάχνει τις παραπάνω
		δομές και έπειτα διαβάζει τα transactions , κάνει τους αντίστοιχους ελέγχους
		για την ορθότητα των transactions και σε περίπτωση επιτυχίας της
		executeTransaction το βάζει στην ListOfTransactions.

	(ο χρήστης έχει φύλλα απο τα bitcoins οπότε αν έχω 2 φύλλα απο το ίδιο θα τα
	αντιμετωπίσω σαν να είναι διαφορετικά δηλ δεν με ενδιαφέρει απο που είναι το 
	συγκεκριμένο κομμάτι bitcoin)
	- int executeTransaction(bitCoinIdArray *bitCoins,struct_wallets *wallets,HashTable *senderHashTable,HashTable *receiverHashTable,struct_arguments *arguments,transaction *tr);
		Η συνάρτηση αυτή αρχικά ελέγχει αν ο sender έχει το απαιτούμενο υπόλοιπο για
		να πραγματοποιηθεί η συναλλαγή.Έπειτα ψάνχει τους χρήστες μέσα στα hashtable
		και τα αποθηκευει στα receiver_temp,receiver_point_on_bucket σε περίπτωση που
		δεν τους βρει τους προσθέτει και αυτούς και ξανά αποθηκεύει την θέση τους
		στα receiver_temp,receiver_point_on_bucket.Ανανεώνω το balance του καθενός
		και τρέχω την λίστα με τα bitcoins του και ως να ικανοποιηθεί το απαιτούμενο
		ποσό (remaining).Ανάλογα με το αν θα χρειαστώ όλο το ποσό του κόμβου ή αν θα
		χρειαστώ κάποιο κομμάτι του θα δημιουργήσω μόνο left την πρώτη περίπτωση ,left
		and right στην δευτερη περίπτωση.Και στις 2 περιπτώσεις διαγράφω τον κόμβο απο
		ta bitcoin του χρήστη και προσθέτω τα (1 ή 2) νέα φύλλα.
		
	- int checkIfSenderHasEnoughBalance(struct_wallets *wallets,transaction *tr);
		Η συνάρτηση αυτή αρχικά ψάχνει τον χρήστη και μετά συγκρίνει το υπόλοιπό του με
		το ποσό που απαιτεί η συναλλαγή και επιστρέφει 0 σε περίπτωση επιτυχιας -1 αλλιως.

	- oneWallet *findUser(struct_wallets *wallets,char *str);
		Η συνάρτηση αυτή ψάχνει τον πίνακα με τους χρήστες για να βρει που είναι ο χτήστης
		που θέλουμε σε περίπτωση αποτυχίας επιστρέφει NULL.

	- onebitCoinId *findBitCoin(bitCoinIdArray *bitCoins,char *str);
		Η συνάρτηση αυτή ψάχνει τον πίνακα με τα bitcoins για να βρει που είναι το
		bitcoin που θέλουμε σε περίπτωση αποτυχίας επιστρέφει NULL.

	- int hash(char *str,int mod);
		Μία απλή συνάρτηση όπου προσθέτει την αξία του κάθε χαρακτήρα της συμβολοσειρας
		και έπειτα το επιστρέφει το mod με κάποιον αριθμό.

	- transaction *breakTransaction(char *string,int begin,int end,listOfTransactions *ListOfTransactions);
		Η συνάρτηση αυτή παίρνει τα δεδομένα απο μία συναλλαγή, δέχεται ενα begin και end
		καθώς σε περίπτωση όπως είναι η requestTransactions από αρχείο του δίνω ένα string
		που τα περιέχει όλα τα Transactions που πρόκειται να γίνουν,οπότε έτσι ξεχωρίζω
		αυτό που με ενδιαφέρει.Διαβάζω με την σειρά τα στοιχεία και ελέγχω αν έχει δωθεί
		ημερομηνία αν είναι μεταγενέστερης της τελευταιας.

	- int checkForDuplicateTransactionID(listOfTransactions *lot);
		Η συνάρτηση αυτή διατρέχει την λίστα και ελέγχει αν έχει δωθεί 2 φορές το ίδιο
		TransactionID.

	- int checkIfUserIsAlreadyInHashTable(HashTable *ht,int offset,char *str,bucketNode **temp,int *point_on_bucket);
		Η συνάρτηση αυτή αρχικά πηγαίνει στην offset θέση του πίνακα HashTable και εκεί
		ψάχνει τον χρήστη μέσα στα buckets και επιστρέφει το bucket που το βρήκε καθώς
		και την θέση που τοποθετείται μέσα σε αυτόν.

	- void printTree(bitCoinIdTreeNode *node);
		Μία απλή συνάρτηση που τυπώνει όλους τους κόμβους της λίστας.

	- int findUnspent(bitCoinIdTreeNode *node);
		Η συνάρτηση αυτή επιστρέφει το ποσό του bitcoin που δεν έχει χρησιμοποιηθεί ακόμα
		σε κάποια συναλλαγή.Αυτό το κάνει πηγαίνοντας στο πιο δεξιά κόμβο του δέντρου.
		Στην συνέχεια ελέγχει αν αυτός ο κόμβος έχει αριστερό παιδί, αν ναι τότε αυτό
		σημαίνει ότι αυτός ο κόμβος έδωσε όλο το ποσό που έιχε οπότε το unspent=0 σε
		αντίθετη περίπτωση το value του κόμβου είναι το unspent.

	- int countDigits(int x);
		Μία απλή συνάρτηση όπου μετράει πόσα digits έχει ο χ

	- void printTransactionHistory(bitCoinIdTreeNode *node);
		Η συνάρτηση αυτή έιναι αναδρομική , αρχικά κοτάει αν υπάρχει αν υπάρχει αριστερό 
		παιδί ο λόγος που στο έξω if έχω μόνο το left είναι γτ αν σε περίπτωση αυτό είναι
		NULL τότε και το right ειναι NULL .Ελέγχει αν έχει ξανατυπώσει την συναλλαγή , με
		το πεδίο unused(που είναι όλα αρχικοποιημένα σε 0) , αν οχι την τυπώνει και
		προχωράει στα παιδιά.Ο λόγος που αν δεν υπάρχει αριστερό παιδί δεν κοιτάζω για
		συναλλαγή είναι γτ αυτός ο κόμβος δεν έχει λάβει μέρος σε συναλλαγή.

	- int check_date(transaction *tr,struct_time time1,struct_time time2,struct_date date1,struct_date date2,int flag_time,int flag_date);
		Η συνάρτηση αυτή δέχεται 3 ημερομηνίες αυτή της συναλλαγής και 2 αυτών των 
		χρονικών ορίων.Αν ο χρήστης έχει δώσει date τότε φτίαχνω τρεις 8digit αριθμούς
		που αντιστοιχούν στις ημερομηνίες.Έπειτα συγκίνω για να δω αν βρίσκεται ανάμεσα.
		Κάνω το ίδιο και για την ώρα αν ζητήται και αν χρειάζεται.

	- void freeBitCoinTree(bitCoinIdTreeNode *node);
		Η συνάρτηση αυτή έιναι αναδρομική , ο κάθε κόμβος καλεί τα παιδιά του και έπειτα
		κάνει free το walletID και τον ίδιο τον κόμβο.

	- void freeWalletUsersList(usersBitCoinsNode *node);
		Η συνάρτηση αυτή έιναι αναδρομική , ο κάθε κόμβος καλεί τον επόμενο και έπειτα
		κάνει τον ίδιο τον κόμβο.

	- void freeHashTableBuckets(bucketNode *node);
		Ομοίως αναδρομικά διατρέχει όλα τα bucketNode και μετά για καθένα απο αυτά καλεί
		την freeHashTableBucketsTrList για να διαγράψει τις λίστες με τις συναλλαγές.

	- void freeHashTableBucketsTrList(transactionNode *node);
		Παρόμοια με την freeWalletUsersList.

	- void freeListOfTransactions(transaction *node);
		Παρόμοια με την freeWalletUsersList.

	-int check_time(transaction *tr,listOfTransactions *ListOfTransactions);
		Όμοια με την check_date απλώς εδώ συγκρίνουμε τον χρόνο ανάμεσα σε 2 transaction.
		
	-int countTransactions(bitCoinIdTreeNode *node);
		Όμοια με την printTransactionHistory απλώς εδώ επιστρέφουμε πόσες συναλλαγές
		έχουν γίνει.Αν ο συγκεκριμένος κόμβος είναι unused επιστρέφω count+1 αλλιώς 
		count.
