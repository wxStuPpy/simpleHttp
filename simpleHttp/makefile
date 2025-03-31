# 定义目标可执行文件的名称
TARGET = simpleHttpServer

# 定义源文件列表
SRC = main.cpp server.cpp

# 定义头文件列表（如果有更多头文件，可继续添加）
HEADERS = server.h

# 定义编译器
CC = g++

# 定义编译选项
CFLAGS = -Wall -std=c++11 -lpthread

# 生成可执行文件的目标
$(TARGET): $(SRC) $(HEADERS)
	-$(CC) $(CFLAGS) $^ -o $@

# 运行可执行文件的目标
run: $(TARGET)
	-./$(TARGET) 10000 /home/ywx/tool

# 清理生成的可执行文件的目标
clean:
	-rm -f $(TARGET)