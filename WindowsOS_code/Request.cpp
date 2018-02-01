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

// No heap allocation here, so set it default (http://en.cppreference.com/w/cpp/keyword/default)
Request::~Request() = default;

//*** Getter methods to be inherited in derived classes ***//

// Trade timestamp getter (as a string)
// Return "NULL" if an exception/error/cancellation occurs 
const std::string Request::getTimestamp() {
	if (Request::rdata != nullptr) {
		std::stringstream ss;
		ss << std::put_time(&Request::rdata->m_timestamp, "%F %T EST");
		return ss.str();
	}
	return "NULL";
}

// Trading instrument getter as a string (stock name in our example)
// Return "NULL" if an exception/error/cancellation occurs
const std::string Request::getInstrument() {
	if (Request::rdata != nullptr)
		return Request::rdata->m_instrument;
	return "NULL";
}

// Trade side (BUY/SELL) getter as a string
// Return "NULL" if an exception/error/cancellation occurs
const std::string Request::getSide() {
	if (Request::rdata != nullptr)
		return Request::rdata->m_side;
	return "NULL";
}

// Trade quantity getter
// Return 0 if an exception/error/cancellation occurs
const long Request::getQuantity() {
	if (Request::rdata != nullptr)
		return Request::rdata->m_quantity;
	return 0;
}

// Quantity setter, in case a trade is not completely filled
void Request::setQuantity(long new_quant) {
	if (Request::rdata != nullptr)
		Request::rdata->m_quantity = new_quant;
}


// Trade price request getter
// Return 0.0 if an exception/error/cancellation occurs
const double Request::getPrice() {
	if (Request::rdata != nullptr)
		return Request::rdata->m_price;
	return 0.0;
}

// Get all data in a tuple in the following order: side, instrument, price, quantity, timestamp
// If data uninitialized, return tuple of: NULL, NULL, 0.0, 0, NULL
const Request::DataTuple Request::getData() {
	if (rdata != nullptr)
		return std::make_tuple(getSide(), getInstrument(), getPrice(), getQuantity(), getTimestamp());
	return std::make_tuple("NULL", "NULL", 0.0, 0, "NULL");
}

// Notice!!! 
// The print function will be overriden below, while the above getters will be inherited 

//========================================================================================

//*** AutoRequest class implementation ***//

// Parameter constructor implementation (No need for default constructor, since "default" trades are not defined)
// Takes Request data as input and instantiates a new request dynamically.
AutoRequest::AutoRequest(std::string side, std::string instrument, double price, long quantity) {
	Request::rdata = new RequestData();
	
	// Key data
	Request::rdata->m_instrument	= instrument;
	Request::rdata->m_quantity	= quantity;
	Request::rdata->m_price		= price;
	Request::rdata->m_side		= side;
	
	// Timestamp
	std::time_t t			= std::time(nullptr);
	Request::rdata->m_timestamp 	= *std::localtime(&t);	
}


// Print method for a trade request
// Prints appropriate error message in case rdata is uninitialized
void AutoRequest::printRequestInfo() {
	if (rdata != nullptr)
		std::cout << "* NEW AUTO REQUEST: "
		<< rdata->m_side << ", "
		<< rdata->m_instrument << ", $"
		<< rdata->m_price << ", "
		<< rdata->m_quantity << ", "
		<< getTimestamp();
	else std::cout << "Nothing to print!";
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
// errors/exceptions/etc. occur, then the constructor will proceed to get the timestamp
ManualRequest::ManualRequest() {
	Request::rdata = new RequestData();

	// Key data init
	init();	

	// If no errors when init
	if (Request::rdata != nullptr) {
		// Timestamp
		std::time_t t			= std::time(nullptr);
		Request::rdata->m_timestamp 	= *std::localtime(&t);
	}	
}

// Print method for a trade request
// Prints appropriate error message in case rdata is uninitialized
void ManualRequest::printRequestInfo() {
	if (rdata != nullptr)
		std::cout << "* NEW MANUAL REQUEST: "
		<< rdata->m_side << ", "
		<< rdata->m_instrument << ", $"
		<< rdata->m_price << ", "
		<< rdata->m_quantity << ", "
		<< getTimestamp() << "\n";
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

// Destructor of ManualRequest derived class
// Enforced garbage collection bellow, that de-allocates the DataRequest 
// from the heap. Conseqeuntly, there will be no memory issues.
ManualRequest::~ManualRequest() {
	if (Request::rdata != nullptr)
		delete Request::rdata;
}
