/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	TradeHeap implementation
*
*/

// Multiple inclusion guards to avoid linker errors
// In case this file is included in multiple source files
// we want to avoid re-compilations
#ifndef TRADE_HEAP_HPP
#define TRADE_HEAP_HPP

// Additional headers to be used below:
//	1) Trader class
//	2) Request class
#include "Trader.hpp"
#include "Request.hpp"

#include <iostream>
#include <chrono>	// System clock

//*** TradeNode data structure ***//

// This data structure holds the trading information of a trade
// and will be treated as a trading entiry from the exchange and the
// matching engine
struct TradeNode {
	Trader*		trader;			// Pointer to a Trader instance to be determined
	Request*	request;		// Pointer to a trading Request instance
	long long	submit_id;		// Timestamp of filing a request in the exchange

	// Default constructor sets the pointers to nullptr, and the id to -1
	TradeNode() : trader(nullptr), request(nullptr), submit_id(-1) {}

	// Parameter constructor for convenience
	TradeNode(Trader * t, Request * r) : trader(t), request(r), submit_id(-1) {}
};

//*** TradeHeap class ***//

// This data structure emulates a priority queue (max heap) 
// where any request that is inserted goes to a position that
// describes its execution priority as per its price. The highest the 
// price, the higher the priority. This holds for both BUY and SELL requests
// Elements to be extracted and executed from the priority queue are always the 
// ones at index 0 i.e. at the head of the priority queue
// The implementation below is in native C++, using a dynamic array of TradeNode type
// and is an alternative of the STL's std::priority_queue, with similar functionality.
// Similar to std::priority_queue, the insertion happens in a sorted manner as per
// the requirements of the Exchange. If two or more elements have the same key value
// (price in our case) then the earlier inserted element has priority i.e. is at a 
// lower index in the queue.
// Another alternative implementation of the below heap -- and perhaps a more efficient one,
// would use a native C doubly linked list (or std::list in case of STL) with similar overloaded
// interface to fit our purposes
class TradeHeap {
public:
	// Constructors allocate on the heap, since the number of 
	// incoming requests in variable and we want the system to
	// handle high trading volumes without the risk of stack overflow
	TradeHeap();
	TradeHeap(const TradeHeap & th);

	// The destructor reclaims the allocated memory once the heap is no 
	// longer used. 
	~TradeHeap();

	// Overloaded operator[] for convenience. It implements
	// an elementary bounds checking, but no exceptions are thrown
	// A good idea is to extend the current system to report errors
	// via an error handling component -- which can be added later
	const TradeNode operator[](unsigned index);

	// Auxiliary features for sanity check, to prevent errors, and for
	// demo convenience and implementation of later components
	const std::size_t size();	// Returns the current capacity of the heap
	void print();				// Iterates and prints in-order the elements
	bool empty();				// Checks if the heap is empty
	
	// Inline pop() method that returns the element at the head of the heap 
	// This is the equivalent method of EXTRACT-MAX() 
	// This method is inlined on purpose for performance reasons.
	// The benefit of an inline function is that at run time every call
	// to this method will only have to look at the class definition instead
	// of searching to the source file for its implementation. The disadvantage
	// is that it might make the compilation process slightly slower since it
	// makes the source code slightly heavier. Nevertheless, in an Exchange we
	// care more about performance than compilation time since every order needs
	// to be executed and files as soon as possible
	inline const TradeNode pop() {

		// If there are no elements on the heap, return a default TradeNode object
		// which will be handled and eventually ignored by another system
		if (m_index == 0) {
			return TradeNode();
		}

		TradeNode temp = m_trades[0];		
		unsigned i = 0;
		for (; i < m_index - 1; ++i) {
			m_trades[i] = m_trades[i + 1];
		}
		--m_index;

		// To improve runtime performance and prevent unecessary overhead
		// we resize (shrink the heap) dynamically when the number of 
		// elements is equal or less than half of the current capacity of the heap
		if ((double)m_index <= 0.5 * (double)m_size)
			shrink();
		return temp;
	}

	// The push() method is the equivalent of the INSERT() method, and for the 
	// same reasons as with pop(), it's implemented inlined. 
	// It takes a TradeNode reference for input and inserts in-place as per the
	// underlying rule: highest price goes first and if two prices (keys) are same
	// the older elements of the queue have priority
	inline void push(TradeNode & trn) {

		// Case heap is empty
		if (m_index == 0) {
			trn.submit_id = std::chrono::system_clock::now().time_since_epoch().count();
			m_trades[m_index] = trn;
			++m_index;
			return;
		}

		// To improve runtime performance and prevent unecessary overhead
		// we resize (expand the heap) dynamically when the number of 
		// elements is equal or greater than 80% of the current capacity of the heap
		if ((double)m_index >= 0.8 * (double)m_size) 
			expand();
		
		// Store input price locally to avoid repeated calls that would drop performance
		double input_price = trn.request->getPrice();

		// Iterate the heap to find the right index to push the object
		unsigned i = 0;
		for (; i < m_index; ++i) {
			double index_price = m_trades[i].request->getPrice();
			if (index_price <= input_price) 
				break;
		}

		// Case where input has lowest price
		if (i == m_index) {
			trn.submit_id = std::chrono::system_clock::now().time_since_epoch().count();
			m_trades[m_index] = trn;
			++m_index;
			return;
		}

		// If two or more keys have the same value (requests with same price)
		// keep iterating until you find the first smaller key
		if (m_trades[i].request->getPrice() == input_price && i < m_index) {
			do {
				++i;
				if (i == m_index)
					break;
			} while (m_trades[i].request->getPrice() == input_price);
		}

		// Insert in the right place
		unsigned j = m_index;
		for (; j > i + 1; --j) {
			m_trades[j] = m_trades[j - 1];
		}
		m_trades[j] = m_trades[i];
		trn.submit_id = std::chrono::system_clock::now().time_since_epoch().count();
		m_trades[i] = trn;
		++m_index;
		return;
	}

private:
	// Private members
	TradeNode*		m_trades;	// The heap (dynamic descending array)
	std::size_t		m_size;		// Capacity of heap
	unsigned int		m_index;	// Index (number of elements)

	static std::size_t	default_size;	// Default initial capacity for all heaps

	// Private methods to resize the heap. Only methods of the class are allow to 
	// perform such operations since they are expensive to perform and should be
	// only used when and if necessary
	void expand();
	void shrink();

	// To avoid security loopholes, we set the assignment operator as private
	TradeHeap& operator=(const TradeHeap & th);
};

#endif // !TRADE_HEAP_HPP
