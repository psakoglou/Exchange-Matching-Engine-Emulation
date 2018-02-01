/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Trader class definition
*
*/

// Multiple inclusion guards to avoid linker errors
// In case this file is included in multiple source files
// we want to avoid re-compilations
#ifndef TRADER_HPP
#define TRADER_HPP

// Necessary libraries
#include <iostream>
#include <vector>
#include <string>

//*** Trader class definition ***//

// Provides an interface that describes active and inactive traders that
// trade in the exchange. Every trader starts with an initial cash value V, preferably
// over $1000. If a trader starts with less than $1000 he/she won't meet the minimum
// requirement trade in the exchange and all their request will be removed once submitted. 
// The $1000 minimum was trivially selected for the system demo. 
//
// A trader, once instantiated with a proper available trading amount V, can start
// selling or buying stocks, with each transaction being logged in the portfolio_value vector.
// Additionally, there is a feature which returns the margins of all transactions.
//
//*** Assumptions:
//			1) We do not check whether or not a trader has a certain stock he/she wants to sell.
//			   The restriction is that you cannot sell more than your current cash value V.
//			2) If a Trader can no longer trade at some point i.e. V < lower_bound, then all remaining 
//			   BUY/SELL requests are removed from the exchange
//			3) For the sake of the demonstration, every trader starts with a random amount between
//			   $500,000 - $1,000,000 
//			   *to be handled by another thread, and not the constructor!
class Trader {
public:
	// Parameter constructor since a default initial cash position is not defined
	Trader(double init_cash);

	// Default destructor since there is no heap allocation
	~Trader();

	// Methods to execute trades
	bool buy(double price, long quantity);
	bool sell(double price, long quantity);

	// Reimburse method in case a trade fails
	void reimburse(double value);

	// Auxiliary features
	const std::vector<double> getMargins();
	const double currentValue();
	const std::string getId();
	bool canTrade();
	void info();

private:
	// A unique lower trading cash amount is defined for all Trader instances
	static double lower_bound;

	// Log the portfolio cash values after each transaction
	std::vector<double> portfolio_value;

	// Cash value
	double V;

	// Trader id
	std::string t_id;

private:
	// No copies of Trader objects are allowed: you cannot replicate an existing trader account
	Trader(const Trader&);
	Trader& operator=(const Trader&);
};

#endif // !TRADER_HPP
