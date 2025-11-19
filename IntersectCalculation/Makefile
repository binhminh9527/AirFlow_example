CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

TARGET = ../dags/bin/IntersectCalculation_bin

SRC = main.cpp DatabaseHandler.cpp IntersectCalculation.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET)