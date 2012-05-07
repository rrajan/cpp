/*
 * Author: Ravi Rajan
 */

#ifndef Types_H
#define Types_H

#include <iostream>
#include <map>
#include <sstream>

namespace Exchange {

class ScopedSpinLock {
public:
    ScopedSpinLock(): myLock(false) {}
    ~ScopedSpinLock() { unlock(); }

    bool lock() {
        if (!myLock) {
            myLock = true;
            return true;
        }
        return false;
    }

    bool isLocked() { return myLock ; }

private:
    bool unlock() {
        if (myLock) {
            myLock = false;
            return true;
        }
        return false;
    }

private:
    bool myLock ; // ideally need spinlock
};

struct Order {

    typedef Order* shared_ptr;

    Order(int orderIdentifier, int internalUseCode, const std::string& stockCode, bool bidSide, int price, int volume) :
        m_orderIdentifier(orderIdentifier),
        m_internalUseCode(internalUseCode),
        m_stockCode(stockCode),
        m_bidSide(bidSide),
        m_price(price),
        m_volume(volume)
    {
        std::cout << "Order::Order created " << toString() << std::endl ;
    }

    std::string toString() {
        std::stringstream ss;

        ss << "orderIdentifier(" << m_orderIdentifier
        << ") internalUseCode(" << m_internalUseCode
        << ") stockCode(" << m_stockCode
        << ") bidSide(" << m_bidSide
        << ") price(" << m_price
        << ") volume(" << m_volume << ")" ;

        return ss.str();
    }

    /* Members */
    int m_orderIdentifier;
    int m_internalUseCode;
    std::string m_stockCode;
    bool m_bidSide;
    int m_price;
    int m_volume;
};


};


#endif
