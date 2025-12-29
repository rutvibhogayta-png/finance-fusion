#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----------------- CONFIG ----------------- */
#define MAX_USERS 10
#define MAX_BANKS 12
#define MAX_ACCOUNTS 5
#define MAX_TRANSACTIONS 50
#define MAX_STOCKS 5
#define MAX_USER_STOCKS 10
#define MAX_SIPS 5
#define MAX_LINE 256

/* ----------------- STRUCTS ----------------- */
typedef struct {
    char text[100]            // transaction description
} Transaction;

typedef struct {
    char bankName[30];
    int pin;
    int accountNumber;          
    double balance;
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount;
} BankAccount;

typedef struct {
    char stockName[30];
    int quantity;
} UserStock;

typedef struct {
    char name[30];
    double monthlyAmount;
} SIPScheme;

typedef struct {
    char sipName[30];
    double investedAmount;
    int schemeIndex;
} UserSIP;

typedef struct {
    char username[30];
    char password[20];
    BankAccount accounts[MAX_ACCOUNTS];
    int accountCount;
    UserStock stocks[MAX_USER_STOCKS];
    int stockCount;
    UserSIP sips[MAX_SIPS];
    int sipCount;
} User;

/* ----------------- GLOBALS ----------------- */
User users[MAX_USERS];
int userCount = 0;
double stockPrices[MAX_STOCKS]

const char *availableStocks[MAX_STOCKS] = {"TCS","INFY","RELIANCE","HDFC","ICICI"};
const char *bankList[MAX_BANKS] = {
    "SBI","HDFC","ICICI","Axis","Kotak","PNB","BankOfBaroda","IndusInd","YesBank","Canara","Union","IDFC"
};

#define MAX_SIP_SCHEMES 5
SIPScheme fixedSIPS[MAX_SIP_SCHEMES] = {
    {"HUL_Growth", 500.0},
    {"Bajaj_Finance", 1000.0},
    {"SmallCap_Focus", 750.0},
    {"Tech_Opportunity", 1200.0}
    {"Global_Bonds", 600.0}
}

/* ----------------- FUNCTION DECLARATIONS ----------------- */
void saveData();
void loadData();
int generateAccountNumber();
void addTransaction(BankAccount *acc, const char *desc);
void updateStockPrices();
void updateSIPValues(User *u);
void signUp();
int loginPrompt();
void userMenu(int userIndex);
void bankMenu(User *u);
void createBankAccount(User *u);
int selectAccount(User *u);
void manageAccount(User *u, int accIndex);
void investmentMenu(User *u);
void stockMarket(User *u);
void sipMenu(User *u);
void trim_newline(char *s);

/* ----------------- HELPER ----------------- */
void trim_newline(char *s) {
    size_t L = strlen(s);
    while (L > 0 && (s[L-1] == '\n' || s[L-1] == '\r')) {
        s[--L] = '\0';
    }
}

/* ----------------- MAIN ----------------- */
int main() {
    srand((unsigned)time(NULL));
    loadData();

    while (1) {
        printf("\n---------------- Finance Fusion ----------------\n");
        printf("1. Sign Up (New User)\n");
        printf("2. Login (Existing User)\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        int ch;
        if (scanf("%d", &ch) != 1) { while(getchar()!='\n'); printf("Invalid input.\n"); continue; }
        while(getchar()!='\n'); // clear buffer

        if (ch == 1) signUp();
        else if (ch == 2) {
            int idx = loginPrompt();
            if (idx >= 0) userMenu(idx);
        }
        else if (ch == 3) {
            saveData();
            printf("Goodbye!\n");
            return 0;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

/* ----------------- USER / AUTH ----------------- */
void signUp() {
    if (userCount >= MAX_USERS) { printf("User limit reached.\n"); return; }
    User nu;
    printf("Enter username (no spaces): "); scanf("%29s", nu.username);
    printf("Enter password (no spaces): "); scanf("%19s", nu.password);
    nu.accountCount = 0; nu.stockCount = 0; nu.sipCount = 0;
    users[userCount++] = nu;
    saveData();
    printf("Sign up successful. You can now log in.\n");
}

int loginPrompt() {
    char uname[30], pwd[20];
    printf("Username: "); scanf("%29s", uname);
    printf("Password: "); scanf("%19s", pwd);
    for (int i = 0; i < userCount; ++i) {
        if (strcmp(users[i].username, uname) == 0 && strcmp(users[i].password, pwd) == 0) {
            printf("Login successful. Welcome %s!\n", uname);
            return i;
        }
    }
    printf("Invalid credentials.\n");
    return -1;
}

/* ----------------- USER MENU ----------------- */
void userMenu(int userIndex) {
    User *u = &users[userIndex];
    updateStockPrices();
    updateSIPValues(u);

    while (1) {
        printf("\n--- Main Menu for %s ---\n", u->username);
        printf("1. Bank Section\n2. Investment Section\n3. View Portfolio\n4. Logout\n");
        printf("Enter choice: ");
        int ch; if(scanf("%d",&ch)!=1){ while(getchar()!='\n'); printf("Invalid\n"); continue; } while(getchar()!='\n');

        if(ch==1) bankMenu(u);
        else if(ch==2) investmentMenu(u);
        else if(ch==3) {
            printf("\n--- Portfolio ---\n");
            for(int i=0;i<u->accountCount;i++){
                BankAccount *a=&u->accounts[i];
                printf("%d) %s A/C:%d Bal:%.2f\n", i+1, a->bankName, a->accountNumber, a->balance);
            }
            printf("\nStocks:\n");
            for(int i=0;i<u->stockCount;i++) printf("%s : %d\n", u->stocks[i].stockName, u->stocks[i].quantity);
            printf("\nSIPs:\n");
            for(int i=0;i<u->sipCount;i++) printf("%s : %.2f\n", u->sips[i].sipName, u->sips[i].investedAmount);
        }
        else if(ch==4) { saveData(); printf("Logged out.\n"); return; }
        else printf("Invalid.\n");
    }
}

/* ----------------- BANK ----------------- */
void bankMenu(User *u) {
    while(1){
        printf("\n--- Bank Menu ---\n1. Create Bank Account\n2. Manage Existing Accounts\n3. Back\nEnter choice: ");
        int ch; if(scanf("%d",&ch)!=1){ while(getchar()!='\n'); printf("Invalid\n"); continue; } while(getchar()!='\n');
        if(ch==1) createBankAccount(u);
        else if(ch==2){ if(u->accountCount==0){ printf("No accounts.\n"); continue; } int idx=selectAccount(u); if(idx>=0) manageAccount(u,idx); }
        else if(ch==3){ saveData(); return; }
        else printf("Invalid.\n");
    }
}

void createBankAccount(User *u){
    if(u->accountCount>=MAX_ACCOUNTS){ printf("Max accounts reached.\n"); return; }
    printf("Select bank:\n");
    for(int i=0;i<MAX_BANKS;i++) printf("%d. %s\n",i+1,bankList[i]);
    int choice; scanf("%d",&choice); while(getchar()!='\n');
    if(choice<1 || choice>MAX_BANKS){ printf("Invalid bank.\n"); return; }

    BankAccount na;
    strncpy(na.bankName, bankList[choice-1],29); na.bankName[29]='\0';
    printf("Set 4-digit PIN: "); scanf("%d",&na.pin); while(getchar()!='\n');
    na.accountNumber = generateAccountNumber();
    na.balance=0.0; na.transactionCount=0;

    u->accounts[u->accountCount++]=na;
    printf("Account created: %s A/C:%d\n", na.bankName, na.accountNumber);

    double dep; printf("Initial deposit: "); scanf("%lf",&dep); while(getchar()!='\n');
    if(dep>0){ u->accounts[u->accountCount-1].balance+=dep; char desc[100]; sprintf(desc,"Initial deposit %.2f",dep); addTransaction(&u->accounts[u->accountCount-1],desc); }
    saveData();
}

int selectAccount(User *u){
    printf("Select account:\n");
    for(int i=0;i<u->accountCount;i++) printf("%d. %s A/C:%d Bal:%.2f\n", i+1, u->accounts[i].bankName, u->accounts[i].accountNumber, u->accounts[i].balance);
    int idx; scanf("%d",&idx); while(getchar()!='\n');
    if(idx<1||idx>u->accountCount){ printf("Invalid choice.\n"); return -1; }
    return idx-1;
}

void manageAccount(User *u,int accIndex){
    BankAccount *acc=&u->accounts[accIndex];
    while(1){
        printf("\n--- Manage %s A/C:%d ---\n1. Deposit\n2. Withdraw\n3. Transactions\n4. Back\nEnter: ", acc->bankName, acc->accountNumber);
        int ch; if(scanf("%d",&ch)!=1){ while(getchar()!='\n'); printf("Invalid\n"); continue; } while(getchar()!='\n');
        if(ch==1){ double amt; printf("Deposit amt: "); scanf("%lf",&amt); while(getchar()!='\n'); if(amt>0){ acc->balance+=amt; char d[100]; sprintf(d,"Deposited %.2f",amt); addTransaction(acc,d); printf("Done.\n"); } else printf("Invalid\n"); }
        else if(ch==2){ double amt; printf("Withdraw amt: "); scanf("%lf",&amt); while(getchar()!='\n'); if(amt>0&&amt<=acc->balance){ acc->balance-=amt; char d[100]; sprintf(d,"Withdrew %.2f",amt); addTransaction(acc,d); printf("Done.\n"); } else printf("Invalid/Insufficient\n"); }
        else if(ch==3){ printf("Balance: %.2f\n",acc->balance); for(int t=0;t<acc->transactionCount;t++) printf("%d. %s\n",t+1,acc->transactions[t].text); }
        else if(ch==4){ saveData(); return; }
        else printf("Invalid.\n");
    }
}

/* ----------------- INVESTMENTS ----------------- */
void investmentMenu(User *u){
    while(1){
        printf("\n--- Investment Menu ---\n1. Stock Market\n2. SIP\n3. Back\nEnter: ");
        int ch; if(scanf("%d",&ch)!=1){ while(getchar()!='\n'); printf("Invalid\n"); continue; } while(getchar()!='\n');
        if(ch==1) stockMarket(u);
        else if(ch==2) sipMenu(u);
        else if(ch==3){ saveData(); return; }
        else printf("Invalid.\n");
    }
}

void stockMarket(User *u){
    updateStockPrices();
    printf("\n--- Stock Market ---\n");
    for(int i=0;i<MAX_STOCKS;i++) printf("%d. %s Price: %.2f\n", i+1, availableStocks[i], stockPrices[i]);

    printf("Buy? 1=Yes 0=No: "); int buy; scanf("%d",&buy); while(getchar()!='\n');
    if(!buy) return;

    int s; printf("Stock # to buy: "); scanf("%d",&s); while(getchar()!='\n'); if(s<1||s>MAX_STOCKS){ printf("Invalid\n"); return; }
    if(u->accountCount==0){ printf("No bank account\n"); return; }
    int accIdx=selectAccount(u); if(accIdx<0) return;
    BankAccount *acc=&u->accounts[accIdx];

    int pin; printf("Enter PIN: "); scanf("%d",&pin); while(getchar()!='\n'); if(pin!=acc->pin){ printf("Wrong PIN\n"); return; }
    int qty; printf("Qty: "); scanf("%d",&qty); while(getchar()!='\n'); if(qty<=0){ printf("Invalid\n"); return; }
    double cost=stockPrices[s-1]*qty; if(cost>acc->balance){ printf("Insufficient funds\n"); return; }

    acc->balance-=cost; char td[100]; sprintf(td,"Bought %d %s for %.2f",qty,availableStocks[s-1],cost); addTransaction(acc,td);

    int found=-1;
    for(int i=0;i<u->stockCount;i++) if(strcmp(u->stocks[i].stockName,availableStocks[s-1])==0){ found=i; break; }
    if(found>=0) u->stocks[found].quantity+=qty;
    else if(u->stockCount<MAX_USER_STOCKS){ strncpy(u->stocks[u->stockCount].stockName,availableStocks[s-1],29); u->stocks[u->stockCount].stockName[29]='\0'; u->stocks[u->stockCount].quantity=qty; u->stockCount++; }
    else printf("Stock limit reached.\n");

    printf("Purchase complete.\n");
    saveData();
}

/* ----------------- SIP ----------------- */
void sipMenu(User *u){
    printf("\n1. Create SIP\n2. Withdraw SIP\n3. Back\nChoice: "); int ch; scanf("%d",&ch); while(getchar()!='\n');
    if(ch==1){
        if(u->sipCount>=MAX_SIPS){ printf("Max SIPs reached.\n"); return; }
        if(u->accountCount==0){ printf("No bank account\n"); return; }

        printf("--- Available SIPs ---\n");
        for(int i=0;i<MAX_SIP_SCHEMES;i++) printf("%d. %s (%.2f)\n",i+1,fixedSIPS[i].name,fixedSIPS[i].monthlyAmount);
        int scheme; printf("Select: "); scanf("%d",&scheme); while(getchar()!='\n'); if(scheme<1||scheme>MAX_SIP_SCHEMES){ printf("Invalid\n"); return; }
        int idx=scheme-1;
        double amt=fixedSIPS[idx].monthlyAmount;

        for(int i=0;i<u->sipCount;i++) if(u->sips[i].schemeIndex==idx){ printf("Already have this SIP\n"); return; }

        int accIdx=selectAccount(u); if(accIdx<0) return;
        BankAccount *acc=&u->accounts[accIdx];
        int pin; printf("Enter PIN: "); scanf("%d",&pin); while(getchar()!='\n'); if(pin!=acc->pin){ printf("Wrong PIN\n"); return; }
        if(amt>acc->balance){ printf("Insufficient funds\n"); return; }

        acc->balance-=amt; char td[100]; sprintf(td,"Invested %.2f in SIP %s",amt,fixedSIPS[idx].name); addTransaction(acc,td);
        strncpy(u->sips[u->sipCount].sipName,fixedSIPS[idx].name,29); u->sips[u->sipCount].sipName[29]='\0';
        u->sips[u->sipCount].investedAmount=amt; u->sips[u->sipCount].schemeIndex=idx; u->sipCount++;
        printf("SIP created.\n"); saveData();
    }
    else if(ch==2){
        if(u->sipCount==0){ printf("No SIPs\n"); return; }
        for(int i=0;i<u->sipCount;i++) printf("%d. %s : %.2f\n",i+1,u->sips[i].sipName,u->sips[i].investedAmount);
        int pick; printf("Pick SIP to withdraw: "); scanf("%d",&pick); while(getchar()!='\n'); if(pick<1||pick>u->sipCount){ printf("Invalid\n"); return; }
        int accIdx=selectAccount(u); if(accIdx<0) return;
        BankAccount *acc=&u->accounts[accIdx];

        double wAmt=u->sips[pick-1].investedAmount;
        acc->balance+=wAmt; char td[100]; sprintf(td,"Withdrew SIP %s: %.2f",u->sips[pick-1].sipName,wAmt); addTransaction(acc,td);

        for(int i=pick-1;i<u->sipCount-1;i++) u->sips[i]=u->sips[i+1]; u->sipCount--;
        printf("SIP withdrawn.\n"); saveData();
    }
    else return;
}

/* ----------------- TRANSACTIONS ----------------- */
void addTransaction(BankAccount *acc, const char *desc){
    if(acc->transactionCount>=MAX_TRANSACTIONS){
        for(int i=1;i<MAX_TRANSACTIONS;i++) acc->transactions[i-1]=acc->transactions[i];
        acc->transactionCount--;
    }
    strncpy(acc->transactions[acc->transactionCount].text,desc,99); acc->transactions[acc->transactionCount].text[99]='\0';
    acc->transactionCount++;
}

/* ----------------- UTILITIES ----------------- */
int generateAccountNumber(){ return 10000+rand()%90000; }

void updateStockPrices(){ for(int i=0;i<MAX_STOCKS;i++) stockPrices[i]=100+rand()%100; }

void updateSIPValues(User *u){ for(int i=0;i<u->sipCount;i++) u->sips[i].investedAmount*=1.03; }

/* ----------------- FILE HANDLING ----------------- */
void saveData(){
    FILE *fp=fopen("userdata.txt","w"); if(!fp)return;
    fprintf(fp,"%d\n",userCount);
    for(int i=0;i<userCount;i++){
        User *u=&users[i];
        fprintf(fp,"%s %s %d %d %d\n",u->username,u->password,u->accountCount,u->stockCount,u->sipCount);
        for(int j=0;j<u->accountCount;j++){
            BankAccount *a=&u->accounts[j];
            fprintf(fp,"%s %d %d %.2lf %d\n",a->bankName,a->accountNumber,a->pin,a->balance,a->transactionCount);
            for(int t=0;t<a->transactionCount;t++) fprintf(fp,"%s\n",a->transactions[t].text);
        }
        for(int j=0;j<u->stockCount;j++) fprintf(fp,"%s %d\n",u->stocks[j].stockName,u->stocks[j].quantity);
        for(int j=0;j<u->sipCount;j++) fprintf(fp,"%s %.2lf %d\n",u->sips[j].sipName,u->sips[j].investedAmount,u->sips[j].schemeIndex);
    }
    fclose(fp);
}

void loadData(){
    FILE *fp=fopen("userdata.txt","r"); if(!fp) return;
    char line[MAX_LINE];
    if(!fgets(line,sizeof(line),fp)){ fclose(fp); return; }
    userCount=atoi(line);
    for(int u=0;u<userCount&&u<MAX_USERS;u++){
        User *usr=&users[u];
        if(!fgets(line,sizeof(line),fp)){ userCount=u; break; }
        int ac,sc,sic;
        if(sscanf(line,"%29s %19s %d %d %d",usr->username,usr->password,&ac,&sc,&sic)!=5){ userCount=u; break; }
        usr->accountCount=ac; usr->stockCount=sc; usr->sipCount=sic;

        for(int a=0;a<usr->accountCount;a++){
            if(!fgets(line,sizeof(line),fp)){ userCount=u; return; }
            BankAccount *acc=&usr->accounts[a]; int tcount;
            if(sscanf(line,"%29s %d %d %lf %d",acc->bankName,&acc->accountNumber,&acc->pin,&acc->balance,&tcount)!=5){ userCount=u; return; }
            acc->transactionCount=tcount;
            for(int t=0;t<tcount;t++){ if(!fgets(line,sizeof(line),fp)){ acc->transactionCount=t; break; } trim_newline(line); strncpy(acc->transactions[t].text,line,99); acc->transactions[t].text[99]='\0'; }
        }
        for(int s=0;s<usr->stockCount;s++){ if(!fgets(line,sizeof(line),fp)){ usr->stockCount=s; break; } if(sscanf(line,"%29s %d",usr->stocks[s].stockName,&usr->stocks[s].quantity)!=2){ usr->stockCount=s; break; } }
        for(int si=0;si<usr->sipCount;si++){ if(!fgets(line,sizeof(line),fp)){ usr->sipCount=si; break; } int idx; if(sscanf(line,"%29s %lf %d",usr->sips[si].sipName,&usr->sips[si].investedAmount,&idx)!=3){ usr->sipCount=si; break; } usr->sips[si].schemeIndex=idx; }
    }
    fclose(fp);
}

