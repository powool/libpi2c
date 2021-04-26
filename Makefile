all: testing mustang
# mpl3115a2.o conflicts with vcnl4040.o
testing: i2cbus.o main.o aht20.o checkTerminate.o ssd1306.o render.o lsm6ds33.o pca9685.o tca9548a.o tmp117.o vcnl4040.o ds3502.o
	g++ -ggdb -rdynamic -funwind-tables -o $@ $^ -lbcm2835 -lpthread

mustang: mustang.o i2cbus.o checkTerminate.o ds3502.o aht20.o
	g++ -ggdb -rdynamic -funwind-tables -o $@ $^ -lbcm2835 -lpthread

clean:
	rm -f *.o testing

# https://gcc.gnu.org/projects/cxx-status.html
.cpp.o:
	g++ -std=c++2a -ggdb -rdynamic -funwind-tables -c $<

main.o: main.cpp

aht20.o: aht20.cpp aht20.hpp

i2cbus.o: i2cbus.cpp i2cbus.hpp

vcnl4040.o: vcnl4040.cpp vcnl4040.hpp

checkTerminate.o: checkTerminate.cpp checkTerminate.hpp

ssd1306.o: ssd1306.cpp ssd1306.hpp

render.o: render.cpp render.hpp

lsm6ds33.o: lsm6ds33.cpp lsm6ds33.hpp

mpl3115a2.o: mpl3115a2.cpp mpl3115a2.hpp

pca9685.o: pca9685.cpp pca9685.hpp

tca9548a.o: tca9548a.cpp tca9548a.hpp

tmp117.o: tmp117.cpp tmp117.hpp

ds3502.o: ds3502.cpp ds3502.hpp
