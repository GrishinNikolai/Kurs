# Makefile для одной папки
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
LIBS = -lssl -lcrypto

# Основные исходные файлы сервера
SRCS = Logger.cpp

# Тестовые файлы
TEST_SRCS = test_main.cpp \
           Test_CommandLineParser.cpp \
           Test_Authenticator.cpp \
           Test_ClientBase.cpp \
           Test_VectorCalculator.cpp \
           Test_Logger.cpp \
           Test_ServerIntegration.cpp

OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TARGET = server
TEST_TARGET = server_tests

# Сборка основного сервера
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Сборка тестов
$(TEST_TARGET): $(TEST_OBJS) $(filter-out main.o, $(OBJS))
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(filter-out main.o, $(OBJS)) $(LIBS) -lUnitTest++

# Компиляция .cpp файлов в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Команды для тестирования
test: $(TEST_TARGET)
	./$(TEST_TARGET)

test-verbose: $(TEST_TARGET)
	./$(TEST_TARGET) -v

# Очистка
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)
	rm -f test_log.txt test_clients.txt valid_test.log

.PHONY: test test-verbose clean
