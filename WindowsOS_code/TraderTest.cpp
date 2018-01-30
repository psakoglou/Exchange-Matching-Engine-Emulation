/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Testing Trader interface
*
*/

#include <iostream>
#include "Trader.hpp"

int main() {

	std::cout << "*** Testing Trader Objects ***\n\n";

	// Test 1: Instantiate a trader on the stack with V = 1000
	// Print the id and the current portfolio value
	std::cout << "*** Test 1:\n\n";
	Trader t1(1000.0);
	std::cout << "Trader 1: $" << t1.currentValue() << ", Id: " << t1.getId() << "\n\n\n";
	// Success!

	// Test 2: Create 2 traders on the heap with random initial values
	// Compare their ids and check if they are eligible to trade
	std::cout << "*** Test 2:\n\n";
	Trader * t2 = new Trader(100'000);
	Trader * t3 = new Trader(500);

	if (t2->canTrade())
		std::cout << "Trader 2 id: " << t2->getId() << ". Eligible to trade!\n";
	else
		std::cout << "Trader 2 id: " << t2->getId() << ". Not eligible to trade!\n";

	if (t3->canTrade())
		std::cout << "Trader 3 id: " << t3->getId() << ". Eligible to trade!\n";
	else
		std::cout << "Trader 3 id: " << t3->getId() << ". Not eligible to trade!\n";

	if (t2->getId() == t3->getId())
		std::cout << "\nWhoops! The Id system is broken!\n";
	else
		std::cout << "\nGood job!\n" << t2->getId() << " != " << t3->getId() << "\n\n\n";

	// Reclaim memory
	delete t2;
	delete t3;

	// Success!

	// Test 3: Create 2 traders on the heap with random initial values
	// Create a couple of random buy/sell requests and check their portfolio values after each
	// Then, print the margins of each trader
	std::cout << "*** Test 3:\n\n";
	t2 = new Trader(100'000);
	t3 = new Trader(500'000);

	double stock1 = 10.34;
	double stock2 = 34.55;

	long quant1 = 200;
	long quant2 = 130;

	// Trader 2
	std::cout << "Trader 2 V: $" << t2->currentValue() << "\n";
	t2->sell(stock1, quant2);
	std::cout << "Trader 2 sells: $" << stock1 * (double)quant2 << ". V: $" << t2->currentValue() << "\n";
	t2->buy(stock1, quant1);
	std::cout << "Trader 2 buys: $" << stock1 * (double)quant1 << ". V: $" << t2->currentValue() << "\n";
	t2->buy(stock2, quant1);
	std::cout << "Trader 2 buys: $" << stock2 * (double)quant1 << ". V: $" << t2->currentValue() << "\n\n";

	// Trader 3
	std::cout << "Trader 3 V: $" << t3->currentValue() << "\n";
	t3->sell(stock2, quant1);
	std::cout << "Trader 3 sells: $" << stock2 * (double)quant1 << ". V: $" << t3->currentValue() << "\n";
	t3->buy(stock1, quant1);
	std::cout << "Trader 3 buys: $" << stock1 * (double)quant1 << ". V: $" << t3->currentValue() << "\n";
	t3->buy(stock2, quant2);
	std::cout << "Trader 3 buys: $" << stock2 * (double)quant2 << ". V: $" << t3->currentValue() << "\n\n";

	// Print margins
	auto t2_margins = t2->getMargins();
	auto t3_margins = t3->getMargins();

	std::cout << "Trader 2 margins:\n";
	for (double & e : t2_margins)
		std::cout << e << " ";
	std::cout << "\n\n";

	std::cout << "Trader 3 margins:\n";
	for (double & e : t3_margins)
		std::cout << e << " ";
	std::cout << "\n\n\n";

	delete t2;
	delete t3;
	
	// Success!

	// Test 4: Create a trader on the stack with $500'000 and keep buying until 
	// is not eligible to trade. Then print the margins
	std::cout << "*** Test 4:\n\n";
	Trader t4(500'000);

	double stock = 40.0;
	long quant = 1000;

	double trade_val = stock * (double)quant;
	
	while (t4.canTrade()) {
		t4.buy(stock, quant);
		std::cout << "Trader 4 with Id: " << t4.getId() << " buys: $" << trade_val << ". V = " << t4.currentValue() << "\n";
		if (!t4.canTrade() || t4.currentValue() < trade_val)
			break;
	}

	std::cout << "Trader 4 has now V = " << t4.currentValue() << "\n\n";

	// Get the margins
	auto t4_margins = t4.getMargins();

	std::cout << "Trader 4 margins:\n";
	for (double & e : t4_margins)
		std::cout << e << " ";
	std::cout << "\n\n\n";

	// Success!

	// Test 5: Try to print the margins and info of a trader without any transactions
	std::cout << "*** Test 5:\n\n";
	Trader * t5 = new Trader(2000);

	std::cout << "Trader 5 info: \n";
	t5->info();

	auto t5_margins = t5->getMargins();
	std::cout << "\nTrader 5 margins:\n";
	for (double & e : t5_margins)
		std::cout << e << " ";
	std::cout << "\n\n\n";

	delete t5;

	// Success! 

	return 0;
}