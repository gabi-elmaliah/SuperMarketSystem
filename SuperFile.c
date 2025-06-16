#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include"myMacros.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName, int choice)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp)
	if (choice == 1)
	{
		if (!saveSuperToBinaryFileCompressed(pMarket, fp))
		{ 
			fclose(fp);
			return 0;
		}
	}
	else
	{
		if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
		{
			fclose(fp);
			return 0;
		}
		if (!saveAddressToFile(&pMarket->location, fp))
		{
			fclose(fp);
			return 0;
		}
		int count = getNumOfProductsInList(pMarket);
		if (!writeIntToFile(count, fp, "Error write product count\n"))
		{
			fclose(fp);
			return 0;
		}

		Product* pTemp;
		NODE* pN = pMarket->productList.head.next; //first Node
		while (pN != NULL)
		{
			pTemp = (Product*)pN->key;
			if (!saveProductToFile(pTemp, fp))
			{
				fclose(fp);
				return 0;
			}
			pN = pN->next;
		}

	}
	fclose(fp);
	saveCustomerToTextFile(pMarket->customerArr,pMarket->customerCount, customersFileName);
	return 1;
}
#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_MSG_RETURN_0(fp)
	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name,fp)
	}

	int count;
	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
	}

	if (count > 0)
	{
		Product* pTemp;
		for (int i = 0; i < count; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
			}
			if (!loadProductFromFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
			}
		}
	}
	fclose(fp);
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName,&pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;
}
#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		CLOSE_RETURN_0(fp)
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		free(pMarket->name);
		CLOSE_RETURN_0(fp)
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	int count;
	fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		pTemp = (Product*)calloc(1, sizeof(Product));
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}
int saveSuperToBinaryFileCompressed(SuperMarket* pMarket, FILE* f)
{
	BYTE data[2]={0};
	int count = getNumOfProductsInList(pMarket);
	int len = strlen(pMarket->name);
	data[0] = (count >> 2);
	data[1] = (count & 0X3) << 6 | len;
	if (fwrite(data,sizeof(BYTE),2,f)!= 2)
		return 0;
	if (fwrite(pMarket->name, sizeof(char), len, f) != len)
		return 0;
	if (saveAdreesToBinaryCompressed(&pMarket->location, f) == 0)
		return 0;
	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!saveProductToBinaryCompressed(pTemp,f))
			return 0;
		pN = pN->next;
	}
	return 1;
}
int readSuperMarketFromBinaryFileCompressed(SuperMarket* pMarket, char* fileName,const char* customersFileName)
{
	FILE* f = fopen(fileName, "rb");
	if (!f)return 0;
	BYTE data[2];
	if (fread(data,sizeof(BYTE), 2, f) != 2)
		return 0;
	int productCount = (int)data[0]<<2 | data[1]>>6;
	int NameLen = data[1] & 0x3F;
	pMarket->name = (char*)calloc(NameLen+1,sizeof(char));
	if (!readCharsFromFile(pMarket->name, NameLen,f,"error"))
		return 0;
	if (!readAddressFromBinaryCompressed(&pMarket->location, f))
		return 0;
	for (int i = 0; i < productCount; i++)
	{
		Product* temp=(Product*)calloc(productCount,sizeof(Product));
		if (!productCount)
		{
			printf("Allocation error\n");
			L_free(&pMarket->productList, freeProduct);
			free(pMarket->name);
			CLOSE_RETURN_0(f)
		}
		if (!readProductFromeCompressedFile(temp,f))
		{
			L_free(&pMarket->productList, freeProduct);
			free(pMarket->name);
			CLOSE_RETURN_0(f)
		}
		if (!insertNewProductToList(&pMarket->productList, temp))
		{
			L_free(&pMarket->productList, freeProduct);
			free(pMarket->name);
			fclose(f);
			return 0;
		}
	}
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	fclose(f);
	return 1;
}