testing: main.cpp i2cbus.hpp vcnl4040.hpp aht20.hpp
	g++ -ggdb -rdynamic -funwind-tables -o testing main.cpp -lbcm2835
