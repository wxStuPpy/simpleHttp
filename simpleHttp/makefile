# 定义目标可执行文件的名称
TARGET = simpleHttpServer

# 定义源文件列表
SRC = main.cpp server.cpp

# 定义头文件列表（如果有更多头文件，可继续添加）
HEADERS = server.h

# 定义编译器
CC = g++

# 定义编译选项
CFLAGS = -Wall -std=c++11

# 目标: 依赖
# 规则
$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $^ -o $@

# 清理目标，删除生成的可执行文件
clean:
	-rm -f $(TARGET)