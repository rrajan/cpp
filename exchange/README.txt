Author: Ravi Rajan
Program: Stock Exchange simulator

Usage-
Type the command 'make all' to create the executables './exchange' and './unitTests'.

TestCases -
* Wrote my own testing class ('./test/testCases.cpp'). The program returns 0 if all tests pass and 1 if there is a failure
* Tests for invalid orders, addOrder to both new and existing price levels, RemoveOrder and the respective callbacks including the OnBestPrice callback.
One large function fires all the orders while maintaining a list of orders (associated with orderids) and the current state of the top of the book. These values are used to verify the data held within the simulator.

Design -
1) ASEImpl inherits from IAustralianStockExchange to provide the implementation for the pure virtuals and assigns callbacks.
2) All callbacks are handled within the appropriate methods in ASEImpl.
3) The Core of the design is in the classes OrderManager and Book which simulate the exchange.
Book -
* This acts as the container for all bids and asks currently in the market i.e it represents the current state of existing order for a given stockCode.
* The book is lock-free as for this problem we are given that it is a single threaded app with only one client associated with it.
* The bid and ask levels are stl maps as a tree would provide us with the sorted data structure as per the requirement.
* It performs basic operations such add and remove an order from the bid and ask levels.
(Order matching is not implemented as it was not required. However, a placeholder is provided for the implementation.)
* The result of each operation is passed on to the OrderManager.

OrderManager -
* This is the layer that communicates with the client, the exchange interface.
* A factory returns a singleton instance of the OrderManger, which is not really a requirement as we only have one client.
* Once an order (add/remove) is received from the client, it performs basic sanity checks and creates a unique order number.
It preserves an internal mapping (conceptually by using of a HashMap) of orders and order ids, which would be used while deleting orders.
* It is responsible for validating adds/removes from the book and passing on the appropriate information to the clients.
* The OrderManager stores a HashMap of Books keyed by stockCode.



Comments - HashMaps, Shared pointers and SpinLocks

The code does not depend on any third party library.

HashMap-
The algorithm is conceptually efficient but is implemented with only the best available data structures in STL. For example, the OrderManager intends to maintain a Hashmap of Books keyed by stockCode. For simplicity, this test code use the standard STL map with a mention that this particular data structure would be served better with a HashMap.
For eg: typedef std::map<std::string, Book> MyStrBookHashMap; // Fake HashMap
        MyStrBookHashMap m_books; // This implies the intent of using a HashMap

Shared pointers-
To avoid using boost::shared_ptr and dependencies, I have specified cases where a shared_ptr pointer would be applicable.
Eg: typedef Order* shared_ptr;

SpinLock-
This code uses a simple atomic variable (boolean) for a lock in the singleton. This is just to illustrate its use. Ideally a spinlock would be the best use from a performance consideration.
