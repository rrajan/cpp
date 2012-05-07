/*
 * Author: Ravi Rajan
 */

#ifndef Book_H
#define Book_H

#include <iostream>
#include <map>
#include <sstream>

#include <Types.h>

namespace Exchange {

/* No Locks are needed since exchange is assumed to be single threaded with only one client */
class Book {

    typedef std::map<int, Order::shared_ptr> MyIdOrderHashMap; // Fake HashMap

    struct LevelInfo {
        MyIdOrderHashMap m_orderMap;
        int m_price;
        int m_sumOrders;
        int m_sumVolume;

        LevelInfo(): m_price(0), m_sumOrders(0), m_sumVolume(0) {}

        std::string toString() const {
            std::stringstream ss; 
            ss << "P(" << m_price << ") V(" << m_sumVolume << ") O(" << m_sumOrders << ")";
            return ss.str();
        }
    };

    typedef int LevelKey;
    typedef std::map<LevelKey, LevelInfo> Side;

public:

    enum tCodes {
        eERROR = 0, eADD = 1, eTRADE = 2, eBESTPRICE = 4, eREMOVE = 8 
    };

    Book(): m_init(false), m_trades(0) {}
    Book(const std::string& name): m_name(name), m_init(true), m_trades(0) {}

    bool setName(const std::string& name) {
        if (!m_init) {
            m_name = name;
            return true;
        }
        return false;
    }

    std::string toString()
    {
        std::stringstream ss;

        std::map<int, LevelInfo>::const_iterator bidIter = m_bid.begin();
        std::map<int, LevelInfo>::const_iterator askIter = m_ask.begin();

        int c = 0;
        ss << m_name << std::endl;
        while (bidIter != m_bid.end() || askIter != m_ask.end())
        {
            c++ ;
            ss << "Level: " << c
            << "\tBid: " << ((bidIter != m_bid.end()) ? bidIter->second.toString() : "Empty")
            << "\t\tAsk: " << ((askIter != m_ask.end()) ? askIter->second.toString() : "Empty")
            << std::endl;


            if (bidIter != m_bid.end()) bidIter++;
            if (askIter != m_ask.end()) askIter++;
        }

        return ss.str();
    }

    int addOrder(Order::shared_ptr orderPtr)
    {
        int eCode = eADD;

        int eTrade = match(orderPtr);

        eCode != eTrade;

        /* Add residual order  */
        const Order& order = *orderPtr;
        Side& levels = getSide(order);
        LevelKey lKey = getLevelKey(order);
        LevelInfo& level = (levels)[lKey]; 
        // if level doesn't exist then a default value will be placed
        if (0 == level.m_sumVolume) {
            level.m_price = order.m_price;
        }
        if ( &level == &(levels.begin()->second) ) // any change to top should publish eBESTPRICE
            eCode |= eBESTPRICE;

        level.m_orderMap[order.m_orderIdentifier] = orderPtr;
        level.m_sumOrders++;
        level.m_sumVolume += order.m_volume;

        return eCode;
    }

    int removeOrder(Order::shared_ptr orderPtr)
    {
        int eCode = eREMOVE;
        const Order& order = *orderPtr;
        Side& levels = getSide(order);
        LevelKey lKey = getLevelKey(order);
        LevelInfo& level = levels[lKey];
        // if level doesn't exist then a we could have a problem if we think order does exist
        if (level.m_orderMap.empty() || level.m_orderMap.end() == level.m_orderMap.find(order.m_orderIdentifier))
            return eERROR;

        if ( &level == &(levels.begin()->second) ) // any change to top should publish eBESTPRICE
            eCode |= eBESTPRICE;

        level.m_orderMap.erase(order.m_orderIdentifier); // to enforce no-delete
        level.m_sumOrders--;
        level.m_sumVolume -= order.m_volume;

        if (0 == level.m_sumVolume) {

            levels.erase(lKey);
        }

        return eCode;
    }

    void getTop(int& bidP, int&bidV, int& bidO, int& askP, int& askV, int& askO) {
        if (!m_bid.empty()) {
            LevelInfo& topBid = m_bid.begin()->second;
            bidP = topBid.m_price;
            bidV = topBid.m_sumVolume;
            bidO = topBid.m_sumOrders;
        }
        else {
            bidP = 0;
            bidV = 0;
            bidO = 0;
        }

        if (!m_ask.empty()) {
            LevelInfo& topAsk = m_ask.begin()->second;
            askP = topAsk.m_price;
            askV = topAsk.m_sumVolume;
            askO = topAsk.m_sumOrders;
        }
        else {
            askP = 0;
            askV = 0;
            askO = 0;
        }
    }

protected:
    int match(Order::shared_ptr orderPtr)
    {
        /* Matching engine is not implemented */

        return eERROR;
    }

private:
    LevelKey getLevelKey(const Order& order) {
        return (order.m_bidSide ? -order.m_price : order.m_price);
    }

    Side& getSide(const Order& order) {
        return (order.m_bidSide ? m_bid : m_ask);
    }

    Side& getOppositeSide(const Order& order) {
        return (order.m_bidSide ? m_ask : m_bid);
    }

private:
    Side m_bid; // need tree for sorted (desc order) storage
    Side m_ask; // need tree for sorted storage
    std::string m_name;
    bool m_init;
    int m_trades;
};

};

#endif
