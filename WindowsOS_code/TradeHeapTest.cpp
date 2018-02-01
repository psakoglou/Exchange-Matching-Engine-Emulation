/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Testing the TradeHeap class
*
*/

// Import the necessary files
#include <iostream>
#include "TradeHeap.hpp"

int main() {

	std::cout << "*** Testing TradeHeap functionality ***\n\n";

	// Test 1: Create a TradeHeap on the STACK and attempt to call the print method
	// Check the current capacity and confirm it's empty
	std::cout << "*** Test 1:\n\n";
	TradeHeap theap1;

	theap1.print(); // Nothing will be printed

	std::cout << "Capacity: " << theap1.size() << "\n";
	std::cout << "Is heap empty? " << std::boolalpha << theap1.empty() << "\n\n\n";

	// Success!

	// Test 2: Create a TradeHeap on the HEAP and attempt to call the print method
	// Check the current capacity and confirm it's empty
	std::cout << "*** Test 2:\n\n";
	TradeHeap * theap2 = new TradeHeap();

	theap2->print(); // Nothing will be printed

	std::cout << "Capacity: " << theap2->size() << "\n";
	std::cout << "Is heap empty? " << std::boolalpha << theap2->empty() << "\n\n\n";
	delete theap2;

	// Success!

	// Test 3: Create a TradeHeap on the stack and insert a few elements
	// Print the elements and check if they are in correct order. 
	// Then remove the elements until the TradeHeap is empty. Check how the capacity behaves
	std::cout << "*** Test 3:\n\n";
	TradeHeap theap3;

	// Elements
	Request * r1 = new AutoRequest("BUY", "GOOGL", 1020.8, 100);
	Request * r2 = new AutoRequest("SELL", "TSLA", 102.89, 50);
	Request * r3 = new AutoRequest("BUY", "DIS", 142.0, 120);

	Trader * t1 = new Trader(100000);
	Trader * t2 = new Trader(500000);

	TradeNode trade1, trade2(t1, r2), trade3(t2, r3);

	trade1.request = r1;
	trade1.trader = t1;

	// Add elements on the TradeHeap. Expected order: (t1, r1), (t2, r3), (t1, r2)
	theap3.push(trade1);
	theap3.push(trade2);
	theap3.push(trade3);

	// Print contents
	theap3.print();
	// Elements in correct order!

	// Remove elements
	std::cout << "Current heap capacity: " << theap3.size() << "\n";
	while (!theap3.empty()) {
		theap3.pop();
		std::cout << "\nElement Removed!";
	}
	std::cout << "\n";
	std::cout << "Current heap capacity: " << theap3.size() << "\n";
	std::cout << "Is heap empty? " << theap3.empty() << "\n\n\n";

	// Success! No heap resize for a small number of elements


	// Test 4: Stress test the heap by adding many elements. Check how the capacity behaves
	// Get the first element and print its information. Then remove all elements
	std::cout << "*** Test 4:\n\n";
	std::size_t no_elements = 150;
	unsigned i = 0;
	for (; i < no_elements; ++i) {
		// Random rule for trade diversity
		if (i % 2 == 0)
			theap3.push(trade1);
		else
			theap3.push(trade2);

		std::cout << "Element added! Capacity: " << theap3.size() << "\n";
	}
		
	// Get top element
	TradeNode top = theap3.pop();

	std::cout << "\n*** Top element:\n";
	if (top.request != nullptr) {
		auto request_tuple = top.request->getData();

		std::cout	<< std::get<0>(request_tuple)	<< ", "
				<< std::get<1>(request_tuple)	<< ", "
				<< std::get<2>(request_tuple)	<< ", "
				<< std::get<3>(request_tuple)	<< ", "
				<< top.request->getTimestamp()	<< "\n";
	}
		
	if (top.trader != nullptr) {
		top.trader->info();
		std::cout << "\n";
	}

	std::cout << "\nSubmission Id: " << top.submit_id << "\n\n";

	// Remove all elements
	while (!theap3.empty())
		theap3.pop();

	// Success!

	// Reclaim memory
	
	// Delete test traders
	delete t1; delete t2;

	// Delete test requests
	delete r1; delete r2; delete r3;	

	return 0;
}
