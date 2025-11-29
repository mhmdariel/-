CXX = g++
CXXFLAGS = -std=c++17 -pthread -O3 -march=native
TARGET_DEMOLITION = demolition_sim
TARGET_ALPHAZONE = alphazone

all: $(TARGET_DEMOLITION) $(TARGET_ALPHAZONE)

$(TARGET_DEMOLITION): demolition_sim.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET_ALPHAZONE): alphazone.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET_DEMOLITION) $(TARGET_ALPHAZONE)

.PHONY: all clean
