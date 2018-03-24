/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  =================================================
*
*	Exchange class implementation
*
*/

#include "Exchange.hpp"

//*** Constructor, Destructor, and Matching Engine methods ***//

// Default constructor opens the Exchange: instantiates the hast table on heap, 
// initializes a hash function, and starts the matching engine
Exchange::Exchange() : m_index(0) {

	// The size of the hash table with the ExchangeNodes is the number of 
	// available stocks at the opening
	m_size = Stocks.size();

	// Hard-coded hash function. Alternatively we can use std::hash with mod m_size, however
	// this might create hashing bugs and collisions. Aa good hash function is an entirely different
	// problem by itself and open to research, thus for the sake of the demo we hard-code a safe
	// hashing method using a lambda object
	hash = [&](std::string stock) {
		if (stock == "GOOGL")
			return 0;
		if (stock == "AMZN")
			return 1;
		if (stock == "TSLA")
			return 2;
		if (stock == "DIS")
			return 3;
		if (stock == "BABA")
			return 4;
	};

	// Create the hash table (dynamic array)
	m_exchange = new ExchangeNode[m_size];

	// Flag the engine and launch engine thread
	exchange_open = true;
	start_engine();
}

// Private method that launches the matching engine on the background
// It runs parallely, letting the brokers/traders to submit requests at all times
// This might be a little expensive for the CPU but will certainly payoff for high volumes
// of incoming requests
void Exchange::start_engine() {
	ignite = std::thread{ &Exchange::matching_engine, this };
}

// Private method that stops the engine. It is called in the destructor
// and the main thread of execution waits for all requests to be completed
// before de-allocates the Exchange, thus preventing a crash and memory segmentation
void Exchange::stop_engine() {
	ignite.join();
}

// Destructor is responsible to stop the matching engine
// and reclaim the allocated memory
Exchange::~Exchange() {
	exchange_open = false;
	stop_engine();
	delete[] m_exchange;
}

//*** Matching Engine Implementation ***//

// The matching engine will run in the background as long as the
// exchange is open, and will constantly be checking for available trades
// anywhere in the exchange. In case we want more matching engines, we can 
// launch more threads but also install mutex mechanism to the other components
void Exchange::matching_engine() {

	// While the exchange is open ...
	while (exchange_open) {

		// ... iterate across the directory ...
		unsigned i = 0;
		for (; i < m_size; ++i) {

			// ... and check for each one whether or not there are available trades.
			if (m_exchange[i].available) {


				// If there are trades to be executed ...
				if (!m_exchange[i].buy_heap.empty() && !m_exchange[i].sell_heap.empty()) {

					// ... check the prices of SELL and BUY orders to see if the trade is possible.
					TradeNode buy_order = m_exchange[i].buy_heap[0];
					TradeNode sell_order = m_exchange[i].sell_heap[0];

					double buy_price = buy_order.request->getPrice();
					double sell_price = sell_order.request->getPrice();

					if (buy_price < sell_price)
						continue;

					double trade_price = 0.0;

					if (buy_price > sell_price)
						if (buy_order.submit_id < sell_order.submit_id)
							trade_price = buy_price;
						else
							trade_price = sell_price;
					

					// Get quantities
					long buy_quant = buy_order.request->getQuantity();
					long sell_quant = sell_order.request->getQuantity();

					bool buy_status;
					bool sell_status;

					// If demand meets supply or when buyer wants more
					if (buy_quant >= sell_quant) {

						// Attempt to perform the trade
						buy_status = buy_order.trader->buy(trade_price, sell_quant);
						sell_status = sell_order.trader->sell(trade_price, sell_quant);

						// Reimburse the trader if the other doesn't fall through
						if (buy_status == true && sell_status == false)
							buy_order.trader->reimburse(trade_price * (double)sell_quant);

						if (buy_status == false && sell_status == true)
							sell_order.trader->reimburse(trade_price * (double)sell_quant);

						// If trade is executed successfully
						if (buy_status && sell_status) {

							// Remove the trades
							auto buyer = m_exchange[i].buy_heap[0];
							buyer.request->setQuantity(buy_quant - sell_quant);

							if (buyer.request->getQuantity() == 0)
								m_exchange[i].buy_heap.pop();

							auto seller = m_exchange[i].sell_heap.pop();

							// Update the Fill book
							std::stringstream ss;
							auto rd1 = buyer.request->getData();
							auto rd2 = seller.request->getData();

							ss << "* Trader: " << buyer.trader->getId() << "\nORDER: " << std::get<0>(rd1)
								<< ", " << std::get<1>(rd1) << ", $" << trade_price
								<< ", " << std::get<3>(rd1) << ", " << std::get<4>(rd1)
								<< "\n* Trader: " << seller.trader->getId() << "\nORDER: " << std::get<0>(rd2)
								<< ", " << std::get<1>(rd2) << ", $" << trade_price
								<< ", " << std::get<3>(rd2) << ", " << std::get<4>(rd2);
							FillBook.push_back(ss.str());

							// Update ExchangeNode as per the availability there
							if (m_exchange[i].buy_heap.empty() && m_exchange[i].sell_heap.empty())
								m_exchange[i].available = false;
						}
					}

					// If seller wants more
					if (buy_quant < sell_quant) {

						// Attempt to perform the trade
						buy_status = buy_order.trader->buy(trade_price, buy_quant);
						sell_status = sell_order.trader->sell(trade_price, buy_quant);

						// Reimburse the trader if the other doesn't fall through
						if (buy_status == true && sell_status == false)
							buy_order.trader->reimburse(trade_price * (double)buy_quant);

						if (buy_status == false && sell_status == true)
							sell_order.trader->reimburse(trade_price * (double)buy_quant);

						// If trade is executed successfully
						if (buy_status && sell_status) {

							// Remove the trades
							auto buyer = m_exchange[i].buy_heap.pop();
							auto seller = m_exchange[i].sell_heap[0];
							seller.request->setQuantity(sell_quant - buy_quant);

							if (seller.request->getQuantity() == 0)
								m_exchange[i].sell_heap.pop();

							// Update the Fill book
							std::stringstream ss;
							auto rd1 = buyer.request->getData();
							auto rd2 = seller.request->getData();

							ss << "* Trader: " << buyer.trader->getId() << "\nORDER: " << std::get<0>(rd1)
								<< ", " << std::get<1>(rd1) << ", $" << trade_price
								<< ", " << std::get<3>(rd1) << ", " << std::get<4>(rd1)
								<< "\n* Trader: " << seller.trader->getId() << "\nORDER: " << std::get<0>(rd2)
								<< ", " << std::get<1>(rd2) << ", $" << trade_price
								<< ", " << std::get<3>(rd2) << ", " << std::get<4>(rd2);
							FillBook.push_back(ss.str());

							// Update ExchangeNode as per the availability there
							if (m_exchange[i].buy_heap.empty() && m_exchange[i].sell_heap.empty())
								m_exchange[i].available = false;
						}
					}
				}
			}
		}
	}
}

//*** Auxiliary Methods for a Stock Exchange ***//

// Method that prints all available trades 
void Exchange::print_available_trades() {
	unsigned i = 0;
	bool no_trades = true;
	for (; i < m_size; ++i)
		if (m_exchange[i].available) {
			std::cout << "Available: " << m_exchange[i].stock << "\n";
			no_trades = false;
		}
	if (no_trades)
		std::cout << "No trades to fill!";
}

// Getter method that returns the order book
const std::vector<std::string> Exchange::getOrderBook() {
	return OrderBook;
}

// Wrapper method to update the order book. Converts all request 
// input to a string upon successful submission
void Exchange::updateOrderBook(TradeNode & tn) {

	std::stringstream ss;
	auto rd = tn.request->getData();

	// Convert to string
	ss	<< "Trader: "	<< tn.trader->getId()	<< "\nORDER: "	<< std::get<0>(rd)
		<< ", "			<< std::get<1>(rd)		<< ", "			<< std::get<2>(rd)
		<< ", "			<< std::get<3>(rd)		<< ", "			<< std::get<4>(rd);

	// Submit to the book
	OrderBook.push_back(ss.str());
}

// Getter method for the Fill book that holds all successfully executed orders
const std::vector<std::string> Exchange::getFillBook() {
	return FillBook;
}

//*** Modifiers ***//

// Editing an existing trade -- change the price
void Exchange::edit_trade_price(Trader * t, Request * r, std::string side, std::string instrument, double new_price) {
	unsigned i = hash(instrument);
	if (!(i >= 0 && i <= 4))
		return;

	if (side == "BUY") {
		unsigned j = 0;
		while (j < m_exchange[i].buy_heap.size()) {
			if (t->getId() == m_exchange[i].buy_heap[j].trader->getId() && r->getId() == m_exchange[i].buy_heap[j].request->getId()) {
				m_exchange[i].buy_heap[j].request->setPrice(new_price);
				m_exchange[i].buy_heap.sort();
				return;
			}
			++j;
		}
		if (j == m_exchange[i].buy_heap.size())
			return;
	}
	

	if (side == "SELL") {
		unsigned j = 0;
		while (j < m_exchange[i].sell_heap.size()) {
			if (t->getId() == m_exchange[i].sell_heap[j].trader->getId() && r->getId() == m_exchange[i].sell_heap[j].request->getId()) {
				m_exchange[i].sell_heap[j].request->setPrice(new_price);
				m_exchange[i].sell_heap.sort();
				return;
			}
			++j;
		}
		if (j == m_exchange[i].sell_heap.size())
			return;
	}
	return;
}

// Editing an existing trade -- change the quantity
void Exchange::edit_trade_quantity(Trader * t, Request * r, std::string side, std::string instrument, long new_quantity) {
	unsigned i = hash(instrument);
	if (!(i >= 0 && i <= 4))
		return;

	if (side == "BUY") {
		unsigned j = 0;
		while (j < m_exchange[i].buy_heap.size()) {
			if (t->getId() == m_exchange[i].buy_heap[j].trader->getId() && r->getId() == m_exchange[i].buy_heap[j].request->getId()) {
				if (new_quantity < m_exchange[i].buy_heap[j].request->getQuantity()) {
					m_exchange[i].buy_heap[j].request->setQuantity(new_quantity);
					m_exchange[i].buy_heap.sort();
					return;
				} 
				else {
					m_exchange[i].buy_heap[j].request->setQuantity(new_quantity);
					return;
				}
			}
			++j;
		}
		if (j == m_exchange[i].buy_heap.size())
			return;
	}


	if (side == "SELL") {
		unsigned j = 0;
		while (j < m_exchange[i].sell_heap.size()) {
			if (t->getId() == m_exchange[i].sell_heap[j].trader->getId() && r->getId() == m_exchange[i].sell_heap[j].request->getId()) {
				if (new_quantity < m_exchange[i].sell_heap[j].request->getQuantity()) {
					m_exchange[i].sell_heap[j].request->setQuantity(new_quantity);
					m_exchange[i].sell_heap.sort();
					return;
				}
				else {
					m_exchange[i].sell_heap[j].request->setQuantity(new_quantity);
					return;
				}
			}
			++j;
		}
		if (j == m_exchange[i].sell_heap.size())
			return;
	}
	return;
}

// Deleting an existing trade
void Exchange::delete_trade(Trader * t, Request * r, std::string side, std::string instrument) {
	unsigned i = hash(instrument);
	if (!(i >= 0 && i <= 4))
		return;

	if (side == "BUY") {
		unsigned j = 0;
		while (j < m_exchange[i].buy_heap.size()) {
			if (t->getId() == m_exchange[i].buy_heap[j].trader->getId() && r->getId() == m_exchange[i].buy_heap[j].request->getId()) {
				m_exchange[i].buy_heap.remove(t, r);
				if (m_exchange[i].buy_heap.empty() && m_exchange[i].sell_heap.empty())
					m_exchange[i].available = false;
				return;
			}
			++j;
		}
		if (j == m_exchange[i].buy_heap.size())
			return;
	}

	if (side == "SELL") {
		unsigned j = 0;
		while (j < m_exchange[i].sell_heap.size()) {
			if (t->getId() == m_exchange[i].sell_heap[j].trader->getId() && r->getId() == m_exchange[i].sell_heap[j].request->getId()) {
				m_exchange[i].sell_heap.remove(t, r);
				if (m_exchange[i].buy_heap.empty() && m_exchange[i].sell_heap.empty())
					m_exchange[i].available = false;
				return;
			}
			++j;
		}
		if (j == m_exchange[i].sell_heap.size())
			return;
	}
	return;
}
