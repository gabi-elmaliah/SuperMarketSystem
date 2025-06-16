#pragma once

#include <stdio.h>
#include "Supermarket.h"
typedef unsigned char BYTE;
int		saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
			const char* customersFileName,int choice);
int		loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
			const char* customersFileName);

//int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName);


int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName);
int saveSuperToBinaryFileCompressed(SuperMarket* pMarket, const char* fileName);
int readSuperMarketFromBinaryFileCompressed(SuperMarket* pMarket, char* fileName, const char* customersFileName);