/*
 * Author: Ravi Rajan
 */

#ifndef ASEImpl_H
#define ASEImpl_H

#include <iostream>

#include <IAustralianStockExchange.h>
#include <OrderManager.h>
#include <Types.h>
#include <Util.h>

namespace Exchange {

class ASEImpl : public IAustralianStockExchange {
public:

    ASEImpl(bool info=true): m_info(info) {
        /* set up callbacks */
        init();
        /* set up order manager */
        m_orderManager = OrderManager::getOrCreateInstance();
        /* register yourself */
        m_orderManager->registerClient(this);
    }

    ~ASEImpl() {
        m_orderManager = NULL;
        OrderManager::destroy();
    }

    void AddOrder( const std::string& stockCode, bool bidSide, int price, int volume, int internalUseCode) {
        if (m_info)
            std::cout << "ASEImpl::AddOrder requested " << stockCode
            << " internaleUseCode(" << internalUseCode << ") bidSide(" << bidSide
            << ") P(" << price << ") V(" << volume << ")" << std::endl ;

        if (NULL != m_orderManager) {
            m_orderManager->createOrder(stockCode, bidSide, price, volume, internalUseCode);
        }
        else {
            std::cout << "ASEImpl::AddOrder Error: No OrderManager available!" << std::endl;
        }
    }

    void RemoveOrder(int orderIdentifier) {
        if (m_info)
            std::cout << "ASEImpl::RemoveOrder requested orderIdentifier: " << orderIdentifier << std::endl;
        if (NULL != m_orderManager) {
            m_orderManager->clearOrder(orderIdentifier);
        }
        else {
            std::cout << "ASEImpl::RemoveOrder Error: No OrderManager available!" << std::endl;
        }
    }

    /* Helpers */
protected:
    virtual void init()
    {
        if (m_info)
            std::cout << "ASEImpl::init\n";
        OnOrderAdded=&ASEImpl::OnOrderAddedImpl;
        OnOrderRemoved=&ASEImpl::OnOrderRemovedImpl;
        OnTradeFeed=&ASEImpl::OnTradeFeedImpl;
        OnBestPriceFeed=&ASEImpl::OnBestPriceFeedImpl;
    }

    /* Callbacks */
protected:
    static void OnOrderAddedImpl ( int orderIdentifier, int internalUseCode, int errorCode)
    {
        std::cout << "ASEImpl::OnOrderAdded callback"
            << " orderIdentifier(" << orderIdentifier
            << ") internalUseCode(" << internalUseCode
            << ") errorCode: " << Helper::getErrorCodeStr(errorCode)
            << std::endl;
    }

    static void OnOrderRemovedImpl ( int orderIdentifier, int errorCode)
    {
        std::cout << "ASEImpl::OnOrderRemoved callback"
            << " orderIdentifier(" << orderIdentifier
            << ") errorCode: " << Helper::getErrorCodeStr(errorCode)
            << std::endl;
    }

    static void OnTradeFeedImpl (int orderIdentifier, int tradeIdentifier, int internalUseCode, int price, int volume)
    {
        std::cout << "ASEImpl::OnTradeFeed callback"
            << " orderIdentifier(" << orderIdentifier
            << ") tradeIdentifier(" << tradeIdentifier
            << ") internalUseCode(" << internalUseCode 
            << ") price(" << price
            << ") volume(" << volume << ")"
            << std::endl;
    }


    static void OnBestPriceFeedImpl(const std::string& stockCode, int bidPrice, int bidVolume, int askPrice, int askVolume)
    {
        std::cout << "ASEImpl::OnBestPriceFeed callback"
            << " stockCode(" << stockCode
            << ") bidPrice(" << bidPrice
            << ") bidVolume(" << bidVolume 
            << ") askPrice(" << askPrice
            << ") askVolume(" << askVolume << ")"
            << std::endl;
    }

    /* Members */
protected:
    OrderManager* m_orderManager;
    bool m_info;
};


};

#endif
