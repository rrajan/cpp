#include <iostream>
#include <ASEImpl.h>

using namespace Exchange;

int main() {

    IAustralianStockExchange* iE = new ASEImpl();

    const std::string stockCode("ANZ");
    bool bidSide = true;
    int price = 100;
    int volume = 10;
    int internalUseCode = 0;

    // place bad order
    const std::string stockCodeUnknown("Unknown");
    iE->AddOrder(stockCodeUnknown, bidSide, price, volume, ++internalUseCode);

    // bid level1
    bidSide=true;
    iE->AddOrder(stockCode, bidSide, price, volume, ++internalUseCode);
    iE->AddOrder(stockCode, bidSide, price, volume+20, ++internalUseCode);

    iE->RemoveOrder(3);

    // ask level1
    bidSide=false;
    iE->AddOrder(stockCode, bidSide, price+10, volume+10, ++internalUseCode);

    // bid price improvement
    bidSide=true;
    iE->AddOrder(stockCode, bidSide, price+10, volume+5, ++internalUseCode);

    // ask price improvement
    bidSide=false;
    iE->AddOrder(stockCode, bidSide, price+5, volume, ++internalUseCode);

    // ask add level 3
    bidSide=false;
    iE->AddOrder(stockCode, bidSide, price+20, volume+5, ++internalUseCode);
    iE->AddOrder(stockCode, bidSide, price+20, volume+20, ++internalUseCode);

    iE->RemoveOrder(6);

    iE->AddOrder("BHP", bidSide, price+20, volume+50, ++internalUseCode);

    iE->AddOrder(stockCode, bidSide, price+30, volume+40, ++internalUseCode);
    iE->AddOrder(stockCode, bidSide, price+30, volume+140, ++internalUseCode);
    iE->AddOrder(stockCode, bidSide, price+30, volume+90, ++internalUseCode);

    delete iE;

    return 0;
}
