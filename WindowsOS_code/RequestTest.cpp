/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Testing 'Request' hierarchy interface
*
*/

#include <iostream>
#include <vector>

#include "Request.hpp"

int main() {

	std::cout << "*** Testing 'Request' hierarchy interface ***\n\n";

	// Test 1: Attempt to instantiate abstract class
	// Request is an abstract class and cannot be instantiated!
	// Request r; // Compiler error: C2259 (https://msdn.microsoft.com/en-us/library/zxt206sk.aspx)
	// Success!

	// Test 2: Create AutoRequest instance on the stack and print the info
	AutoRequest auto_req1("BUY", "GOOGL", 1000.43, 100);
	auto_req1.printRequestInfo();
	std::cout << "\n\n";
	// Success!

	// Test 3: Create ManualRequest instance on the stack and print the info
	ManualRequest man_req1;
	man_req1.printRequestInfo();
	std::cout << "\n\n";
	// Success!

	// Test 4: Create AutoRequest on the heap and print the info
	AutoRequest * auto_req2 = new AutoRequest("BUY", "GOOGL", 1000.43, 100);
	auto_req2->printRequestInfo();
	std::cout << "\n\n";
	delete auto_req2;
	// Success!

	// Test 5: Create ManualRequest on the heap, print the info, print the quantity
	ManualRequest * man_req2 = new ManualRequest();
	man_req2->printRequestInfo();
	std::cout << "\nQuantity: " << man_req2->getQuantity() << "\n";		// Will print 0, if you cancel the request
	std::cout << "Timestamp: " << man_req2->getTimestamp() << "\n\n";	// Will print NULL, if you cancel the request
	delete man_req2;
	// Success!

	// Test 6: Create AutoRequests on the heap, print their price 
	Request * r1 = new AutoRequest("BUY", "TSLA", 28.9, 90);
	Request * r2 = new AutoRequest("SELL", "BABA", 99.51, 210);
	Request * r3 = new AutoRequest("BUY", "GOOG", 1000.43, 100);

	std::cout << "Stock prices:\n";
	std::cout << r1->getPrice() << "\n";
	std::cout << r2->getPrice() << "\n";
	std::cout << r3->getPrice() << "\n\n";

	// Test 7: Assume that upon submission requests were logged in a vector
	// Sort the vector as per the highest price
	
	// Submit/Log the requests
	std::vector<Request *> submitted;
	submitted.push_back(r1);
	submitted.push_back(r2);
	submitted.push_back(r3);

	//*** Sort per price
	// Lambda O(n^2) sorting as per highest price (for demo only)
	auto myN2PriceSort = [&]()->void {

		std::size_t size = submitted.size();

		if (size < 2) return;

		unsigned i = 0;
		
		for (; i < size; ++i) {

			double max_price = submitted[i]->getPrice();
			int max_index = i;

			unsigned j = i + 1;
			for (; j < size; ++j) {
				if (max_price < submitted[j]->getPrice()) {
					max_price = submitted[j]->getPrice();
					max_index = j;
				}
			}
			std::swap(submitted[i], submitted[max_index]);
		}
	};

	// Sort
	myN2PriceSort();
	
	// Print results
	std::cout << "\n\n*******************************************";
	std::cout << "\n*** Request priority as per highest price:\n\n";
	unsigned i = 1;
	for (auto e : submitted) {
		std::cout << i++ << ". ";
		e->printRequestInfo();
		std::cout << "Price: " << e->getPrice() << "\n\n";
	}
	// Success!

	// Clear submission vector
	submitted.clear();

	// Reclaim heap memory
	delete r1;
	delete r2;
	delete r3;

	// Test 8: Create an auto request on the heap and print its info using the tuple
	AutoRequest * auto_req3  = new AutoRequest("SELL", "GOOGL", 1000.43, 100);
	
	std::cout 	<< "\n\n*** AutoRequest with tuple:\n";
	std::cout	<< std::get<0>(auto_req3->getData()) << " "
			<< std::get<1>(auto_req3->getData()) << " "
			<< std::get<2>(auto_req3->getData()) << " "
			<< std::get<3>(auto_req3->getData()) << " "
			<< std::get<4>(auto_req3->getData()) << "\n\n\n";

	delete auto_req3;


	// Test 9: Testing polymorphic behavior 
	Request * r;
	
	r = new AutoRequest("SELL", "BABA", 55.43, 100);
	std::cout << "*** Testing polymorphic behavior:\n\n";
	std::cout << "Test i -- print():\n";
	r->printRequestInfo();

	std::cout	<< "\nTest ii -- raw:\n";
	std::cout	<< std::get<0>(r->getData())		<< " "
				<< std::get<1>(r->getData())	<< " "
				<< r->getPrice()		<< " "
				<< r->getQuantity()		<< " "
				<< r->getTimestamp()		<< "\n\n\n";
	delete r;

	r = new AutoRequest("SELL", "DIS", 155.43, 150);
	std::cout << "Test i -- print():\n";
	r->printRequestInfo();

	std::cout	<< "\nTest ii -- raw:\n";
	std::cout	<< std::get<0>(r->getData())	<< " "
			<< std::get<1>(r->getData())	<< " "
			<< r->getPrice()		<< " "
			<< r->getQuantity()		<< " "
			<< r->getTimestamp()		<< "\n\n\n";
	delete r;

	r = new ManualRequest();
	std::cout << "Test i -- print():\n";
	r->printRequestInfo();

	std::cout	<< "\nTest ii -- raw:\n";
	std::cout	<< std::get<0>(r->getData())	<< " "
			<< std::get<1>(r->getData())	<< " "
			<< r->getPrice()		<< " "
			<< r->getQuantity()		<< " "
			<< r->getTimestamp()		<< "\n\n";
	delete r;

	// Success!

	return 0;
}
