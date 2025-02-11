#pragma once

typedef struct
{
	int		num;// max num = 255 (8 bits)
	char*	street;// save without '\0' (no compress)
	char*	city;// save without '\0' (no compress)
}Address;

int		initAddress(Address* pAd);
void	printAddress(const Address* pAd);
int		saveAddressToFile(const Address* pAdd, FILE* fp);
int saveAddressToFileCompressed(const Address* pAdd, FILE* fp);
int		loadAddressFromFile(Address* pAdd, FILE* fp);
int loadAddressFromFileCompressed(Address* pAdd, FILE* fp);
void	freeAddress(Address* pAd);

int		checkElements(char**  elements, int count);
char*	fixAddressParam(char* param);
void	freeElements(char**  elements, int count);

