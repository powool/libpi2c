testing: i2cbus.o main.o aht20.o vcnl4040.o checkTerminate.o
	g++ -ggdb -rdynamic -funwind-tables -o testing $^ -lbcm2835 -lpthread

clean:
	rm *.o testing

# https://gcc.gnu.org/projects/cxx-status.html
.cpp.o:
	g++ -std=c++2a -ggdb -rdynamic -funwind-tables -c $<

#	main.cpp i2cbus.hpp vcnl4040.hpp aht20.hpp

main.o: main.cpp

aht20.o: aht20.cpp aht20.hpp

i2cbus.o: i2cbus.cpp i2cbus.hpp

vcnl4040.o: vcnl4040.cpp vcnl4040.hpp

checkTerminate.o: checkTerminate.cpp checkTerminate.hpp
