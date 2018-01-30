/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Request hierarchy implementation
*
*/

#include "Request.hpp"

//*** Request base class implementation ***//

// Default constructor of base class initializes the RequestData pointer member
// to nullptr to be used as a designated pointer value in case something
// goes wrong i.e. if an exception happens and then the user/program tries to 
// use 'rdata' uninitialized. This helps as a sanity check and can prevent run-time errors
Request::Request() : rdata(nullptr) {}

// Notice!!! 
// The rest of the base class functionality will be overriden in derived classes below 

//========================================================================================

//*** AutoRequest class implementation ***//

// Parameter constructor implementation (No need for default constructor, since "default" trades are not defined)
// Takes Request data as input and instantiates a new request.
AutoRequest::AutoRequest(std::string side, std::string instrument, double price, long quantity) {
	Request::rdata = new RequestData();
	
	// Key data
	Request::rdata->m_instrument	= instrument;
	Request::rdata->m_quantity	= quantity;
	Request::rdata->m_price		= price;
	Request::rdata->m_side		= side;
	
	// Timestamp
	std::time_t t			= std::time(nullptr);
	Request::rdata->m_timestamp     = *std::localtime(&t);	
}

// Setting trade ID upon submission
void AutoRequest::setId() {
	Request::id = std::chrono::system_clock::now().time_since_epoch().count();
}

// Print method for a trade request
// Prints appropriate error message in case rdata is uninitialized
void AutoRequest::printRequestInfo() const {
	if (rdata != nullptr)
		std::cout 	<< "* NEW AUTO REQUEST: "
				<< rdata->m_side 	<< ", "
				<< rdata->m_instrument 	<< ", $"
				<< rdata->m_price 	<< ", "
				<< rdata->m_quantity 	<< ", "
				<< getTimestamp() 	<< "\n";
	else std::cout << "Nothing to print!";
}

//*** 'RequestData' getters for AutoRequest class ***//

// Trade ID getter
// Return 0 if an exception/error/cancellation occurs
const long long	AutoRequest::getId() const {
	if (Request::rdata != nullptr)
		return Request::id;
	return 0;
}

// Trade timestamp getter (as a string)
// Return "NULL" if an exception/error/cancellation occurs 
const std::string AutoRequest::getTimestamp() const {
	if (Request::rdata != nullptr) {
		std::stringstream ss;
		ss << std::put_time(&Request::rdata->m_timestamp, "%F %T EST");
		return ss.str();
	}
	return "NULL";	
}

// Trading instrument getter as a string (stock id in our example)
// Return "NULL" if an exception/error/cancellation occurs 
const std::string AutoRequest::getInstrument() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_instrument;
	return "NULL";
}

// Trade side getter as a string
// Return "NULL" if an exception/error/cancellation occurs 
const std::string AutoRequest::getSide() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_side;
	return "NULL";
}

// Trade quantity getter
// Return 0 if an exception/error/cancellation occurs 
const long AutoRequest::getQuantity() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_quantity;
	return 0;
}

// Trade price request getter
// Return 0.0 if an exception/error/cancellation occurs 
const double AutoRequest::getPrice() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_price;
	return 0.0;
}

// Destructor of AutoRequest derived class
// Enforced garbage collection bellow, that de-allocates the DataRequest 
// from the heap. Conseqeuntly, there will be no memory issues.
AutoRequest::~AutoRequest() {
	if (Request::rdata != nullptr)
		delete Request::rdata;
}

//========================================================================================

//*** ManualRequest class implementation ***//

// ManualRequest Default constructor will instantiate RequestData on the heap, call an init()
// method that will allow the user SAFELY to select the wanted request values, and if no
// errors/exceptions/etc. occur, then the constructor will proceed to get the timestamp of the
// trade and its ID
ManualRequest::ManualRequest() {
	Request::rdata = new RequestData();

	// Key data init
	init();	

	// If no errors when init
	if (Request::rdata != nullptr) {
		// Timestamp
		std::time_t t		    = std::time(nullptr);
		Request::rdata->m_timestamp = *std::localtime(&t);
	}	
}

// Setting trade ID upon submission
void ManualRequest::setId() {
	Request::id = std::chrono::system_clock::now().time_since_epoch().count();
}

// Print method for a trade request
// Prints appropriate error message in case rdata is uninitialized
void ManualRequest::printRequestInfo() const {
	if (rdata != nullptr)
		std::cout 	<< "* NEW MANUAL REQUEST: "
				<< rdata->m_side 	<< ", "
				<< rdata->m_instrument 	<< ", $"
				<< rdata->m_price 	<< ", "
				<< rdata->m_quantity 	<< ", "
				<< getTimestamp() 	<< "\n";
	else std::cout << "Nothing to print!";
}

// Init() method to interactively allow the trader to select the Request parameters
// Runs in the constructor only
void ManualRequest::init() {

	// Naive and unprotected Stock[] C-style array. To be encapsulated for further security and performance
	// For demonstration only, the following array will serve our purposes
	const char * Stocks[]{ "GOOGL", "BABA", "AMZN", "TSLA", "DIS", "\0" };
	// Demo-available stocks:
		// GOOGL  -- Aphabet Inc. 
		// BABA	  -- Alibaba Group Holding Ltd. 
		// AMZN	  -- Amazon.com Inc.
		// TSLA	  -- Tesla Inc.
		// DIS	  -- Walt Disney Co

	//*** Interaction interface with error/exception handling ***//

	std::cout << "\n*** INIT REQUEST ***\n\n";

	std::cout << "Press 0 to cancel request. Press any key to continue: ";
	int decision = 0;
	std::cin >> decision;
	if (std::cin.fail()) {

		// Reset failbit
		std::cin.clear();

		// User didn't input a number
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}
	else {
		if (!decision) {
			// Handle memory and return
				if (Request::rdata != nullptr) {
					delete Request::rdata;
					Request::rdata = nullptr;
				}
			// Good bye!
			std::cout << "Request Cancelled! Try again!\n";
			return;
		}
	}

	// Elementary rule of thumb for error handling:
		//	When a false input or error occurs, cancel the transaction, handle the allocated
		//	RequestData memory and set the 'rdata' pointer to nullptr, and return.
		//	Concequently the destructor won't deallocate again, so it's safe.
	
	//*** Select trade side
	std::cout << "Do you want to BUY or to SELL?\n";
	std::cout << "1. BUY\t2. SELL\nYour choice: ";

	std::cin >> decision;
	if (std::cin.fail()) {

		// Reset failbit
		std::cin.clear();

		// User didn't input a number
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}
	
	if (decision == 1) 
		Request::rdata->m_side = "BUY";
	else if (decision == 2) 
		Request::rdata->m_side = "SELL";
	else {
		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}


	//*** Select trade instrument
	std::cout << "Which instrument you want to trade?\n";
	unsigned i = 1; 
	while (Stocks[i-1] != "\0") {
		std::cout << i << ". " << Stocks[i - 1] << "\n";
		++i;
	}
	std::cout << "\nYour choice: ";

	std::cin >> decision;
	if (std::cin.fail()) {

		// Reset failbit
		std::cin.clear();

		// User didn't input a number
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}

	switch (decision)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		Request::rdata->m_instrument = Stocks[decision - 1];
		break;
	default:
		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}

	//*** Select trade price
	std::cout << "Specify trade price: ";

	double price = 0.0;
	std::cin >> price;
	if (std::cin.fail()) {

		// Reset failbit
		std::cin.clear();

		// User didn't input a number
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}

	if (price < DBL_MAX && price >= 0.0)	
		Request::rdata->m_price = price;
	else {
		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}

	//*** Select trade quantity
	std::cout << "Specify trade quantity: ";

	long quantity = 0;
	std::cin >> quantity;
	if (std::cin.fail()) {

		// Reset failbit
		std::cin.clear();

		// User didn't input a number
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
		return;
	}

	if (quantity < INT_MAX && quantity > 0)
		Request::rdata->m_quantity = quantity;
	else {
		// Handle memory and return 
		if (Request::rdata != nullptr) {
			delete Request::rdata;
			Request::rdata = nullptr;
		}
		// Good bye!
		std::cout << "Request Cancelled! Try again!\n";
	}
	return;
}

//*** 'RequestData' getters for ManualRequest class ***//

// Trade ID getter
// Return 0 if an exception/error/cancellation occurs
const long long	ManualRequest::getId() const {
	if (Request::rdata != nullptr)
		return Request::id;
	return 0;
}

// Trade timestamp getter (as a string)
// Return "NULL" if an exception/error/cancellation occurs 
const std::string ManualRequest::getTimestamp() const {
	if (Request::rdata != nullptr) {
		std::stringstream ss;
		ss << std::put_time(&Request::rdata->m_timestamp, "%F %T EST");
		return ss.str();
	}
	return "NULL";
}

// Trading instrument getter as a string (stock id in our example)
// Return "NULL" if an exception/error/cancellation occurs 
const std::string ManualRequest::getInstrument() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_instrument;
	return "NULL";
}

// Trade side getter as a string
// Return "NULL" if an exception/error/cancellation occurs 
const std::string ManualRequest::getSide() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_side;
	return "NULL";
}

// Trade quantity getter
// Return 0 if an exception/error/cancellation occurs 
const long ManualRequest::getQuantity() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_quantity;
	return 0;
}

// Trade price request getter
// Return 0.0 if an exception/error/cancellation occurs 
const double ManualRequest::getPrice() const {
	if (Request::rdata != nullptr) 
		return Request::rdata->m_price;
	return 0.0;
}

// Destructor of ManualRequest derived class
// Enforced garbage collection bellow, that de-allocates the DataRequest 
// from the heap. Conseqeuntly, there will be no memory issues.
ManualRequest::~ManualRequest() {
	if (Request::rdata != nullptr)
		delete Request::rdata;
}
