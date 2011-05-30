	#include <stdio.h>

struct node {											
	char  data;										
	node* pNext;									
};

	static void appendNode(node* pHead, node* Node){
		node* pCurrent=pHead;	
		while(pCurrent->pNext!=NULL)
		{
			pCurrent=pCurrent->pNext;
		}
		pCurrent->pNext=Node;

		printf("added '%c' to the end of the list\n",Node->data);
	}

	static int countList(node* pHead){
		node* pCurrent = pHead; 
		int c=0;
		while(pCurrent!=NULL)
		{
			c++;
			pCurrent = pCurrent->pNext; 
		}
		return c;
	}	

	static void printList(node* pHead)
	{
		node* pCurrent = pHead; 
		int i;              		
		for(i=1;i<=countList(pHead);i++)               			
		{
			printf("data node %i: %c\n",i, pCurrent->data);    
			pCurrent = pCurrent->pNext;    			
		}
		printf("\n");
	}

	static void deleteNode(int position, node* pHead){
		if(position==1)
		{
			printf("cannot delete list head");
			return;
		}
		int i;
		node* pCurrent = pHead; 
		char c;
		for(i=0;i<countList(pHead);i++)
		{
			if((i+2)==position){
				c=pCurrent->pNext->data;
				pCurrent->pNext=pCurrent->pNext->pNext;	
			}

			pCurrent = pCurrent->pNext; 
		}
		printf("deleted node at position %i containing '%c'\n",position,c);
	}
	
	int main()
	{

		node N1, N2, N3, N4, N5;
		node* pHead;

		N1.data = 'A';								
		pHead = &N1;							
		
		N2.data = 'B';								
		N1.pNext = &N2;								
		
		N3.data = 'C';							
		N2.pNext = &N3;	
		
		N4.data = 'D';
		N3.pNext = &N4;
		
		N4.pNext = NULL;
		

		printList(pHead);
		
		//deleting at position 3 means 			
		deleteNode(3,pHead);	
		

		printList(pHead);

		node NA;
		NA.data='X';
		NA.pNext=NULL;
		appendNode(pHead,&NA);

		printList(pHead);
		

		
		return 0;
	}
	
