CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2 -I/usr/include/postgresql -I/usr/include/gdal -I../Common
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lpq -lpthread -lgdal

TARGET = ../dags/bin/IntersectCalculation_bin

SRC = ./main.cpp ../Common/DatabaseHandler.cpp ../Common/LandProperty.cpp ../Common/ShapefileHandler.cpp ../Common/InvalidPolygonTableHandler.cpp

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)