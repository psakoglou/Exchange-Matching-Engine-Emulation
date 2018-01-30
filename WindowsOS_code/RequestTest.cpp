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

	// Test 5: Create ManualRequest on the heap, print the info, set and print trade id
	ManualRequest * man_req2 = new ManualRequest();
	man_req2->printRequestInfo();
	std::cout << "\n";
	man_req2->setId();
	std::cout << "Submission ID: " << man_req2->getId() << "\n\n";	// Will print 0, if you cancel the request
	delete man_req2;
	// Success!

	// Test 6: Create AutoRequests on the heap, set their ID 
	Request * r1 = new AutoRequest("BUY", "GOOG", 1000.43, 100);
	Request * r2 = new AutoRequest("SELL", "BABA", 99.51, 210);
	Request * r3 = new AutoRequest("BUY", "TSLA", 28.9, 90);

	r1->setId();
	r3->setId(); 
	r2->setId();
	// Setting the ID upon submission:
		// r1 is submitted first, r3 is submitted second, r2 is submitted third


	// Test 7: Assume that upon submission requests were logged in a vector
	// Sort the vector as per their submission ID and then as per the highest price
	
	// Submit/Log the requests
	std::vector<Request *> submitted;
	submitted.push_back(r1);
	submitted.push_back(r2);
	submitted.push_back(r3);

	//*** Sort per time
	// Lambda O(n^2) sorting as per time (for demo only)
	auto myN2TimeSort = [&]()->void {
		if (submitted.empty() || submitted.size() == 1) return;
	
		unsigned i = 0;
		for (; i < submitted.size(); ++i) {

			long long min_id = submitted[i]->getId();
			int min_index = 0;

			unsigned j = i + 1;
			for (; j < submitted.size(); ++j) {
				if (min_id > submitted[j]->getId()) {
					min_id = submitted[j]->getId();
					min_index = j;
				}
			}
			if (j != submitted.size())
				std::swap(submitted[i], submitted[j]);
		}
	};
	
	myN2TimeSort();

	std::cout << "\n\n*******************************************";
	std::cout << "\n*** Request priority as per submission time:\n\n";
	unsigned i = 1;
	for (auto e : submitted) {
		std::cout << i++ << ". ";
		e->printRequestInfo();
		std::cout << "ID: " << e->getId() << "\n\n";
	}
	// Success!

	//*** Sort per price
	// Lambda O(n^2) sorting as per highest price (for demo only)
	auto myN2PriceSort = [&]()->void {
		if (submitted.empty() || submitted.size() == 1) return;

		unsigned i = 0;
		for (; i < submitted.size(); ++i) {

			double max_price = submitted[i]->getPrice();
			int max_index = 0;

			unsigned j = i + 1;
			for (; j < submitted.size(); ++j) {
				if (max_price < submitted[j]->getPrice()) {
					max_price = submitted[j]->getPrice();
					max_index = j;
				}
			}
			if (j != submitted.size())
				std::swap(submitted[i], submitted[j]);
		}
	};

	myN2PriceSort();
	
	std::cout << "\n\n*******************************************";
	std::cout << "\n*** Request priority as per highest price:\n\n";
	i = 1;
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

	return 0;
}