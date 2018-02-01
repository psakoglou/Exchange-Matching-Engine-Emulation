/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Stock Exchange Matching Engine Demo 1:
*		Stress Testing with high volume async requests
*
*/

// Necessary libraries
#include <iostream>
#include "Exchange.hpp"

int main() {

	// The Stock Exchange opens! 
	std::cout << "*** NYSE OPEN ***\n\n";
	Exchange NYSE;

	// What trades are there to fill?
	NYSE.print_available_trades();
	std::cout << "\n\n";

	// Requests to be processed
	long long N = 1000;

	
	// 0. Instantiate timer
	
		std::chrono::system_clock::time_point start, end;


	// 1. Create a large BUY and SELL side on the heap

		// Create one thousand Traders with BUY requests on the heap
		std::vector<Trader*> buyers;
		std::vector<Request*> buy_requests;
		unsigned i = 0;
		for (; i < N; ++i) {
			buyers.push_back(new Trader(100000));
			buy_requests.push_back(new AutoRequest("BUY", "GOOGL", 1.43, 1));
		}


		// Create one thousand Traders with SELL requests on the heap
		std::vector<Trader*> sellers;
		std::vector<Request*> sell_requests;
		i = 0;
		for (; i < N; ++i) {
			sellers.push_back(new Trader(100000));
			sell_requests.push_back(new AutoRequest("SELL", "GOOGL", 1.43, 1));
		}

	

	// 2. Create BUY and SELL side thread pools and launch

		// Create thread pools for requests
		std::vector<std::thread> buy_request_thread_pool;
		std::vector<std::thread> sell_request_thread_pool;

		// Launch BUY and SELL side threads
		
		// Start measuring time, since threads are launching upon creation
		start = std::chrono::system_clock::now();

		i = 0;
		for (; i < N; ++i) {
			buy_request_thread_pool.push_back(std::thread(&Exchange::submit_trade, &NYSE, TradeNode(buyers.at(i), buy_requests.at(i))));
			sell_request_thread_pool.push_back(std::thread(&Exchange::submit_trade, &NYSE, TradeNode(sellers.at(i), sell_requests.at(i))));
		}


	
	// 3. Join all threads to main() and compute elapsed time

		i = 0;
		for (; i < N; ++i) {
			buy_request_thread_pool.at(i).join();
			sell_request_thread_pool.at(i).join();
		}

		// Stop measuring time, since at this time all threads (except main()) have finished execution
		end = std::chrono::system_clock::now();


		long long elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		std::cout << N * 2 << " processed trades. Time Elapsed: " << elapsed_time << " ms\n\n";


	
	// 4. Check the number of filled and requested orders

		std::cout << "Total Trade Requests: "	<< NYSE.getOrderBook().size()		<< "\n";
		std::cout << "Total Filled Trades: "	<< 2 * NYSE.getFillBook().size()	<< "\n\n";	

			// The fill book counts the number of paired requests, that's why we multiply its size by 2 
			// to get the actual value


	// 5. Reclaim memory

		// Free the BUY side
		i = 0;
		for (; i < N; ++i) {
			delete buyers.at(i);
			delete buy_requests.at(i);
		}

		// Free the SELL side
		i = 0;
		for (; i < N; ++i) {
			delete sellers.at(i);
			delete sell_requests.at(i);
		}

		// Success!

	return 0;
}