#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 50

// account node
typedef struct node
{
	char username[MAX];
	char password[MAX];
	int status;
	struct node *next;
} node_t;

char filename[] = "account.txt";
int is_login = 0; 

// find a node exist in linked list given username
node_t *find_node(node_t *head, char *username)
{
	node_t *current = head;
	while(current != NULL)
	{
		if (strcmp(current->username, username) == 0)
			return current;
		current = current->next;
	}
	return NULL;
}

// save list to text file
void save_list(node_t *head)
{
	FILE *f;
	f = fopen(filename, "w");
	node_t *current;
	for (current = head; current; current = current->next)
		fprintf(f, "%s %s %d\n", current->username, current->password, current->status);
	fclose(f);
}

// append node to list
void append(node_t *head, node_t* node)
{
	if (head == NULL)
	{
		head = node;
		return;
	}
	node_t *current = head;
	while(current->next != NULL)
		current = current->next;
	current->next = node;
}

// MENU: register
void my_register(node_t *head)
{
	char username[MAX], password[MAX];

	printf(
		"\n---------------------------------------------\n"
		"REGISTER\n"
		"---------------------------------------------\n"
		"Create new account:\n");

	// get user input
	printf("Username: ");
	scanf("%[^\n]%*c", username);

	// check if account existed
	if (find_node(head, username))
	{
		printf("Account existed!\n");
		return;
	}

	// if not existed, ask for user password
	printf("Password: ");
	scanf("%[^\n]%*c", password);

	// create new node
	node_t *node = malloc(sizeof(node_t));
	strcpy(node->username, username);
	strcpy(node->password, password);
	node->status = 1;

	// add node to linked list and save
	append(head, node);
	save_list(head);
}

// MENU: sign in
void sign_in(node_t *head)
{
	char username[MAX], password[MAX];
	int wrong_times = 0;
	node_t *found;

	printf(
		"\n---------------------------------------------\n"
		"SIGN IN\n"
		"---------------------------------------------\n");

	// get username from input
	printf("Username: ");
	scanf("%[^\n]%*c", username);

	// if account not found in linked list
	if (!(found = find_node(head, username)))
	{
		printf("Cannot find account!\n");
		return;
	}

	// if found
	while(1)
	{	
		// ask for password
		printf("Password: ");
		scanf("%[^\n]%*c", password);

		// if password is correct
		if (!strcmp(found->password, password))
		{
			printf("Hello %s\n", found->username);
			is_login = 1;
			break;
		}

		// if not, inform and ask again
		printf("Password is incorrrect\n");

		// if password is typed wrong 3 time, lock account
		if (++wrong_times == 3)
		{
			found->status = 0;
			save_list(head);
			printf("Account is blocked\n");
			break;	
		}
	}
}

// MENU: search
void search(node_t *head)
{
	node_t *found;
	char username[MAX];

	printf(
		"\n---------------------------------------------\n"
		"SEARCH\n"
		"---------------------------------------------\n");

	// get user input
	printf("Username: ");
	scanf("%[^\n]%*c", username);

	// find account
	if (!(found = find_node(head, username)))
	{
		printf("Cannot find account!\n");
		return;
	}

	// get status if found
	if (found->status == 0)
		printf("Account is blocked\n");
	else
		printf("Account is active\n");
}

// MENU: sign out
void sign_out(node_t *head)
{
	char username[MAX];
	node_t *found;

	printf(
		"\n---------------------------------------------\n"
		"SIGN OUT\n"
		"---------------------------------------------\n");

	// get username from input
	printf("Username: ");
	scanf("%[^\n]%*c", username);

	// find account
	if (!(found = find_node(head, username)))
	{
		printf("Cannot find account!\n");
		return;
	}

	// if found, check if signed in or not
	if (is_login == 0)
		printf("Account is not sign in\n");
	else
		printf("Goodbye %s\n", found->username);
}

int main()
{
	int menu, status, count = 0;
	char username[MAX], password[MAX];
	FILE *f;
	node_t *head, *current;
	head = current = NULL;

	// load data from text file to linked list
	printf("LOADING DATA FROM FILE...\n");
	
	// get number of account
	if((f = fopen(filename, "r")) == NULL)
	{
		printf("Cannot open file!\n");
		exit(0);
	}

	// load accounts
	while(fscanf(f, "%s %s %d\n", username, password, &status) != EOF)
	{
		// create new node
		node_t *node = malloc(sizeof(node_t));
		strcpy(node->username, username);
		strcpy(node->password, password);
		node->status = status;

		// add node to list
		if (head == NULL)
			current = head = node;
		else
			current = current->next = node;
		count++;
	}
	
	fclose(f);
	printf("LOADED SUCCESSFULY %d ACCOUNT(S)\n", count);
	save_list(head);

	// menu
	do {
		printf(
			"\n---------------------------------------------\n"
			"USER MANAGEMENT PROGRAM\n"
			"---------------------------------------------\n"
			"1. Register\n"
			"2. Sign in\n"
			"3. Search\n"
			"4. Sign out\n"
			"Your choice (1-4, other to quit): ");

		scanf("%d", &menu);
		getchar();

		switch (menu)
		{
			case 1: my_register(head); break;
			case 2: sign_in(head); break;
			case 3: search(head); break;
			case 4: sign_out(head); break;
			default: break;
		}
	}
	while (menu >=1 && menu <= 4);

	return 0;
}