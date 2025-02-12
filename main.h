#pragma once


#define EXIT -1
#define VALID_ARGS_NUM 5
#define SUPER_FILE_NAME "SuperMarket.bin"
#define SUPER_FILE_NAME_COMPRESSED "SuperMarket_compress.bin"
#define CUSTOMER_FILE_NAME "Customers.txt"


typedef enum
{
	eShowSuperMarket, eAddProduct, eAddCustomer, eCustomerDoShopping, ePrintCart,eCustomerPay,
	eSortCustomer,eSearchCustomer,ePrintProductByType, eNofOptions
} eMenuOptions;

static const char* menuStrings[eNofOptions] = { "Show SuperMarket", "Add Product",
								"Add Customer", "Customer Shopping","Print Shopping Cart","Customer Pay",
								"Sort Customers", "Search an Customer","Print Product By Type" };

int menu();