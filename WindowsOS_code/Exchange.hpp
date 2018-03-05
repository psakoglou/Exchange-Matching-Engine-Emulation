/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ===============================================
*
*	Exchange class definition
*
*/

// Multiple inclusion guards to avoid linker errors
// In case this file is included in multiple source files
// we want to avoid re-compilations
#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

// Necessary libraries
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <mutex>
#include <condition_variable>
#include <functional>	
#include <thread>

#include "TradeHeap.hpp"

//*** ExchangeNode data structure ***//

// This is a data structure that models a particular equity in the stock market
// Every trading requests that refer to a stock will be handled by the ExchangeNode
// of that stock. It consists of the stock name, an availability indicator that holds 
// true when there is at least one available trade there, and two TradeHeap objects
// whose purpose is to sort and handle all requests appropriately. 
struct ExchangeNode {
	std::string		stock;
	TradeHeap		buy_heap;		// Buy requests will be stored here
	TradeHeap		sell_heap;		// Sell requests will be stored here
	bool			available;

	ExchangeNode() : available(false), stock("") {}
};

//*** Exchange class ***//

// This class encapsulates a naive version of a Stock Exchange. One can submit trading
// requests to the Stock Exchange, and its the responsibility of the Exchange's matching 
// engine to handle all requests and execute those which are possible.
// As a result, the matching engine is ignited upon opening of the Stock Exchange and 
// continuously runs in the background until the Stock Exchange closes for the day. 
// All remaining requests that are not executed remain in the database (in our case are discarded)
// Due to the high volume of requests in an exchange, we need to protect the submit method
// from two or more brokers (threads) trying to submit at the same time to the exchange. In order
// submission is required, thus we are using STL's mutual exclusion mechanisms and condition variables.
// Additionally, all the matching engine methods are encapsulated (declared private) as its only
// the Exchange's responsibility to execute requests. In a case of a dark pool, that might not hold.
class Exchange {
public:

	// Default constructor opens the Exchange and launches the matching engine
	// There is heap allocation, since new stocks (IPOs) are coming daily and the 
	// number of total Stocks is variable
	Exchange();

	// closes the Ctock Exchange and waits for the matching engine to finish execution
	// Then it reclaims memory
	~Exchange();

	// Iterates the hash table and prints all available trade information
	void print_available_trades();

	// Returns the Order book
	const std::vector<std::string> getOrderBook();

	// Returns the Fill book
	const std::vector<std::string> getFillBook();

	// Edit trade
	void edit_trade_price(Trader * t, Request * r, std::string side, std::string instrument, double new_price);
	void edit_trade_quantity(Trader * t, Request * r, std::string side, std::string instrument, long quantity);

	// Delete trade
	void delete_trade(Trader * t, Request * r, std::string side, std::string instrument);

	// Submit trade method. This method takes a TradeNode object reference cause
	// we want the traders' accounts to be updated after a trade is executed by the matchine engine.
	// It implements elementary mutex mechanisms to hedge against multiple requests, 
	// and logs every successful trade request to the Order Book.
	// Like in other files, the reason we inline this function is for better performance 
	// We want the requests to be submitted as fast as possible
	inline bool submit_trade(TradeNode & tn) {
		std::unique_lock<std::mutex> lock(mt);

		// Get the stock name
		std::string input_stock = tn.request->getInstrument();

		// Check if that stock is available in O(log n) time. 
		// If not, print an error message, release the lock and notify waiting threads
		if (Stocks.find(input_stock) == Stocks.end()) {
			std::cerr << "Bad trade request! Stock doesn't exist.\n";
			lock.unlock();
			cv.notify_all();
			return false;
		}

		// Get the index by hashing the stock name
		// This allows constant time querries to the exchange
		m_index = hash(input_stock);
		
		// Get the trading side
		std::string side = tn.request->getSide();

		// Submit a BUY order
		if (side == "BUY") {
			m_exchange[m_index].buy_heap.push(tn);
			m_exchange[m_index].available	= true;
			m_exchange[m_index].stock		= input_stock;
			updateOrderBook(tn);
			lock.unlock();
			cv.notify_all();
			return true;
		}

		// Submit a SELL order
		if (side == "SELL") {
			m_exchange[m_index].sell_heap.push(tn);
			m_exchange[m_index].available	= true;
			m_exchange[m_index].stock		= input_stock;
			updateOrderBook(tn);
			lock.unlock();
			cv.notify_all();
			return true;
		}

		// Release the lock in all cases!
		lock.unlock();
		cv.notify_all();
		return false;
	}

private:
	// Model a hash table using a dynamic array and an elementary hash function
	ExchangeNode*								m_exchange;
	std::size_t									m_size;
	unsigned int								m_index;
	std::function<std::size_t(std::string)>		hash;
	std::set<std::string>						Stocks = { "GOOGL", "BABA", "AMZN", "TSLA", "DIS" };

	// Threading shield
	std::mutex					mt;
	std::condition_variable		cv;
	std::thread					ignite;

	// Matching Engine stuff
	bool exchange_open;

	void matching_engine();
	void start_engine();
	void stop_engine();

	// Order Book
	std::vector<std::string> OrderBook;
	void updateOrderBook(TradeNode & tn);

	// Fill Book
	std::vector<std::string> FillBook;
	
	// Avoid accidental or intentional copies and clones of the Exchange
	Exchange(const Exchange&);
	Exchange& operator=(const Exchange&);
};

#endif // !EXCHANGE_HPP

