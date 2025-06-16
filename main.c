#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"





int menu();


int main(int argc,char* argv[])
{
	SuperMarket	market;
	char* fileName;
	int choice; ;
	if (argc != 3)
		return -1;
	choice = atoi(argv[1]);
	fileName = argv[2];

	

	if (!initSuperMarket(&market,fileName,CUSTOMER_FILE_NAME,choice))
	{
		printf("error init  Super Market");
		return 0;
	}

	int option;
	int stop = 0;
	
	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;


		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if(!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerPay:
			if(!doPayment(&market))
				printf("Error in payment\n");
			break;
		
		case eSortCustomer:
			sortCustomers(&market);
			break;
			
		case eSearchCustomer:
			findCustomer(&market);
			break;
			
		case ePrintProductByType:
			printProductByType(&market);
			break;

		case EXIT:
			printMessage("Thank","you","for","Shopping","with","us",NULL);
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	handleCustomerStillShoppingAtExit(&market);

	if (!saveSuperMarketToFile(&market, fileName, CUSTOMER_FILE_NAME,choice))
		printf("Error saving supermarket to file\n");

	freeMarket(&market);

	system("pause");
	return 1;
}

int menu()
{
	int option;
	printf("\n\n");
	printf("Please choose one of the following options\n");
	for(int i = 0 ; i < eNofOptions ; i++)
		printf("%d - %s\n",i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

