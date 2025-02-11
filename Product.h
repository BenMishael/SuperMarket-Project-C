#pragma once

typedef enum {eFruitVegtable, eFridge, eFrozen, eShelf, eNofProductType} eProductType;

static const char* typeStr[eNofProductType] = { "Fruit Vegtable", "Fridge", "Frozen", "Shelf" };

#define NAME_LENGTH 20
#define BARCODE_LENGTH 7

typedef struct
{
	char			name[NAME_LENGTH+1]; //max length =< 15 (4 bits)
	char			barcode[BARCODE_LENGTH + 1];// length = 48 bits / 6 bits for every char
	eProductType	type;// (0 to 3 >> 2 bits)
	float			price;// max price = 0 to 99 for cents (7 bits) 0 to 512 for other (9 bits)
	int				count;// max count = 255 (8 bits)
}Product;

void	initProduct(Product* pProduct);
void	initProductNoBarcode(Product* pProduct);
void	initProductName(Product* pProduct);
void	printProduct(const Product* pProduct);
int		saveProductToFile(const Product* pProduct, FILE* fp);
int		loadProductFromFile(Product* pProduct, FILE* fp);

int		compareProductByBarcode(const void* var1, const void* var2);

void	getBorcdeCode(char* code);
int		isProduct(const Product* pProduct,const char* barcode);
eProductType getProductType();
const char*	getProductTypeStr(eProductType type);

void	updateProductCount(Product* pProduct);
void	freeProduct(Product* pProduct);

void encryptBarcode(char* barcode);
void decryptBarcode(char* barcode);

int		saveProductToBinaryFileCompressed(const Product* pProduct, FILE* fp);
int		loadProductFromBinaryFileCompressed(Product* pProduct, FILE* fp);
