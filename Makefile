C_COMPILER=cc
CFLAGS=-std=c11 -Wall -Wextra -Werror
BIN_DIR=./bin
TARGET = chibirb
UNITY_ROOT=./lib/unity
SRC_FILES=\
  $(UNITY_ROOT)/src/unity.c \
  $(UNITY_ROOT)/src/unity_fixture.c \
  src/Lexer.c \
	test/TestLexer.c \
	test/test_runners/TestLexer_Runner.c \
	test/test_runners/all_tests.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src

all: clean default

default:
	@$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(BIN_DIR)/$(TARGET)
	@$(BIN_DIR)/$(TARGET)

clean:
	@rm -f $(BIN_DIR)/*.o $(BIN_DIR)/$(TARGET)
