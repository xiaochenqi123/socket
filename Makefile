# 编译器配置
CC      := gcc
CFLAGS  := -std=gnu11 -Wall -Wextra -O2 -g
LDFLAGS := -lm

# 目标文件
LIB_NAME    := libnet.a
TEST_TARGET := test_net

# 源文件
LIB_SRC := net_lib.c
TEST_SRC := test_net_lib.c

# 生成对象文件
LIB_OBJ := $(LIB_SRC:.c=.o)
TEST_OBJ := $(TEST_SRC:.c=.o)

# 默认目标
.PHONY: all
all: $(LIB_NAME) $(TEST_TARGET)

# 静态库构建
$(LIB_NAME): $(LIB_OBJ)
	ar rcs $@ $^
	ranlib $@

# 测试程序构建
$(TEST_TARGET): $(TEST_OBJ) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 通用编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
.PHONY: clean
clean:
	rm -f $(LIB_NAME) $(TEST_TARGET) *.o

# 安装库和头文件（可选）
.PHONY: install
install:
	sudo cp $(LIB_NAME) /usr/local/lib/
	sudo cp net_lib.h /usr/local/include/

# 卸载（可选）
.PHONY: uninstall
uninstall:
	sudo rm -f /usr/local/lib/$(LIB_NAME) \
              /usr/local/include/net_lib.h

# 头文件依赖
net_lib.o test_net_lib.o: net_lib.h