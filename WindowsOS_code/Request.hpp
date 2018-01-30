/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Request hierarchy definition
*
*/

// Multiple inclusion guards to avoid linker errors
// In case this file is included in multiple source files
// we want to avoid re-compilations
#ifndef REQUEST_HPP
#define REQUEST_HPP

// Necessary Standard Lib dependencies
// Make sure they work under the production OS
#include <iostream>
#include <chrono>		// System clock etc.: http://en.cppreference.com/w/cpp/chrono
#include <ctime>
#include <iomanip>		// std::put_time, to convert timestamp to string: http://en.cppreference.com/w/cpp/header/iomanip
#include <string>
#include <sstream>

//*** RequestData struct ***//

// Encapsulate all the necessary data for a request + timestamp
// This struct will be encapsulated below as well. Only used for convenience
// and intuitive structuring of the parameters
struct RequestData {
	std::tm		m_timestamp;	// Timestamp of Request creation
	std::string m_instrument;	// Underlying Instrument
	std::string m_side;			// Trade side (BUY/SELL)
	long		m_quantity;		// Trade quantity
	double		m_price;		// Trade price
};

//=========================================================================================

//*** Request abstract interface ***//

// Provides an interface to pull trade Request data. Encapsuates a pointer to 
// the RequestData to make the program lighter and implements most its methods to the 
// derived types of Requests below. Constructor and destructor must be public for the 
// inheritance to work, however we DONT WANT COPIES of a request of any type 
// (copy 'ctor + operator= are private) nor anyone to access the pointer member and 
// modify the data, except the derived '*Request' classes ('rdata' is protected)
class Request {
public:
	Request();
	virtual ~Request() = default;	// No heap allocation here, so set it default (http://en.cppreference.com/w/cpp/keyword/default)
	// This destructor is virtual cause although the threads will only hold a pointer
	// to this interface, when they delete the pointer the derived interface destructor will
	// be called if this is not virtual as well, and we will get instant memory leak
public:
	virtual const long long		getId()				const = 0;
	virtual const std::string	getTimestamp()		const = 0;
	virtual const std::string	getInstrument()		const = 0;
	virtual const std::string	getSide()			const = 0;
	virtual const long			getQuantity()		const = 0;
	virtual const double		getPrice()			const = 0;
	virtual void				setId()					  = 0;
	virtual void				printRequestInfo()	const = 0;
protected:
	RequestData * rdata;
	long long id;
private:
	// No copies of trade requests for security reasons or runtime bugs
	Request(const Request &);
	Request& operator=(const Request &);
};

//=========================================================================================

//*** AutoRequest interface ***//

// Provides an interface for automated request handling. In case the trader has bots that submit requests,
// this interface can be used and modified to handle algo trading.
// In this demo, instances of this class will be used by a pool of buyer/seller (consumer/producer) 
// thread pools. The parameters will be determined upon construction either "manually" or by an input
// handler that will randomly select and pass values in the parameter constructor. 
// Notice!!!
	// This implementation is for demo only 
class AutoRequest : public Request {
public:
	// The parameter constructor is responsible for the memory allocation of RequestData
	AutoRequest(std::string side, std::string instrument, double price, long quantity);

	~AutoRequest(); // Memory management internally! This destructor is responsible for the
	// memory de-allocation of RequestData. 

	// Access interface
	void				printRequestInfo()	const;
	const std::string	getInstrument()		const;
	const std::string	getTimestamp()		const;
	const long			getQuantity()		const;
	const double		getPrice()			const;
	const std::string	getSide()			const;
	const long long		getId()				const;
		
	// Set ID upon Request submission
	void setId();

private:
	// No copies of trade requests for security reasons or runtime bugs
	AutoRequest& operator=(const AutoRequest &);
	AutoRequest(const AutoRequest &);
};

//=========================================================================================

//*** ManualRequest interface ***//

// Provides an interactive user interface within the constructor for security reasons.
// It allows the trader to select trade values and handles any typing errors or other runtime
// bugs that migth trigger exceptions and terminate the execution of the program or request. 
// In case of execution termination or bad input, the interactive interface will clean the 
// allocated memory of the RequestData, set the pointer to nullptr, and return, thus cancelling
// the request
class ManualRequest : public Request {
public:
	// The parameter constructor is responsible for the memory allocation of RequestData
	ManualRequest();

	~ManualRequest(); // Memory management internally! This destructor is responsible for the
	// memory de-allocation of RequestData. 

	// Access interface
	void				printRequestInfo()	const;
	const std::string	getInstrument()		const;
	const std::string	getTimestamp()		const;
	const long			getQuantity()		const;
	const double		getPrice()			const;
	const std::string	getSide()			const;
	const long long		getId()				const;
	
	// Set ID upon Request submission
	void setId();
private:
	// Only the constructor will provide an interactive Request init() method to the trader 
	void init();

	// No copies of trade requests for security reasons or runtime bugs
	ManualRequest(const ManualRequest &);
	ManualRequest & operator=(const ManualRequest &);
};

#endif // !REQUEST_HPP