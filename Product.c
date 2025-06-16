#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "Address.h"
#include "fileHelper.h"


#define MIN_DIG 3
#define MAX_DIG 5

void	initProduct(Product* pProduct)
{
	initProductNoBarcode(pProduct);
	getBorcdeCode(pProduct->barcode);
}

void	initProductNoBarcode(Product* pProduct)
{
	initProductName(pProduct);
	pProduct->type = getProductType();
	pProduct->price = getPositiveFloat("Enter product price\t");
	pProduct->count = getPositiveInt("Enter product number of items\t");
}

void initProductName(Product* pProduct)
{
	do {
		printf("enter product name up to %d chars\n", NAME_LENGTH );
		myGets(pProduct->name, sizeof(pProduct->name),stdin);
	} while (checkEmptyString(pProduct->name));
}

void	printProduct(const Product* pProduct)
{
	printf("%-20s %-10s\t", pProduct->name, pProduct->barcode);
	printf("%-20s %5.2f %10d\n", typeStr[pProduct->type], pProduct->price, pProduct->count);
}

int		saveProductToFile(const Product* pProduct, FILE* fp)
{
	if (fwrite(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error saving product to file\n");
		return 0;
	}
	return 1;
}

int		loadProductFromFile(Product* pProduct, FILE* fp)
{
	if (fread(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error reading product from file\n");
		return 0;
	}
	return 1;
}

void getBorcdeCode(char* code)
{
	char temp[MAX_STR_LEN];
	char msg[MAX_STR_LEN];
	sprintf(msg,"Code should be of %d length exactly\n"
				"UPPER CASE letter and digits\n"
				"Must have %d to %d digits\n"
				"First and last chars must be UPPER CASE letter\n"
				"For example A12B40C\n",
				BARCODE_LENGTH, MIN_DIG, MAX_DIG);
	int ok = 1;
	int digCount = 0;
	do {
		ok = 1;
		digCount = 0;
		printf("Enter product barcode "); 
		getsStrFixSize(temp, MAX_STR_LEN, msg);
		if (strlen(temp) != BARCODE_LENGTH)
		{
			puts(msg);
			ok = 0;
		}
		else {
			//check and first upper letters
			if(!isupper(temp[0]) || !isupper(temp[BARCODE_LENGTH-1]))
			{
				puts("First and last must be upper case letters\n");
				ok = 0;
			} else {
				for (int i = 1; i < BARCODE_LENGTH - 1; i++)
				{
					if (!isupper(temp[i]) && !isdigit(temp[i]))
					{
						puts("Only upper letters and digits\n");
						ok = 0;
						break;
					}
					if (isdigit(temp[i]))
						digCount++;
				}
				if (digCount < MIN_DIG || digCount > MAX_DIG)
				{
					puts("Incorrect number of digits\n");
					ok = 0;
				}
			}
		}
		
	} while (!ok);

	strcpy(code, temp);
}


eProductType getProductType()
{
	int option;
	printf("\n\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNofProductType; i++)
			printf("%d for %s\n", i, typeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNofProductType);
	getchar();
	return (eProductType)option;
}

const char* getProductTypeStr(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typeStr[type];
}

int		isProduct(const Product* pProduct, const char* barcode)
{
	if (strcmp(pProduct->barcode, barcode) == 0)
		return 1;
	return 0;
}

int		compareProductByBarcode(const void* var1, const void* var2)
{
	const Product* pProd1 = (const Product*)var1;
	const Product* pProd2 = (const Product*)var2;

	return strcmp(pProd1->barcode, pProd2->barcode);
}


void	updateProductCount(Product* pProduct)
{
	int count;
	do {
		printf("How many items to add to stock?");
		scanf("%d", &count);
	} while (count < 1);
	pProduct->count += count;
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}
int saveProductToBinaryCompressed(Product* p, FILE* f)
{
	BYTE data[6] = {0};
	int len = strlen(p->barcode);
	int arr[7] = {0};
	int nameLen = strlen(p->name);
	for (int i = 0; i < 7; i++)
	{
		arr[i] = barcodeChar_To_index(p->barcode[i]);
	}
	data[0] = arr[0] << 2 | arr[1] >> 4;
	data[1] = arr[1] << 4 | arr[2] >> 2;
	data[2] = arr[2] << 6 | arr[3];
	data[3] = arr[4] << 2 | arr[5] >> 4;
	data[4] = arr[5] << 4 | arr[6] >> 2;
	data[5] = (arr[6] << 6) | (nameLen<<2) | (p->type);
	if (fwrite(data, sizeof(BYTE), 6, f) != 6)
		return 0;
	if (fwrite(&nameLen, sizeof(int), 1, f) != 1)
		return 0;
	if (fwrite(p->name, sizeof(char), nameLen, f) != nameLen)
		return  0;
	int shalem = (int)p->price;
	int agorot = (int)((p->price) * 100) % 100;
	BYTE byte[3] = { 0 };
	byte[0] = p->count;
	byte[1] = agorot << 1 | shalem >> 8;
	byte[2] = shalem;
	if (fwrite(byte, sizeof(BYTE), 3, f) != 3)
		return 0;
	return 1;
}
int barcodeChar_To_index(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return ch - 'A' + 10;
	else
	{
		if (ch<= '9' && ch>= '0')
			return ch - '0';
	}
	return -1;

}
char indexToChar(int i)
{
	if (i <= 9 && i >= 0)
		return i + '0';
	else
	{
		if (i > 9 && i<=35)
			return i - 10 + 'A';
	}
	return '@';
	
}
int readProductFromeCompressedFile(Product* p, FILE* f)
{
	BYTE data[6];
	if (fread(data, sizeof(BYTE), 6, f) != 6)
		return 0;
	p->barcode[0] = indexToChar(data[0] >> 2);
	p->barcode[1]= indexToChar((data[0]&0x3)<<4 | data[1]>>4);
	p->barcode[2]= indexToChar((data[1]& 0xF)<<2 | data[2]>>6);
	p->barcode[3] = indexToChar(data[2] & 0x3F );
	p->barcode[4] = indexToChar(data[3] >> 2);
	p->barcode[5] = indexToChar((data[3]& 0x3)<<2 | data[4]>>4);
	p->barcode[6] = indexToChar((data[4] & 0xF) << 2 | data[5] >> 6);
	p->barcode[7] = '\0';

	int lengthOfName = (data[5] & 0x3C) >> 2;
	p->type = data[5] & 0x3;
	p->name[lengthOfName] = '\0';
	if (!readCharsFromFile(p->name, lengthOfName, f, "error"))
		return 0;
	BYTE byte[3];
	if (fread(byte, sizeof(BYTE), 3, f) != 3)
		return 0;
	p->count = byte[0];
	int agorot = data[1] >> 1;
	int shalem = (data[1] & 0x1) << 8 | data[2];
	p->price = (float)(shalem * 100 + agorot)/100.0;
	return 1;
}

