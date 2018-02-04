/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	TradeHeap implementation
*
*/

#include "TradeHeap.hpp"

//*** Constructors and static members ***//

// Default constructor allocates on the heap and creates the 
// heap array of a default size. Initializes index to zero
TradeHeap::TradeHeap() : m_size(default_size), m_index(0) {
	m_trades = new TradeNode[m_size];
}

// Static member for default size trivially set to 10
// We need to be careful how much default memory allocate to 
// prevent waste of resources (i.e. RAM)
std::size_t TradeHeap::default_size = 10;

// Copy constructor allocates a new heap and copies the elements
// of an existing one
TradeHeap::TradeHeap(const TradeHeap & th) {
	m_size = th.m_size;
	m_index = th.m_index;
	m_trades = new TradeNode[m_size];
	unsigned i = 0;
	for (; i <= m_index; ++i)
		m_trades[i] = th.m_trades[i];
}

// Default constructor reclaims the allocated memory
TradeHeap::~TradeHeap() {
	delete[] m_trades;
}

//*** Auxiliary methods ***//

// Operator[] allows us to access an element of the heap in O(1)
// Used only for convenience and can be ignored
const TradeNode TradeHeap::operator[](unsigned index) {
	if (index == 0)
		return m_trades[0];

	// In case of illegal input return the first element
	if (index >= m_index || index >= m_size || index < 0) {
		// Possibly throw here
		return m_trades[0];
	}
	else return m_trades[index];
}

// Print method that iterates the heap and prints the information of 
// each element
void TradeHeap::print() {
	unsigned i = 0;
	for (; i < m_index; ++i) {
		std::cout << "Request: "; m_trades[i].request->printRequestInfo();
		std::cout << "\nTrader: "; m_trades[i].trader->info();
		std::cout << "\nSubmit Id: " << m_trades[i].submit_id << "\n\n";
	}
}

// Getter method for the number of elements in the heap
const std::size_t TradeHeap::size() {
	return m_index;
}

// Boolean method to check whether or not the heap is empty
bool TradeHeap::empty() {
	return !m_index;
}

//*** Resize methods ***//

// Expand method re-allocates memory dynamically as per 
// the underlying rule
void TradeHeap::expand() {
	TradeHeap temp(*this);
	m_size += m_size / 3;
	delete m_trades;
	m_trades = new TradeNode[m_size];
	unsigned i = 0;
	for (; i < m_index; ++i)
		m_trades[i] = temp.m_trades[i];
}

// Shrink method re-allocates memory dynamically as per 
// the underlying rule
void TradeHeap::shrink() {
	if (m_size <= default_size) return;
	TradeHeap temp(*this);
	m_size = (m_size <= default_size) ? default_size : (2 * m_size / 3);
	delete m_trades;
	m_trades = new TradeNode[m_size];
	unsigned i = 0;
	for (; i < m_index; ++i)
		m_trades[i] = temp.m_trades[i];
}

// Re-sort method for modified contents
void TradeHeap::sort() {
	if (m_index < 2)
		return;
	
	unsigned i = 0;
	for (; i < m_index; ++i) {

		double max_price = m_trades[i].request->getPrice();
		unsigned max_index = i;

		unsigned j = i + 1;
		for (; j < m_index; ++j) {

			if (m_trades[i].request->getPrice() == m_trades[j].request->getPrice())
				continue;

			if (m_trades[i].request->getPrice() < m_trades[j].request->getPrice()) {
				max_price = m_trades[j].request->getPrice();
				max_index = j;
			}
		}
		std::swap(m_trades[i], m_trades[max_index]);
	}
}

// Method to remove a particular trader's request from the heap
void TradeHeap::remove(Trader * t, Request * r) {
	if (m_index == 0)
		return;

	unsigned i = 0;
	for (; i < m_index; ++i) {
		if (m_trades[i].trader->getId() == t->getId() && m_trades[i].request->getId() == r->getId()) {
			break;
		}
	}
	std::swap(m_trades[i], m_trades[m_index - 1]);
	--m_index;
	sort();
}