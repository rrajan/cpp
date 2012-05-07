/*
 * Author: Ravi Rajan
 */

#ifndef AustralianStockExchange_H
#define AustralianStockExchange_H

namespace Exchange {

class IAustralianStockExchange {
public:
    // Public Methods
    virtual ~IAustralianStockExchange() {};
    virtual void AddOrder( const std::string& stockCode, bool bidSide, int price, int volume, int internalUseCode) = 0;
    virtual void RemoveOrder(int orderIdentifier) = 0;

    // Public Events
    void (*OnOrderAdded)(int orderIdentifier, int internalUseCode, int errorCode);
    void (*OnOrderRemoved)(int orderIdentifier, int errorCode);
    void (*OnTradeFeed)(int orderIdentifier, int tradeIdentifier, int internalUseCode, int price, int volume);
    void (*OnBestPriceFeed)(const std::string& stockCode, int bidPrice, int bidVolume, int askPrice, int askVolume);

    // Define error codes that can be returned by implemented call-backs 
    //
    enum tCallBackErrorCodes {
        eNO_ERROR = 0, eINTERNAL_SYSTEM_FAILURE = 1, eNONEXISTENT_ORDER = 2, eUNKNOWN_STOCK_CODE = 3, eINVALID_ORDER_PRICE = 4, eINVALID_ORDER_VOLUME = 5,
    };
};

};

#endif
