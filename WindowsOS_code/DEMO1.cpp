/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Stock Exchange Matching Engine Demo 1: 
*		A simple trade submission and execution
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

	// Traders open their accounts to start trading
	Trader * t1 = new Trader(1200);
	Trader * t2 = new Trader(1200);

	Trader * t3 = new Trader(10000);
	Trader * t4 = new Trader(10000);

	Trader * t5 = new Trader(100000);
	Trader * t6 = new Trader(100000);

	// Traders are initializing requests
	Request * r1 = new AutoRequest("BUY", "GOOGL", 10, 10);
	Request * r2 = new AutoRequest("SELL", "GOOGL", 10, 10);

	Request * r3 = new AutoRequest("BUY", "AMZN", 100, 10);
	Request * r4 = new AutoRequest("SELL", "AMZN", 20, 20);

	Request * r5 = new AutoRequest("BUY", "DIS", 20, 100);
	Request * r6 = new AutoRequest("SELL", "BABA", 20, 20);

	// Traders are submitting their trades asynchronously


	// Scenario 1
	// ==========

	// Trader 1: BUY GOOGL $10 10 Request -> current V = $1200, after filling V = $1100
	std::thread submission1(&Exchange::submit_trade, &NYSE, TradeNode(t1, r1));

	/* Now GOOGL is available */

	// Trader 2: SELL GOOGL $10 10 Request -> current V = $1200, after filling V = $1300
	std::thread submission2(&Exchange::submit_trade, &NYSE, TradeNode(t2, r2));

	/* After executing, GOOGL is anavailable */


	// Scenario 2
	// ==========

	// Trader 3: BUY AMZN $100 10 -> current V = $10,000, after filling V = $9,000
	std::thread submission3(&Exchange::submit_trade, &NYSE, TradeNode(t3, r3));

	/* Now AMZN is available */

	// Trader 4: SELL AMZN $20 20 -> current V = $10,000, after filling V = $10,400
	std::thread submission4(&Exchange::submit_trade, &NYSE, TradeNode(t4, r4));

	/* There are still 10 AMZN Stocks available at $20 */


	// Scenario 3
	// ==========

	// Trader 4: BUY DIS $20 100 -> current V = $100,000, after filling V = $98,000
	std::thread submission5(&Exchange::submit_trade, &NYSE, TradeNode(t5, r5));

	/* Now DIS is available */

	// Trader 5: SELL BABA $20 20 -> current V = $100,000 after filling V = $100,400
	std::thread submission6(&Exchange::submit_trade, &NYSE, TradeNode(t6, r6));

	/* DIS and BABA are available*/


	// Make sure the orders have been submitted
	submission1.join();
	submission2.join();
	submission3.join();
	submission4.join();
	submission5.join();
	submission6.join();

	// Delete the last two trades
	NYSE.delete_trade(t6, r6, "SELL", "BABA");
	NYSE.delete_trade(t5, r5, "BUY", "DIS");

	// Only AMZN is available now

	// Edit the existing AMZN order to ask for $1000
	NYSE.edit_trade_price(t4, r4, "SELL", "AMZN", 1000);
	
	// Check remaining available trades
	std::cout << "\n***\n";
	std::cout << "After some requests, the available stocks are:\n";
	NYSE.print_available_trades();
	std::cout << "\n\n\n";

	auto order_book = NYSE.getOrderBook();
	auto fill_book = NYSE.getFillBook();

	// Print the trader accounts and all submissions
	std::cout << "*** NYSE CLOSED ***\n\n";

	std::cout << "Statistics:\n";

	std::cout << "*** Submitted orders: " << order_book.size() << "\n";
	for (auto e : order_book)
		std::cout << e << "\n\n";
	std::cout << "\n";

	std::cout << "*** Filled orders: " << fill_book.size() << "\n";
	for (auto e : fill_book)
		std::cout << e << "\n\n";
	std::cout << "\n\n\n";

	std::cout << "Trader 1: ";	// Bought $10x10 = 100 from Trader 2 -> Original V = $1100, Final V = $1100
	t1->info();
	std::cout << "\n\n";

	std::cout << "Trader 2: ";	// Sold $10x10 = 100 to Trader 1 -> Original V = $1100, Final V = $1300
	t2->info();
	std::cout << "\n\n";

	// Success!

	std::cout << "Trader 3: ";	// Bought at $60x10 from Trader 4 -> Original V = $10,000, Final V = $9,400
	t3->info();
	std::cout << "\n\n";

	std::cout << "Trader 4: ";	// Sold at $60x10 to Trader 3 -> Original V = $10,000, Final V = $10,600
	t4->info();
	std::cout << "\n\n";

	// Success!

	std::cout << "Trader 5: ";	// Didn't buy anything -> Original V = $100,000, Final V = $100,000
	t5->info();
	std::cout << "\n\n";

	std::cout << "Trader 6: ";	// Didn't sell anything -> Original V = $100,000, Final V = $100,000
	t6->info();
	std::cout << "\n\n";

	// Success!
	

	// Reclaim all memory
	delete t1; delete t2; delete t3; delete t4; delete t5; delete t6;
	delete r1; delete r2; delete r3; delete r4; delete r5; delete r6;

	return 0;
}