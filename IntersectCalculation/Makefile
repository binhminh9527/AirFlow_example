CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2 -I/usr/include/postgresql
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lpq -lpthread

TARGET = ../dags/bin/IntersectCalculation_bin

SRC = main.cpp DatabaseHandler.cpp IntersectCalculation.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)