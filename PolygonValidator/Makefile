CXX = g++
CXXFLAGS = -std=c++23 -Wall -g -O0 -I/usr/include/gdal -I../Common
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lgdal

TARGET = ../dags/bin/PolygonValidator_bin

SRC = main.cpp PolygonValidator.cpp ../Common/ShapefileHandler.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	$(TARGET) ../Shapefile_validity_test/invalid_polygons.shp