#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include "myMacros.h"

int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName, int isCompressed)
{
	if (isCompressed)
		return saveSuperMarketToFileCompressed(pMarket, fileName, customersFileName);
	else
		return saveSuperMarketToFileNotCompressed(pMarket, fileName, customersFileName);
}

int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName, int isCompressed)
{
	if (isCompressed)
		return loadSuperMarketFromFileCompressed(pMarket, fileName, customersFileName);
	else
		return loadSuperMarketFromFileNotCompressed(pMarket, fileName, customersFileName);
}

int	loadSuperMarketFromFileNotCompressed(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
		CHECK_MSG_RETURN_0(fp, "Error open company file\n");

	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
		CLOSE_RETURN_0(fp);
	

	if (!loadAddressFromFile(&pMarket->location, fp))
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	

	int count;
	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	

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
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
			if (!loadProductFromFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
		}
	}


	fclose(fp);

	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
		CHECK_RETURN_0(pMarket->customerArr);

	return	1;

}

int	saveSuperMarketToFileNotCompressed(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	if (!fp) {
		printf("Error open supermarket file to write\n");
		return 0;
	}

	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
		CLOSE_RETURN_0(fp);

	if (!saveAddressToFile(&pMarket->location, fp))
		CLOSE_RETURN_0(fp);
	int count = getNumOfProductsInList(pMarket);

	if (!writeIntToFile(count, fp, "Error write product count\n"))
		CLOSE_RETURN_0(fp);

	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!saveProductToFile(pTemp, fp))
			CLOSE_RETURN_0(fp);
		pN = pN->next;
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}

int	saveSuperMarketToFileCompressed(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	if (!fp) {
		printf("Error open supermarket file to write\n");
		return 0;
	}

	BYTE countAndLength[2] = { 0 };
	int productCount,marketNameLen;
	marketNameLen = strlen(pMarket->name);
	productCount = getNumOfProductsInList(pMarket);
	countAndLength[0] = ((productCount >> 2) & 0xff);
	countAndLength[1] = ((productCount & 0x3) << 6) | (marketNameLen & 0x3f);

	if (fwrite(&countAndLength, sizeof(BYTE), 2, fp) != 2)
	{
		printf("Error write product count and name length\n");
		fclose(fp);
		return 0;
	}

	// Save super Name
	if (fwrite(pMarket->name, sizeof(char), marketNameLen, fp) != marketNameLen)
	{
		fclose(fp);
		return 0;
	}

	if (!saveAddressToFileCompressed(&pMarket->location, fp))
	{
		fclose(fp);
		return 0;
	}

	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!saveProductToBinaryFileCompressed(pTemp, fp))
		{
			fclose(fp);
			return 0;
		}
		pN = pN->next;
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}

int	loadSuperMarketFromFileCompressed(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
		CHECK_MSG_RETURN_0(fp, "Error open company file\n");

	BYTE countAndLength[2] = { 0 };
	int productCount, nameLength;

	if (fread(countAndLength, sizeof(BYTE), 2, fp) != 2)
	{
		printf("Error in reading supermarket prodcount and length\n");
		fclose(fp);
		return 0;
	}

	productCount = (int)((countAndLength[0] << 2) | ((countAndLength[1] >> 6) & 0x3f));
	nameLength = (int)(countAndLength[1] & 0x3f);

	pMarket->name = (char*)calloc(1, nameLength + 1);
		CHECK_RETURN_0(pMarket->name);
	if(fread(pMarket->name, sizeof(char), nameLength, fp) != nameLength)
		return 0;

	if(!loadAddressFromFileCompressed(&pMarket->location, fp))
		FREE_CLOSE_FILE_RETURN_0(&pMarket->location, fp);
		
	
	

	if(productCount > 0)
	{
		Product* pTemp;
		for (int i = 0; i < productCount; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
			if (!loadProductFromBinaryFileCompressed(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
		}
	}


	fclose(fp);

	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
		CHECK_RETURN_0(pMarket->customerArr);
	return	1;
}

//int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
//	const char* customersFileName)
//{
//	FILE* fp;
//	fp = fopen(fileName, "rb");
//	if (!fp)
//	{
//		printf("Error open company file\n");
//		return 0;
//	}
//
//	//L_init(&pMarket->productList);
//
//
//	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
//	if (!pMarket->name)
//	{
//		fclose(fp);
//		return 0;
//	}
//
//	if (!loadAddressFromFileCompressed(&pMarket->location, fp))
//	{
//		free(pMarket->name);
//		fclose(fp);
//		return 0;
//	}
//
//	fclose(fp);
//
//	loadProductFromTextFile(pMarket, "Products.txt");
//
//
//	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
//	if (!pMarket->customerArr)
//		return 0;
//
//	return	1;
//
//}
//
//
//int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
//{
//	FILE* fp;
//	//L_init(&pMarket->productList);
//	fp = fopen(fileName, "r");
//	int count;
//	fscanf(fp, "%d\n", &count);
//
//
//	//Product p;
//	Product* pTemp;
//	for (int i = 0; i < count; i++)
//	{
//		pTemp = (Product*)calloc(1, sizeof(Product));
//		myGets(pTemp->name, sizeof(pTemp->name), fp);
//		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
//		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
//		insertNewProductToList(&pMarket->productList, pTemp);
//	}
//
//	fclose(fp);
//	return 1;
//}