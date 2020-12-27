testing: main.o aht20.o vcnl4040.o i2cbus.o
	g++ -ggdb -rdynamic -funwind-tables -o testing $^ -lbcm2835

clean:
	rm *.o testing

.cpp.o:
	g++ -ggdb -rdynamic -funwind-tables -c $<

#	main.cpp i2cbus.hpp vcnl4040.hpp aht20.hpp

main.o: main.cpp

aht20.o: aht20.cpp aht20.hpp

i2cbus.o: i2cbus.cpp i2cbus.hpp

vcnl4040.o: vcnl4040.cpp vcnl4040.hpp
