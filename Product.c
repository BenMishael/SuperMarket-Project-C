#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "fileHelper.h"
#include "myMacros.h"


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

int		saveProductToBinaryFileCompressed(const Product* pProduct, FILE* fp)
{
	int nameLen = strlen(pProduct->name);
	int intPart = (int)pProduct->price;
	int decPart = ((int)(pProduct->price * N_DECIMAL_POINTS_PRECISION) % N_DECIMAL_POINTS_PRECISION);
	
	BYTE data1[3] = { 0 }; // 4 chars (6 bits) = 24
	BYTE data2[3] = { 0 }; // 3 char (6 bits) + int (4 bits) +int (2 bit) = 24
	BYTE data3[3] = { 0 }; // int amount (8 bits) + int cents (7 bits) + int price (9 bits) = 24
	
	char tempBarcode[BARCODE_LENGTH + 1];
	strcpy(tempBarcode, pProduct->barcode);
	encryptBarcode(tempBarcode);
	
	//Bits work
	data1[0] = ((tempBarcode[0] & 0x3f) << 2) | (tempBarcode[1] >> 4);
	data1[1] = ((tempBarcode[1] & 0xf) << 4) | (tempBarcode[2] >> 2);
	data1[2] = ((tempBarcode[2] & 0x3) << 6) | (tempBarcode[3] & 0x3f);
	
	data2[0] = (((tempBarcode[4] & 0x3f) << 2) | (tempBarcode[5] >> 4));
	data2[1] = ((tempBarcode[5] & 0xf) << 4) | ((tempBarcode[6] & 0x3c) >> 2);
	data2[2] = ((tempBarcode[6] & 0x3) << 6) | ((BYTE)strlen(pProduct->name) & 0x3f) << 2 | pProduct->type;

	data3[0] = pProduct->count;
	data3[1] = (decPart << 1) | intPart & 0x100;
	data3[2] = intPart & 0xff;


	if (fwrite(&data1, sizeof(BYTE), 3, fp) != 3)
		return 0;

	if (fwrite(&data2, sizeof(BYTE), 3, fp) != 3)
		return 0;

	if (!writeCharsToFile(pProduct->name, nameLen, fp, "Error in writing product's name\n"))
		return 0;

	if (fwrite(&data3, sizeof(BYTE), 3, fp) != 3)
		return 0;


	return 1;
}

int		loadProductFromBinaryFileCompressed(Product* pProduct, FILE* fp)
{
	int intPart, nameLen, floatPart;
	BYTE data1[3];
	BYTE data2[3];
	BYTE data3[3];
	char tempBarcode[8] = { 0 };

	if (fread(data1, sizeof(BYTE), 3, fp) != 3)
	{
		return 0;
	}

	if (fread(data2, sizeof(BYTE), 3, fp) != 3)
	{
		return 0;
	}

	tempBarcode[0] = ((data1[0] >> 2) & 0x3f);
	tempBarcode[1] = ((data1[0] << 4) & 0x30) | ((data1[1] >> 4) & 0xf);
	tempBarcode[2] = ((data1[1] & 0xf) << 2) | ((data1[2] >> 6) & 0x3);
	tempBarcode[3] = data1[2] & 0x3f;
	tempBarcode[4] = (data2[0] >> 2) & 0x3f;
	tempBarcode[5] = ((data2[0] & 0x3) << 4) | ((data2[1] >> 4) & 0xf);
	tempBarcode[6] = ((data2[1] & 0xf) << 2) | ((data2[2] >> 6) & 0x3);
	tempBarcode[7] = '\0';
	decryptBarcode(tempBarcode);

	strcpy(pProduct->barcode, tempBarcode);
	pProduct->type = data2[2] & 0x3;

	nameLen = (data2[2] >> 2) & 0xf;
	if (!readCharsFromFile(pProduct->name, nameLen, fp, "Error In reading product's name"))
		return 0;

	pProduct->name[nameLen] = '\0';

	if (fread(data3, sizeof(BYTE), 3, fp) != 3)
	{
		return 0;
	}

	
	pProduct->count = data3[0];
	floatPart = (int)((data3[1] >> 1) & 0x7f);
	intPart = (int)(data3[1] & 0x1) << 8 | data3[2];
	pProduct->price = floatPart;
	pProduct->price /= 100;
	pProduct->price += intPart;
	
	return 1;
}

void encryptBarcode(char* barcode)
{
	for (int i = 0; i < strlen(barcode); i++)
	{
		if (barcode[i] <= '9')
			barcode[i] -= '0';
		else
			barcode[i] -= '7';
	}
}

void decryptBarcode(char* barcode)
{
	for (int i = 0; i < BARCODE_LENGTH; i++)
	{
		if ((int)barcode[i] <= 9)
			barcode[i] += '0';//for number
		else
			barcode[i] += '7';//for letter
	}
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}