/*
 * Author: Ravi Rajan
 */

#ifndef Factories_H
#define Factories_H

#include <iostream>
#include <string>
#include <sstream>

#include <IAustralianStockExchange.h>
#include <Book.h>
#include <Types.h>
#include <Util.h>

namespace Exchange {

class OrderManager {

    /* static methods */
public:
    static OrderManager* getOrCreateInstance() {
        /* This method should have a lock in it for thread safety */
        if (NULL == s_instance) {
            s_lock.lock() ;
            // second check just in case there was a race condition
            if (NULL == s_instance)
            {
                s_instance = new OrderManager();
                std::cout << "OrderManager::createOrGetInstance() created new instance" << std::endl;
            }
        } // s_lock will unlock when this stack unwinds

        return s_instance;
    }

    static bool destroy()
    {
        /* This method should have a lock in it for thread safety */
        if (NULL != s_instance) {
            s_lock.lock() ;
            // second check just in case there was a race condition
            if (NULL != s_instance)
            {
                delete s_instance;
                s_instance = NULL;
                std::cout << "OrderManager::destroy() destroyed instance" << std::endl;
            }
        } // s_lock will unlock when this stack unwinds
    }

    /* Global Members */
private:
    static OrderManager* s_instance;
    static ScopedSpinLock s_lock;

    /* Instance Members */
private:
    int m_orders;
    ScopedSpinLock m_lockOrders;
    typedef std::map<std::string, Book> MyStrBookHashMap; // Fake HashMap
    MyStrBookHashMap m_books;
    typedef std::map<int, Order::shared_ptr> MyOidOrderHashMap; // Fake HashMap
    MyOidOrderHashMap m_oidOrderMap; // need HashMap for quick retrieval
    IAustralianStockExchange* m_client; // need set or map if we want to handle multiple clients

    /* Non static methods - can only be accessed by an instance */
public:
    OrderManager(): m_orders(0), m_client(NULL)
    {
        /* include the given symbols */
        addStockCode("BHP");
        addStockCode("RIO");
        addStockCode("ANZ");

        std::cout << "OrderManager() Valid Stock Codes at initialization: "
            << getValidStockCodes() << std::endl;
    }

    ~OrderManager() {
        m_client = NULL; // no delete !
    }

    void registerClient(IAustralianStockExchange* client) {
        m_client = client;
    }

    bool addStockCode(const std::string& stockCode) {
        m_lockOrders.lock();
        return m_books[stockCode].setName(stockCode);
    }

    bool removeStockCode(const std::string& stockCode) {
        m_lockOrders.lock();
        if (!isStockCodeValid(stockCode))
            return false;
        m_books.erase(stockCode);
        return true;
    }

    bool isStockCodeValid(const std::string& stockCode) {
        m_lockOrders.lock();
        return (m_books.find(stockCode) != m_books.end());
    }

    std::string getValidStockCodes() {
        std::stringstream ss;
        for (MyStrBookHashMap::iterator iter = m_books.begin(); iter != m_books.end(); iter++) {
            ss << iter->first << " " ;
        }
        return ss.str();
    }

    std::string getBookString(const std::string& stockCode) {
        return m_books[stockCode].toString();
    }

    /* Could manage a memory pool of orders */
    void createOrder(const std::string& stockCode, bool bidSide, int price, int volume, int internalUseCode) /* Can use shared_ptr for return type */
    {
        m_lockOrders.lock();
        ++m_orders;

        int eCode = testOrderDetails(stockCode, price, volume);
        if (NULL != m_client)
            (m_client->OnOrderAdded)(m_orders, internalUseCode, eCode);
        if (eCode != IAustralianStockExchange::eNO_ERROR) {
            std::cout << "OrderManager:::createOrder invalid order (" << Helper::getErrorCodeStr(eCode) << ")" << std::endl;
            return;
        }

        /* Go ahead and create the Order */
        std::cout << "OrderManager::createOrder #" << m_orders << std::endl;
        Order::shared_ptr orderPtr = new Order(m_orders, internalUseCode, stockCode, bidSide, price, volume) ;
        m_oidOrderMap[m_orders] = orderPtr;

        // Do book keeping
        int eBookCode = m_books[stockCode].addOrder(orderPtr);

        if (Book::eADD & eBookCode) {
            std::cout << m_books[stockCode].toString() << std::endl;

            if (Book::eBESTPRICE & eBookCode) {
                int bidP(-1), bidV(-1), bidO(-1), askP(-1), askV(-1), askO(-1);
                m_books[stockCode].getTop(bidP, bidV, bidO, askP, askV, askO);
                if (NULL != m_client)
                    (m_client->OnBestPriceFeed)(stockCode, bidP, bidV, askP, askV);
            }

            /* If Trade occurred - place holder */
            if (Book::eTRADE & eBookCode) {
                // send callback
            }
        }
    }

    /* Could manage a memory pool of orders */
    void clearOrder(int orderIdentifier)
    {
        m_lockOrders.lock();
        int eCode = IAustralianStockExchange::eNO_ERROR;

        if (m_oidOrderMap.find(orderIdentifier) == m_oidOrderMap.end()) {
            eCode = IAustralianStockExchange::eNONEXISTENT_ORDER;
        }

        // Do book keeping
        Order::shared_ptr orderPtr = m_oidOrderMap[orderIdentifier];
        const std::string& stockCode = orderPtr->m_stockCode;
        int eBookCode = m_books[stockCode].removeOrder(orderPtr);
        if (Book::eERROR & eBookCode)
            eCode = IAustralianStockExchange::eINTERNAL_SYSTEM_FAILURE;

        // Only at this point is the remove clean!
        if (NULL != m_client)
            (m_client->OnOrderRemoved)(orderIdentifier, eCode);

        std::cout << m_books[stockCode].toString() << std::endl;

        if (Book::eBESTPRICE & eBookCode) {
            int bidP(-1), bidV(-1), bidO(-1), askP(-1), askV(-1), askO(-1);
            m_books[stockCode].getTop(bidP, bidV, bidO, askP, askV, askO);
            if (NULL != m_client)
                (m_client->OnBestPriceFeed)(stockCode, bidP, bidV, askP, askV);
        }

        m_oidOrderMap.erase(orderIdentifier);
        delete orderPtr;


    }

private:
    inline int testOrderDetails(const std::string& stockCode, int price, int volume)
    {
        if (!isStockCodeValid(stockCode))
            return IAustralianStockExchange::eUNKNOWN_STOCK_CODE;
        else if (price <= 0)
            return IAustralianStockExchange::eINVALID_ORDER_PRICE;
        else if (volume <= 0)
            return IAustralianStockExchange::eINVALID_ORDER_VOLUME;
        return IAustralianStockExchange::eNO_ERROR;
    }
};

};


#endif
