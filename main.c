#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For usleep() function on Linux

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

char getch() {
#ifdef _WIN32
    return _getch();
#else
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return buf;
#endif
}

#define MAX_PASSWORD_LENGTH 50

void clearScreen(void) {
    printf("\033[H\033[J");  // ANSI escape code to clear the screen
    fflush(stdout);
}

void setCursorPosition(int x, int y) {
    printf("\033[%d;%df", y, x);
    fflush(stdout);
}

struct UserData {
    char username[50];
    int date, month, year;
    char phoneNumber[15];
    char adharNumber[20];
    char firstName[20];
    char lastName[20];
    char fatherName[20];
    char motherName[20];
    char address[50];
    char accountType[20];
};

struct MoneyTransfer {
    char recipientUsername[50];
    char senderUsername[50];
    long int amount;
};

struct UserPassword {
    char password[50];
};

void createUserAccount(void);
void displayUserInformation(char*);
void userLogin(void);
void loginSuccessful(void);
void transferMoney(void);
void checkBalance(char*);
void userLogout(void);
void accountCreated(void);

int main() {
    int choice;

    clearScreen();

    printf("\n\t\t\tWELCOME TO BANK ACCOUNT SYSTEM\n\n");
    printf("\t\t\t******************************\n\n");
    printf(" 1... CREATE A BANK ACCOUNT\n");
    printf(" 2... ALREADY A USER? SIGN IN\n");
    printf(" 3... EXIT\n\n");

    printf("ENTER YOUR CHOICE.. ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            clearScreen();
            createUserAccount();
            break;
        case 2:
            userLogin();
            break;
        case 3:
            exit(0);
            break;
        default:
            exit(0);
            break;
    }

    return 0;
}

void createUserAccount(void) {
    FILE *userDataFile;
    struct UserData newUser;

    FILE *userPasswordFile;
    struct UserPassword userPassword;

    // Open file to write user data
    userDataFile = fopen("userAccounts.txt", "ab");

    // Open file to write user password
    userPasswordFile = fopen("userPasswords.txt", "ab");

    // Input user information
    clearScreen();
    printf("\n\t\t\t\t=====CREATE ACCOUNT=====\n\n");

    printf(" FIRST NAME: ");
    scanf("%s", newUser.firstName);

    printf("\n LAST NAME: ");
    scanf("%s", newUser.lastName);

    printf("\n FATHER's NAME: ");
    scanf("%s", newUser.fatherName);

    printf("\n MOTHER's NAME: ");
    scanf("%s", newUser.motherName);

    printf("\n ADDRESS: ");
    scanf("%s", newUser.address);

    printf("\n ACCOUNT TYPE: ");
    scanf("%s", newUser.accountType);

    printf("\n DATE OF BIRTH: ");
    printf("\n DATE: ");
    scanf("%d", &newUser.date);

    printf(" MONTH: ");
    scanf("%d", &newUser.month);

    printf(" YEAR: ");
    scanf("%d", &newUser.year);

    printf("\n ADHAR NUMBER: ");
    scanf("%s", newUser.adharNumber);

    printf("\n PHONE NUMBER: ");
    scanf("%s", newUser.phoneNumber);

    printf("\n\tUSERNAME 50 CHARACTERS MAX!");
    printf("\n\tPASSWORD 50 CHARACTERS MAX!");

    printf("\n USERNAME: ");

    scanf("%s", newUser.username);

    char *enteredPassword = malloc(MAX_PASSWORD_LENGTH * sizeof(char));
    if (enteredPassword == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
    }

    printf("\n PASSWORD: ");
    fflush(stdout); // Flush the output buffer to ensure immediate printing
    int i = 0;
    char character;

    while ((character = getch()) != '\n' && character != '\r' && i < MAX_PASSWORD_LENGTH - 1) {
        enteredPassword[i++] = character;
        putchar('*'); // Print asterisk to simulate hiding the character
        fflush(stdout); // Flush the output buffer to ensure immediate printing
    }

    enteredPassword[i] = '\0';

    printf("\nEntered password: %s\n", enteredPassword);

    free(enteredPassword);

    // Write user data to the file
    fwrite(&newUser, sizeof(newUser), 1, userDataFile);

    // Write user password to the file
    strcpy(userPassword.password, enteredPassword);
    fwrite(&userPassword, sizeof(userPassword), 1, userPasswordFile);

    // Close files
    fclose(userDataFile);
    fclose(userPasswordFile);

    // After successful creation, call function
    accountCreated();
}

void displayUserInformation(char username1[]) {
    FILE *userDataFile;
    struct UserData user;

    // Open file to read user data
    userDataFile = fopen("userAccounts.txt", "rb");

    if (userDataFile == NULL) {
        printf("ERROR IN OPENING FILE\n");
        return;
    }

    clearScreen();

    printf("=== ACCOUNT INFORMATION ===\n\n");

    while (fread(&user, sizeof(user), 1, userDataFile)) {
        if (strcmp(username1, user.username) == 0) {
            printf("WELCOME, %s %s\n", user.firstName, user.lastName);
            printf(".........................\n\n");
            printf("=== YOUR ACCOUNT INFO ===\n");
            printf("*************************\n");
            printf("NAME: %s %s\n", user.firstName, user.lastName);
            printf("FATHER's NAME: %s %s\n", user.fatherName, user.lastName);
            printf("MOTHER's NAME: %s %s\n", user.motherName, user.lastName);
            printf("ADHAR CARD NUMBER: %s\n", user.adharNumber);
            printf("MOBILE NUMBER: %s\n", user.phoneNumber);
            printf("DATE OF BIRTH: %d-%d-%d\n", user.date, user.month, user.year);
            printf("ADDRESS: %s\n", user.address);
            printf("ACCOUNT TYPE: %s\n", user.accountType);

            break;  // No need to continue searching
        }
    }

    fclose(userDataFile);

    // Menu to perform different actions
    printf("\n\n HOME \n");
    printf("******\n");
    printf(" 1... CHECK BALANCE\n");
    printf(" 2... TRANSFER MONEY\n");
    printf(" 3... LOG OUT\n");
    printf(" 4... EXIT\n");
    printf("\n ENTER YOUR CHOICE.. ");
    
    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            checkBalance(username1);
            break;
        case 2:
            transferMoney();
            break;
        case 3:
            userLogout();
            userLogin();  // Log in again after logout
            break;
        case 4:
            exit(0);
            break;
        default:
            exit(0);
            break;
    }
}

void userLogin(void) {
    FILE *userDataFile;
    struct UserData user;
    
    FILE *userPasswordFile;
    struct UserPassword userPassword;

    // Open file to read user data
    userDataFile = fopen("userAccounts.txt", "rb");

    if (userDataFile == NULL) {
        printf("ERROR IN OPENING FILE\n");
        return;
    }

    // Open file to read user passwords
    userPasswordFile = fopen("userPasswords.txt", "rb");

    if (userPasswordFile == NULL) {
        printf("ERROR IN OPENING FILE\n");
        fclose(userDataFile);
        return;
    }

    clearScreen();

    printf("\t\t\t       ACCOUNT LOGIN       \n");
    printf("\t\t\t***************************\n\n");
    printf("\t\t\t       === LOGIN ===\n");

    char enteredUsername[50];
    //char enteredPassword[50];
    int foundUser = 0;

    printf("\t USERNAME: ");
    scanf("%s", enteredUsername);

    char *enteredPassword = malloc(MAX_PASSWORD_LENGTH * sizeof(char));
    if (enteredPassword == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
    }

    printf("\t PASSWORD: ");
    fflush(stdout); // Flush the output buffer to ensure immediate printing
    int i = 0;
    char character;

    while ((character = getch()) != '\n' && character != '\r' && i < MAX_PASSWORD_LENGTH - 1) {
        enteredPassword[i++] = character;
        putchar('*'); // Print asterisk to simulate hiding the character
        fflush(stdout); // Flush the output buffer to ensure immediate printing
    }
    enteredPassword[i] = '\0';

    printf("\nEntered password: %s\n", enteredPassword);

    free(enteredPassword);

    // Validate the entered credentials
    while (fread(&user, sizeof(user), 1, userDataFile) && fread(&userPassword, sizeof(userPassword), 1, userPasswordFile)) {
        if (strcmp(enteredUsername, user.username) == 0 && strcmp(enteredPassword, userPassword.password) == 0) {
            foundUser = 1;
            break;
        }
    }

    // Close files
    fclose(userDataFile);
    fclose(userPasswordFile);

    if (foundUser) {
        loginSuccessful();
        displayUserInformation(enteredUsername);
    } else {
        printf("\n\nInvalid username or password. Please try again.\n");
        sleep(2); // Sleep for 2 seconds to display the message
        userLogin();
    }
}

void loginSuccessful(void) {
    int i;
    clearScreen();

    printf("Fetching account details");

    // Simulate processing with a delay
    int j;
    for (j = 0; j < 5; j++) {
        usleep(120000);
        printf(".");
        fflush(stdout);
    }

    clearScreen();

    printf("\n\t\t\tLOGIN SUCCESSFUL\n");

    printf("\nPress Enter to continue\t");
    getchar();  // Consume the newline character
    getchar();  // Wait for the user to press Enter
}

void transferMoney(void) {
    FILE *userDataFile;
    struct UserData user;

    FILE *moneyTransactionFile;
    struct MoneyTransfer transaction;

    clearScreen();

    // Open file in read mode
    userDataFile = fopen("userAccounts.txt", "rb");

    if (userDataFile == NULL) {
        printf("Error in opening user data file\n");
        return;
    }

    // Open file to write amount along with username to which amount is going to be transferred
    moneyTransactionFile = fopen("moneyTransactions.txt", "ab");

    if (moneyTransactionFile == NULL) {
        printf("Error in opening money transaction file\n");
        fclose(userDataFile);
        return;
    }

    printf("*** TRANSFER MONEY ***\n");
    printf("======================\n");

    char senderUsername[20];
    char recipientUsername[20];

    printf("FROM (your username): ");
    scanf("%s", senderUsername);

    printf("TO (username of person): ");
    scanf("%s", recipientUsername);

    // Check for username if it is present in the file or not
    int foundSender = 0;

    while (fread(&user, sizeof(user), 1, userDataFile)) {
        if (strcmp(senderUsername, user.username) == 0) {
            foundSender = 1;
            strcpy(transaction.senderUsername, user.username);
            strcpy(transaction.recipientUsername, recipientUsername);
            break;
        }
    }

    if (!foundSender) {
        printf("\n\nSender username not found. Please try again.\n");
        sleep(2);  // Sleep for 2 seconds to display the message
        fclose(userDataFile);
        fclose(moneyTransactionFile);
        return;
    }

    // Taking amount input
    printf("ENTER THE AMOUNT TO BE TRANSFERRED: ");
    scanf("%ld", &transaction.amount);

    // Write to file
    fwrite(&transaction, sizeof(transaction), 1, moneyTransactionFile);

    fclose(userDataFile);
    fclose(moneyTransactionFile);

    // Display processing message
    printf("\nTransferring amount, please wait.");

    // Simulate processing with a delay
    int j;
    for (j = 0; j < 20; j++) {
        usleep(12000);
        printf(".");
        fflush(stdout);
    }

    printf("\n\nAMOUNT SUCCESSFULLY TRANSFERRED\n");
    getchar();  // Consume the newline character
    getchar();  // Wait for the user to press Enter

    // Function to return to the home screen
    displayUserInformation(senderUsername);
}

void checkBalance(char username2[]) {
    FILE *moneyTransactionFile;
    struct MoneyTransfer transaction;
    int i = 1, sumMoney = 0;

    // Open amount file record
    moneyTransactionFile = fopen("moneyTransactions.txt", "rb");

    if (moneyTransactionFile == NULL) {
        printf("Error in opening money transaction file\n");
        return;
    }

    clearScreen();

    printf("\t\t\t   === BALANCE DASHBOARD ===\n");
    printf("\t\t\t*******************************\n\n");

    int k = 5, l = 10;
    int m = 30, n = 10;
    int u = 60, v = 10;

    setCursorPosition(k, l);
    printf("S no.");
    setCursorPosition(m, n);
    printf("TRANSACTION ID");
    setCursorPosition(u, v);
    printf("AMOUNT");

    // Reading username to fetch the correct record
    while (fread(&transaction, sizeof(transaction), 1, moneyTransactionFile)) {
        if (strcmp(username2, transaction.senderUsername) == 0) {
            setCursorPosition(k, ++l);
            printf("%d", i);

            setCursorPosition(m, ++n);
            printf("%s", transaction.recipientUsername);

            setCursorPosition(u, ++v);
            printf("%ld", transaction.amount);

            // Adding and finding total money
            sumMoney += transaction.amount;

            i++;
        }
    }

    setCursorPosition(80, 10);
    printf("TOTAL AMOUNT\n");

    setCursorPosition(80, 11);
    printf("%d\n", sumMoney);

    fclose(moneyTransactionFile);

    setCursorPosition(0, 30);
    printf("Press Enter to continue");
    getchar();  // Consume the newline character
    getchar();  // Wait for the user to press Enter

    // Function to return to the home screen
    displayUserInformation(username2);
}

void userLogout(void) {
    int i, j;

    clearScreen();

    printf("Please wait, logging out");

    // Simulate logging out with a delay
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 2500000; j++) {
            i++;
            i--;
        }
        printf(".");
    }

    printf("\nSign out successfully...\n");

    printf("\nPress any key to continue...");
    getchar();  // Consume the newline character
    getchar();  // Wait for the user to press Enter
}

void accountCreated(void) {
    int i;
    clearScreen();

    printf("\n\nYour data is processing");

    // Simulate processing with a delay
    int j;
    for (j = 0; j < 5; j++) {
        usleep(120000);
        printf(".");
        fflush(stdout);
    }

    clearScreen();

    printf("\nACCOUNT CREATED SUCCESSFULLY...\n");

    printf("\nPress Enter to login\t");
    getchar();  // Consume the newline character
    getchar();  // Wait for the user to press Enter

    userLogin();
}
