# Makefile for raw2png using C++ compiler

CXX = g++
CXXFLAGS = -Wall -O2

TARGET = raw2png
SOURCES = raw2png.cpp

all: $(TARGET)

$(TARGET): $(SOURCES) stb_image_write.h
	$(CXX) $(CXXFLAGS) -x c++ -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
