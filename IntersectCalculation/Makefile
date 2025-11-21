CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2 -I/usr/include/postgresql -I/usr/include/gdal
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lpq -lpthread -lgdal

TARGET = ../dags/bin/IntersectCalculation_bin

SRC = main.cpp DatabaseHandler.cpp LandProperty.cpp ShapefileHandler.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)