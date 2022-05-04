# https://www.partow.net/programming/makefile/index.html
CXX      := clang++ 
CXXFLAGS := -std=c++20 -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -stdlib=libc++
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := program
# https://qiita.com/DQNEO/items/4e5c2eca2761ec08b922
INCLUDE  := -Iinclude
# 変数での*の利用はwildcardが必要
SRC      :=                      \
   $(wildcard src/module1/*.cpp) \
   $(wildcard src/module2/*.cpp) \
   $(wildcard src/*.cpp)         \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

# If you want make to simply ignore a makefile which does not exist or cannot be remade, with no error message, use the -include directive instead of include, like this:
-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

vervose: CXXFLAGS += -v
vervose: all
 
clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"


# TEST
# https://gist.github.com/mihaitodor/bfb8e7ad908489fdf3ceb496573f306a

GTEST_DIR_BASE := ./gtest
# Points to the root of Google Test, relative to where this file is.
GTEST_DIR := $(GTEST_DIR_BASE)/googletest-release-$(GTEST_VER)/googletest
GTEST_VER := 1.11.0
TESTS      :=                      \
   $(wildcard test/module1/*.cpp) \
   $(wildcard test/module2/*.cpp) \
   $(wildcard test/*.cpp)         \

# build結果
BUILD_TEST_DIR  := $(BUILD)/tests
BUILD_GTEST_DIR  := $(BUILD_TEST_DIR)/gtest

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

test_mihai.o : $(USER_DIR)/test_mihai.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test_mihai.cpp

test_mihai : test_mihai.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@



.PHONY: test

$(GTEST_DIR_BASE)/googletest-release-$(GTEST_VER):
	@mkdir -p $(@D)
	curl -OL https://github.com/google/googletest/archive/refs/tags/release-$(GTEST_VER).tar.gz
	tar -xvzf release-$(GTEST_VER).tar.gz
	mv googletest-release-$(GTEST_VER) $(GTEST_DIR_BASE)/
	rm -rf release-$(GTEST_VER).tar.gz



test: $(GTEST_DIR) $(TEST) $(SRC)
	INCLUDE += -I$(GTEST_DIR)



