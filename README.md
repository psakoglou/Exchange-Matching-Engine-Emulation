# Exchange Matching Engine Emulation

This project implements an elementary version of a matching engine in a stock market exchange. It uses data structures and algorithms to describe an efficient system that handles asynchronous BUY and SELL requests from buyers and sellers for a pool of financial instruments (equity in this case). The system fills the requests or keeps them pending as per the availability, and keeps track of all transactions and their timestamp in the order book.

# System Components

## Trade Request interface

The Request hierarchy models a basic SELL/BUY request from a trader. It encapsulates basic parameters such as the financial instrument (equity) name, the quantity and price of the trade, the trade side (BUY side or SELL side), and the timestamp of the initiated transaction trade. 

The base class implements a basic access interface to these parameters (getters) and a convenient getData() method that returns a tuple with all these parameters. It also provides a pure virtual (abstract) method to be implemented in the derived classes. One of the reasons for making the Request base class abstract is that we don't want the trader (user) to instantiate it, since there are currently two different options for initializing a trade request: an automated option and a manual one. 

The automated request object basically uses one parameter constructor to get the trade input upon construction -- which  input could be coming from another software component that determines each trade. This would work for algorithmic trading where these processes are all automated and different components instantiate trade requests automatically. 

The manual request object is for traders who manually want to specify the trade parameters. Thus upon construction of a manual trade, the trader (user) can interactively select the trade parameters via the init() method of the ManualRequest class. Notice that since there is internal heap allocation and memory handling, it won't be wise to write init()'s code inside of the constructor cause we want to be able to handle bad input and other run time errors without risking a crash or memory leak. That being said, init() only serves as a safe interactive interface for the user, thus we keep it private. 

Finally, notice that the copy constructor and assignment operator of the entire Request hierarchy are set to be private. The prevents any Request instance to be accidentally or intentionally copied in the exchange. All submitted requests must be unique.

![Data-Flow](/img/RequestUML.jpg)

## Trader interface

The Trader class is much simpler than the Request class. We implement some basic attributes of a trader (id, trading eligibility, etc.) and we are solely focusing on the cash position of the trader. The Trader class additionally provides a buy() and a sell() method to be used for the transactions -- which directly affect the current cash position of the trader. Moreover, auxiliary methods are implemented to give a more realistic sense of a general Trader object, but also for extendability demonstration reasons. 

For simplicity reasons we are making three major assumptions for the Trader object. These assumptions can be eliminated once further system details are provided.

1) We do not check whether or not a trader has a certain stock he/she wants to sell. The restriction is that you cannot sell more than your current cash value V.

2) If a Trader can no longer trade at some point i.e. V < lower\_bound, then all remaining BUY/SELL requests are removed from the exchange

3) Every trader starts with a random amount between $500,000 - $1,000,000 

Again, keep in mind that the objective of this system is to emulate a matching machine in an exchange, thus we don't want to over-engineer supporting system components and overfit the underlying problem. 

Last but not least, notice that the copy constructor and assignment operator are set to be private. This ensures that no copies of a Trader object will be created anywhere in the program on runtime. This enhances security in the system cause we don't want replications of any trader account at all times. Every trader must be unique. 

![Data-Flow](/img/TraderUML.jpg)
