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

$(OBJ_DIR)/src/%.o: src/%.cpp
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
 
clean: clean_test
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"


GTEST_DIR_BASE := ./gtest
GTEST_VER := 1.11.0
GTEST_DIR := $(GTEST_DIR_BASE)/googletest-release-$(GTEST_VER)/googletest
BUILD_TEST_DIR  := $(BUILD)/tests
BUILD_GTEST_DIR  := $(BUILD)/gtest

.PHONY: test clean_gtest clean_test

$(BUILD_GTEST_DIR): $(GTEST_DIR)
	@mkdir -p $(GTEST_DIR_BASE)/googletest-release-$(GTEST_VER)/build
	cd $(GTEST_DIR_BASE)/googletest-release-$(GTEST_VER)/build && cmake .. && make
	@mkdir -p $(BUILD_GTEST_DIR)
	cp $(GTEST_DIR_BASE)/googletest-release-$(GTEST_VER)/build/lib/*.a $(BUILD_GTEST_DIR)/


$(GTEST_DIR):
	@mkdir -p $(@D)
	curl -OL https://github.com/google/googletest/archive/refs/tags/release-$(GTEST_VER).tar.gz
	tar -xvzf release-$(GTEST_VER).tar.gz
	mv googletest-release-$(GTEST_VER) $(GTEST_DIR_BASE)/
	rm -rf release-$(GTEST_VER).tar.gz

clean_gtest: 
	-@rm -rvf $(GTEST_DIR_BASE)/*
	-@rm -rvf $(BUILD_GTEST_DIR)/*

clean_test:
	-@rm -rvf $(BUILD_TEST_DIR)/*

# テストファイル一覧
TESTS_SRC      :=                 \
   $(wildcard test/module1/*.cpp) \
   $(wildcard test/module2/*.cpp) \
   $(wildcard test/*.cpp)         \

# テストファイルを実行可能にしたもの
TESTS_BUILD  := $(TESTS_SRC:test/%.cpp=$(BUILD_TEST_DIR)/%)

# mainが含まれている.oファイルはテスト時には省きたいので以下のようにmoduleのみをリンク対象とする
MODULEOBJ  =                    			 \
   $(wildcard build/objects/src/module1/*.o) \
   $(wildcard build/objects/src/module2/*.o) \

TEST_OBJECTS  := $(TESTS_SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_DEPENDENCIES := $(TEST_OBJECTS:.o=.d)
-include $(TEST_DEPENDENCIES)

$(OBJ_DIR)/test/%.o: test/%.cpp $(GTEST_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -I$(GTEST_DIR)/include/gtest -c $< -MMD -o $@

$(BUILD_TEST_DIR)/%: $(OBJ_DIR)/test/%.o $(OBJECTS) $(BUILD_GTEST_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MODULEOBJ) -pthread -lgtest_main -lgtest -L $(BUILD_GTEST_DIR)

# $(info TESTS_BUILD $(TESTS_BUILD))
# $(info MODULEOBJ $(MODULEOBJ))
# $(info TEST_OBJECTS $(TEST_OBJECTS))
# $(info BUILD_TEST_DIR $(BUILD_TEST_DIR))
# $(info TEST_DEPENDENCIES $(TEST_DEPENDENCIES))

test: $(TESTS_BUILD)