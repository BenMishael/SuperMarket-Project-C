#include "ShoppingCart.h"
#ifndef __MACROS__
#define __MACROS__

#define CHECK_RETURN_0(ptr) {\
					 if (ptr==NULL){\
				     return 0;}\
					}
#define CHECK_MSG_RETURN_0(ptr,msg) {\
							if (ptr==NULL){\
							puts(msg);\
							return 0;}\
							}
#define FREE_CLOSE_FILE_RETURN_0(ptr,fileName) {\
							free(ptr);\
							fclose(fp);\
							return 0;\
							}

#define CLOSE_RETURN_0(fileName){\
						fclose(fp);\
						return 0;\
					}

#define DETAIL_PRINT

#ifdef DETAIL_PRINT
	#define PRINT_CART(pCart) printShoppingCart(pCart);
#else
	#define PRINT_CART(pCart) printShoppingCartNoTotal(pCart);
#endif

#endif // !__MACROS__