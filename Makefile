CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
LIBS = -lssl -lcrypto

SRCS = main.cpp Server.cpp ClientHandler.cpp Authenticator.cpp \
       VectorCalculator.cpp Logger.cpp ClientBase.cpp CommandLineParser.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = server

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean