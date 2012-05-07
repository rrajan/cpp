/*
 * Author: Ravi Rajan
 */

#include <iostream>
#include <string>
#include <sstream>
#include <ASEImpl.h>
#include <map>

using namespace Exchange;

class TestData {
public:
    int id;
    int eCode;
    bool bidSide;
    int bidPrice;
    int bidVolume;
    int askPrice;
    int askVolume;
    std::string stockCode;

    TestData()
    : id(-1), eCode(-1), bidSide(true), bidPrice(-1), bidVolume(-1), askPrice(-1), askVolume(-1), stockCode("NA")
    {}

    TestData(int id, int e)
    : id(id), eCode(e), bidSide(true), bidPrice(-1), bidVolume(-1), askPrice(-1), askVolume(-1), stockCode("NA")
    {}

    TestData(int id, int e, bool b, int bp, int bv, int ap, int av, const std::string& code)
        : id(id), eCode(e), bidSide(b), bidPrice(bp), bidVolume(bv), askPrice(ap), askVolume(av), stockCode(code)
    {}

    bool operator==(const TestData& out) {
        if ( id == out.id &&
            eCode == out.eCode &&
            bidSide == out.bidSide&&
            bidPrice == out.bidPrice &&
            bidVolume == out.bidVolume &&
            askPrice == out.askPrice && 
            askVolume == out.askVolume 
            && (0 == stockCode.compare(out.stockCode)) 
            ) {
            return true;
        }
        return false;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "id (" << id
        << ") errorCode(" << eCode
        << ") bidSide(" << bidSide
        << ") bidPrice(" << bidPrice
        << ") bidVolume(" << bidVolume
        << ") askPrice(" << askPrice
        << ") askVolume(" << askVolume
        << ") " << stockCode;
        return ss.str();
    }
};

class BasicTest : public ASEImpl {
public:

    static int s_errors;
    static int s_orders;
    static int s_internalUseCode;
    static std::map<std::string, TestData> s_state;
    static std::map<int, TestData> s_params;

    BasicTest(bool info=false): ASEImpl(info) {}

    void runTests() {
        overrideCallbacks();
        testOrders();
    }

    /* Add test cases */
private:

    void testOrders() {

        const std::string stockCode("ANZ");
        bool bidSide = true;
        int bidPrice = 0;
        int bidVolume = 0;
        int askPrice = 0;
        int askVolume = 0;
        int eCode = IAustralianStockExchange::eNO_ERROR;

        int price = 100;
        int volume = 10;

        // start
        // expected state
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);

        // bid side

        //iid 1 - Bad stock Code
        ++s_internalUseCode;
        s_params[s_internalUseCode] = TestData(s_internalUseCode, IAustralianStockExchange::eUNKNOWN_STOCK_CODE);
        AddOrder("Unknown", bidSide, price, volume, s_internalUseCode);

        //iid 2 - Bad price
        ++s_internalUseCode;
        s_params[s_internalUseCode] = TestData(s_internalUseCode, IAustralianStockExchange::eINVALID_ORDER_PRICE);
        AddOrder(stockCode, bidSide, 0, volume, s_internalUseCode);

        //iid 3 - Bad volume
        ++s_internalUseCode;
        s_params[s_internalUseCode] = TestData(s_internalUseCode, IAustralianStockExchange::eINVALID_ORDER_VOLUME);
        AddOrder(stockCode, bidSide, price, 0, s_internalUseCode);

        //iid 4 bid level1 - 1st successful order
        ++s_internalUseCode;
        bidPrice = price;
        bidVolume = volume;
        // expected state
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);
        s_params[s_internalUseCode] = TestData(s_internalUseCode, eCode, bidSide, bidPrice, bidVolume, 0, 0, stockCode);
        AddOrder(stockCode, bidSide, price, volume, s_internalUseCode);
        //expect a best price

        //iid 5 bid level2 - 2nd successful order
        ++s_internalUseCode;
        price -= 10;
        volume = 20;
        s_params[s_internalUseCode] = TestData(s_internalUseCode, eCode, bidSide, price, volume, 0, 0, stockCode);
        AddOrder(stockCode, bidSide, price, volume, s_internalUseCode);

        //iid 6 bid level1 (join top) - 3rd successful order
        ++s_internalUseCode;
        price += 10;
        volume = 5;
        bidVolume += volume;
        // expected state
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);
        s_params[s_internalUseCode] = TestData(s_internalUseCode, eCode, bidSide, price, volume, 0, 0, stockCode);
        AddOrder(stockCode, bidSide, price, volume, s_internalUseCode);
        //expect a best price

        // ask side
        bidSide = false;

        //iid 7 ask level1 - 4th successful order
        ++s_internalUseCode;
        price += 10;
        volume = 30;
        askPrice = price;
        askVolume = volume;
        // expected state
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);
        s_params[s_internalUseCode] = TestData(s_internalUseCode, eCode, bidSide, 0, 0, price, volume, stockCode);
        AddOrder(stockCode, bidSide, price, volume, s_internalUseCode);
        // expect best price

        //delete a bid order from top
        bidVolume -= 5;
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);
        RemoveOrder(6); // remove price & volume + 5 (at top)
        // expect best price

        //iid 8 ask level1 (join top) - 5th successful order
        ++s_internalUseCode;
        volume = 5;
        askPrice = price;
        askVolume += volume;
        // expected state
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);
        s_params[s_internalUseCode] = TestData(s_internalUseCode, eCode, bidSide, 0, 0, price, volume, stockCode);
        AddOrder(stockCode, bidSide, price, volume, s_internalUseCode);
        // expect best price

        //iid 8 ask level1 (price improvement) - 6th successful order
        ++s_internalUseCode;
        volume = 15;
        price -= 10;
        askPrice = price;
        askVolume = volume;
        // expected state
        s_state[stockCode] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);
        s_params[s_internalUseCode] = TestData(s_internalUseCode, eCode, bidSide, 0, 0, price, volume, stockCode);
        AddOrder(stockCode, bidSide, price, volume, s_internalUseCode);
        // expect best price

        // try another working symbol and make sure a new book is created
        price=300;
        volume=15000;
        bidPrice=0;
        bidVolume=0;
        askPrice=price;
        askVolume=volume;
        std::string newStock("BHP");
        //iid 9 ask
        ++s_internalUseCode;
        // expected state
        s_state[newStock] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, newStock);
        AddOrder(newStock, false, price, volume, s_internalUseCode);
        // expect best price

        //iid 10 bid - make sure market is crossed
        ++s_internalUseCode;
        price += 70;
        volume += 1000;
        bidPrice = price;
        bidVolume = volume;
        s_state[newStock] = TestData(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, newStock);
        AddOrder(newStock, true, price, volume, s_internalUseCode);
        // expect best price
    }

    virtual void overrideCallbacks()
    {
        std::cout << "BasicTest::overrideCallbacks\n";
        OnOrderAdded=&BasicTest::OnOrderAddedTest;
        OnOrderRemoved=&BasicTest::OnOrderRemovedTest;
        OnTradeFeed=&BasicTest::OnTradeFeedTest;
        OnBestPriceFeed=&BasicTest::OnBestPriceFeedTest;
    }

protected:
    static void OnOrderAddedTest ( int orderIdentifier, int internalUseCode, int errorCode)
    {
        if (IAustralianStockExchange::eNO_ERROR != errorCode) {
            if (orderIdentifier != internalUseCode) {
                s_errors++;
                std::cerr << "BasicTest::OnOrderAdded Failed internalUseCode(" << internalUseCode 
                    << ") != orderIdentifier(" << orderIdentifier << ") error #" << s_errors << std::endl;
            }
            else // check if the error code is expected
            {
                TestData got(orderIdentifier, errorCode); 
                if (!(s_params[orderIdentifier] == got))
                {
                    s_errors++;
                    std::cerr << "BasicTest::OnOrderAdded Failed" << std::endl;
                    std::cerr << "Got: " << got.toString() << std::endl
                        << "Expected: " << s_params[orderIdentifier].toString() << std::endl;
                }
            }
        }
    }

    static void OnOrderRemovedTest ( int orderIdentifier, int errorCode)
    {
        if ( (s_params.find(orderIdentifier) == s_params.end() && errorCode == IAustralianStockExchange::eNO_ERROR) ||
            !(s_params.find(orderIdentifier) == s_params.end() || errorCode == IAustralianStockExchange::eNO_ERROR)
        ) {
            s_errors++;
            std::cerr << "BasicTest::OnOrderRemoved Failed orderIdentifier(" << orderIdentifier << ") error #" << s_errors << std::endl;
        }
    }

    static void OnTradeFeedTest (int orderIdentifier, int tradeIdentifier, int internalUseCode, int price, int volume)
    {
        std::cout << "BasicTest::OnTradeFeed callback" << std::endl;
    }

    static void OnBestPriceFeedTest(const std::string& stockCode, int bidPrice, int bidVolume, int askPrice, int askVolume)
    {
        std::cout << "BasicTest::OnBestPriceFeed callback" << std::endl;

        TestData got(s_internalUseCode, IAustralianStockExchange::eNO_ERROR, true, bidPrice, bidVolume, askPrice, askVolume, stockCode);

        if (!(s_state[stockCode] == got)) {
            s_errors++;
            std::cerr << "BasicTest::OnBestPriceFeed Failed state error #" << s_errors << std::endl;
            std::cerr << "Got: " << got.toString() << std::endl
            << "Expected: " << s_state[stockCode].toString() << std::endl;
        }
    }

};

int BasicTest::s_errors = 0;
int BasicTest::s_internalUseCode = 0;
std::map<std::string, TestData> BasicTest::s_state;
std::map<int, TestData> BasicTest::s_params;

int main()
{
    IAustralianStockExchange* iE = new BasicTest(false);

    dynamic_cast<BasicTest*>(iE)->runTests();

    delete iE;

    std::cerr << "\n\nTest cases executed with " << BasicTest::s_errors << " error(s)\n" << std::endl;
    return (BasicTest::s_errors > 0 ? 1 : 0);
}
