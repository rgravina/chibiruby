C_COMPILER=cc
CFLAGS=-std=c11 -Wall -Wextra
BIN_DIR=./bin
TARGET = chibirb
TARGET_TEST = chibirb_test
UNITY_ROOT=./lib/unity
SRC_FILES=\
  src/Lexer.c
SRC_FILES_TEST=\
  $(UNITY_ROOT)/src/unity.c \
  $(UNITY_ROOT)/src/unity_fixture.c \
	test/TestLexer.c \
	test/test_runners/TestLexer_Runner.c \
	test/test_runners/all_tests.c \
	$(SRC_FILES)
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src

all: test
chibirb: clean _chibirb
test: clean _test

_chibirb:
	@$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) src/main.c -o $(BIN_DIR)/$(TARGET)
	@$(BIN_DIR)/$(TARGET)

_test:
	@$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SRC_FILES_TEST) -o $(BIN_DIR)/$(TARGET_TEST)
	@$(BIN_DIR)/$(TARGET_TEST)

clean:
	@rm -f $(BIN_DIR)/*.o $(BIN_DIR)/$(TARGET)
