#pragma once

#include <stdio.h>
#include "Supermarket.h"

int		saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
			const char* customersFileName, int isCompressed);
int		saveSuperMarketToFileCompressed(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);
int		saveSuperMarketToFileNotCompressed(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);
int		loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
			const char* customersFileName,int isCompressed);
int		loadSuperMarketFromFileCompressed(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);
int		loadSuperMarketFromFileNotCompressed(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);


//int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName);


int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName);
