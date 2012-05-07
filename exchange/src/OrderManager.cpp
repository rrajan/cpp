#include <OrderManager.h>

namespace Exchange {

OrderManager* OrderManager::s_instance = NULL;
ScopedSpinLock OrderManager::s_lock;

};
