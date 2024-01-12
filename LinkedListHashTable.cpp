/*
* Filename: LinkedListHashTable
* Project: LinkedListHashTable
* Programmer: Salma Rageh
* Date: 2023-03-17
* Description: This program takes names from a file and
*			   stores them in a Hash Table and a Linked list.
*			   It then searches for the names and tells the user
*			   the number of comparisons each data structure made 
*			   before finding the name. It's used as a comparison
*			   between Linked lists and Hash Tables.
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#pragma warning(disable: 4996)

#define kMaxInput 21
#define kHashTableSize 127

// Struct
typedef struct NameNode
{
	char* name = NULL;
	struct NameNode* next = NULL;
}NameNode;


// Prototypes
void clearNewLine(char* str);
NameNode* enterNewInfo(NameNode* newHead, char* input);
int myHashFunction(char* str);
unsigned long hash(char* str);
NameNode* searchLinkedList(char* searchName, NameNode* linkedList, int* comparisonCount);
void searchForNameTwice(char* searchName, NameNode* linkedList, NameNode* hashTable, int comparisonCount[2]);
void freeAll(NameNode* head);


int main()
{
	// Hash Table
	NameNode* hashTable[kHashTableSize];

	for (int i = 0; i < kHashTableSize; i++)
	{
		hashTable[i] = NULL;
	}

	NameNode* head = NULL;

	char fileName[kMaxInput];

	// Reading file
	FILE* fp = NULL;
	fp = fopen("names.txt", "r");

	if (fp == NULL)
	{
		printf("can't read file\n");
		return 1;
	}

	while (fgets(fileName, kMaxInput, fp) != NULL)
	{
		clearNewLine(fileName);
		int index = myHashFunction(fileName);

		// Insert into hashtable
		hashTable[index] = enterNewInfo(hashTable[index], fileName);

		// Insert into sorted linked list
		head = enterNewInfo(head, fileName);
	}

	if (fclose(fp) != 0)
	{
		printf("error closing");
	}

	int bothComparison[2] = { 0,0 };
	char searchWord[kMaxInput];
	int totalSearches = 0;
	int totalSearchesLL = 0;
	int totalSearchesHash = 0;

	printf("Search for names.  Input '.' to exit the loop.\n\n");

	while (1)
	{
		// user search word
		fgets(searchWord, kMaxInput, stdin);
		clearNewLine(searchWord);

		if (searchWord[0] == '.')
		{
			break;
		}

		totalSearches++;
		int searchIndex = myHashFunction(searchWord);

		// Search lists
		searchForNameTwice(searchWord, head, hashTable[searchIndex], bothComparison);

		totalSearchesLL = totalSearchesLL + bothComparison[0];
		totalSearchesHash = totalSearchesHash + bothComparison[1];

		bothComparison[0] = 0;
		bothComparison[1] = 0;
	}

	// Final totals
	printf("\n\n\n");
	printf("\tTotal Number of Searches: %d\n", totalSearches);
	printf("\tTotal Number of Comparisons in Linked List: %d\n", totalSearchesLL);
	printf("\tTotal Number of Comparisons in Hash Table: %d\n", totalSearchesHash);

	// Free linked list
	freeAll(head);

	// Free hash table
	for (int i = 0; i < kHashTableSize; i++)
	{
		free(hashTable[i]);
	}

	return 0;
}


/*
 * Function: searchLinkedList()
 * Description: This function search a linked list for a word.
 * Parameters:  NameNode* head: pointer to the head of the list the word is in
 *				char* searchName: name your searching for
 *				int* comparisonCount: pointer to int filled in by the function with the
 *									  count of strcmp comparisons done in searching the linked list
 * Returns: Pointer to the node search word is in
 *			Null if search word isn't in the list
*/

NameNode* searchLinkedList(char* searchName, NameNode* linkedList, int* comparisonCount)
{
	NameNode* node = linkedList;
	while ((node != NULL) && (strcmp(node->name, searchName) < 0))
	{
		(*comparisonCount)++;
		node = node->next;
	}

	if (node != NULL && strcmp(node->name, searchName) == 0)
	{
		(*comparisonCount)++;
		return node;
	}

	return NULL;

}


/*
 * Function: searchForNameTwice()
 * Description: This function search a linked list for a word.
 * Parameters:  NameNode* head: pointer to the head of the list the word is in
 *				char* searchName: name your searching for
 *				int* comparisonCount[2]: array containing the count of strcmp comparisons done
 *										 in searching the extremely-long sorted linked list (element 0) and in searching
 *										 the hash table (element 1)
 *				NameNode* hashTable: hash table to search
 * Returns: nothing
*/

void searchForNameTwice(char* searchName, NameNode* linkedList, NameNode* hashTable, int comparisonCount[2])
{
	NameNode* listNode = NULL;
	NameNode* hashTableNode = NULL;
	listNode = searchLinkedList(searchName, linkedList, &comparisonCount[0]);
	hashTableNode = searchLinkedList(searchName, hashTable, &comparisonCount[1]);

	if ((listNode != NULL) && (hashTableNode != NULL))
	{
		printf("\t%s was found in the linked list in %d comparisons.\n", searchName, comparisonCount[0]);
		printf("\t%s was found in the hash table bucket in %d comparisons.\n", searchName, comparisonCount[1]);
	}
	else if ((listNode == NULL) && (hashTableNode == NULL))
	{
		printf("\t%s was NOT found in the linked list in %d comparisons.\n", searchName, comparisonCount[0]);
		printf("\t%s was NOT found in the hash table bucket in %d comparisons.\n", searchName, comparisonCount[1]);
	}
}


/*
 * Function: enterNewInfo()
 * Description: This function enters information into the hash table or linked list
 * Parameters:  WordNode* newHead: pointer to the head of the list
 *				char* input: input word
 * Returns: newHead: pointer to the new head of the list
*/

NameNode* enterNewInfo(NameNode* newHead, char* input)
{
	int index = myHashFunction(input);
	NameNode* newBlock = NULL;
	NameNode* ptr = NULL;
	NameNode* prev = NULL;

	// allocate a block of memory for new record
	newBlock = (NameNode*)malloc(sizeof(NameNode));
	if (newBlock == NULL)
	{
		printf("Error! No more memory\n");
		return newHead;
	}

	// allocate memory for name
	newBlock->name = (char*)malloc((strlen(input)) + 1);
	if (newBlock->name == NULL)
	{
		printf("Error! No more memory\n");
		return newHead;
	}
	strcpy(newBlock->name, input);
	newBlock->next = NULL;

	// now link this record to the list in the appropriate place
	if (newHead == NULL)
	{
		// empty list, so set head
		newHead = newBlock;
	}

	else if (strcmp(newHead->name, newBlock->name) >= 0) // special case
	{
		// we're inserting at the front of the list

		// set the next pointer for the newBlock record to be the
		// location that used to be at the front of the list
		newBlock->next = newHead;
		// set first_number to point to the new start of the list
		newHead = newBlock;
	}
	else
	{


		prev = newHead;		// first item in list
		ptr = newHead->next;	// second item in list 

		while (ptr != NULL)
		{
			if (strcmp(ptr->name, newBlock->name) >= 0)
			{
				// we've found a name in the list that is either equal to or greater 
				// than the one we're entering now
				break;
			}
			prev = ptr;
			ptr = ptr->next;
		}

		// add the new node here, between prev and ptr
		newBlock->next = ptr;
		prev->next = newBlock;

	}

	return newHead;

}


/*
 * Function: freeAll()
 * Description: This function removes all items from the linked list.
 * Parameters: NameNode *head: start of linked list
 * Returns: nothing
*/

void freeAll(NameNode* head)
{
	NameNode* curr = NULL, * next = NULL;

	curr = head;

	// traverse the list
	while (curr != NULL)
	{
		next = curr->next;
		free(curr->name);
		free(curr);
		curr = next;
	}

}


/*
 * Function: myHashFuction()
 * Description: This function gets a string and calsulated its key value
 * Parameters:  char* str: a string
 * Returns: int hashValue: a number from 0-10 giving the bucket
*/

int myHashFunction(char* str)
{
	unsigned long keyValue = hash(str);
	int hashValue = keyValue % kHashTableSize;
	return hashValue;
}


/*
* TITLE : “hash” function source code
* AUTHOR : Dan Bernstein
* DATE : 2023-02-12
* AVAILABIILTY : Hash Table lecture PDF
*/

unsigned long hash(char* str)
{
	unsigned long hash = 5381;
	int c = 0;
	while ((c = *str++) != '\0')
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}



/*
* Function: void clearCRLF(char *str)
*
* Description: This function will get rid of a \n at the end of a string.
*			   If there isn't one, it doesn't change the string.
*
* Parameter: char *str: start of the string
* Return Value: nothing
*/

void clearNewLine(char* str)
{
	char* whereIsNewLine = strchr(str, '\n');
	if (whereIsNewLine != NULL)
	{
		*whereIsNewLine = '\0';
	}
}