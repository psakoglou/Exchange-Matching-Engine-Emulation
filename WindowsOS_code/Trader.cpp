/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Trader class implementation
*
*/

#include "Trader.hpp"

#include <chrono>	// For random generation
#include <random>	// For id generation

//*** Trader interface implementation ***//

// Parameter constructor to initialize the portfolio cash value
// and log it in the books, and assigns a random id to the new trader
Trader::Trader(double init_cash) : V(init_cash) {
	portfolio_value.push_back(init_cash);
	
	// Generate a random 8-bit string for trader id
	// The probability that two traders share the same id is 2^-8
	std::uniform_int<int> dist(0, 1);
	std::mt19937 eng;
	unsigned i = 0; 
	for (; i < 8; ++i) {
		// Generate pure randomness as per the system clock in every iteration
		eng.seed(std::chrono::system_clock::now().time_since_epoch().count());
		t_id += std::to_string(dist(eng));
	}
}

// Static private member initialization
// Trivially set to $1000 
double Trader::lower_bound = 1000.0;

// Methods that checks whether or not a transaction 
// is valid and the trader eligible to trade
bool Trader::canTrade() {
	return V >= lower_bound;
}

// Buy method that executes a trade
void Trader::buy(double price, long quantity) {
	// Check financial eligibility of trader
	if (!canTrade()) {
		std::cerr << "Trader with id: " << t_id << " cannot trade!\n";
		return;
	}

	double trade_price = price * (double)quantity;

	// Check financial eligibility of request (transaction)
	if (trade_price > V) {
		std::cerr << "Trader with id: " << t_id << " cannot perform this transaction!\n";
		return;
	}

	// If all is legal, trade and log the trade
	V -= trade_price;
	portfolio_value.push_back(V);
}

// Sell method that executes a trade
void Trader::sell(double price, long quantity) {
	// Check financial eligibility of trader
	if (!canTrade()) {
		std::cerr << "Trader with id: " << t_id << " cannot trade!\n";
		return;
	}

	double trade_price = price * (double)quantity;

	// Check financial eligibility of request (transaction)
	if (trade_price > V) {
		std::cerr << "Trader with id: " << t_id << " cannot perform this transaction!\n";
		return;
	}

	// If all is legal, trade and log the trade
	V += trade_price;
	portfolio_value.push_back(V);
}

// Getter method that returns the current portfolio value
const double Trader::currentValue() {
	return V;
}

// Getter method that returns the trader's id
const std::string Trader::getId() {
	return t_id;
}

// Additional feature that computes and returns the margins
// of each transaction
const std::vector<double> Trader::getMargins() {

	std::vector<double> margins;

	// Iterate and compute portfolio value flunctuations	
	auto begin	= portfolio_value.begin();
	auto end	= portfolio_value.end();
	auto iter	= begin + 1;

	while (iter != end) {
		margins.push_back(*iter - *begin);
		++iter; ++begin;
	}
	return margins;
}

// Method to display the trader's info
void Trader::info() {
	std::cout 
		<< "Trader ID: "
		<< t_id
		<< "\nCash position: " 
		<< V
		<< "\nTrading Eligibility: " 
		<< std::boolalpha << canTrade();
}

// Default destructor is ok -- no heap allocation
Trader::~Trader() = default;