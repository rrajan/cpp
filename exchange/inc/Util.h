/*
 * Author: Ravi Rajan
 */

#ifndef Util_H
#define Util_H

#include <IAustralianStockExchange.h>

namespace Exchange {

class Helper {

public:
    static inline std::string getErrorCodeStr(int eCode)
    {
        if (IAustralianStockExchange::eNO_ERROR == eCode)
            return std::string("eNO_ERROR");
        else if (IAustralianStockExchange::eINTERNAL_SYSTEM_FAILURE == eCode)
            return std::string("eINTERNAL_SYSTEM_FAILURE");
        else if (IAustralianStockExchange::eNONEXISTENT_ORDER == eCode)
            return std::string ("eNONEXISTENT_ORDER");
        else if (IAustralianStockExchange::eUNKNOWN_STOCK_CODE == eCode)
            return std::string ("eUNKNOWN_STOCK_CODE");
        else if (IAustralianStockExchange::eINVALID_ORDER_PRICE == eCode)
            return std::string ("eINVALID_ORDER_PRICE");
        else if (IAustralianStockExchange::eINVALID_ORDER_VOLUME == eCode)
            return std::string ("eINVALID_ORDER_VOLUME");
        else {
            return std::string ("eUNKNOWN");
        }

    }
};

};

#endif
