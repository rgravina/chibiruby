#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OSTYPE),cygwin)
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
elseifeq ($(OSTYPE),msys)
    CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.exe
elseifeq ($(OS),Windows_NT)
	CLEANUP = del /F /Q
	MKDIR = mkdir
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=
endif

UNITY_ROOT=./lib/unity
C_COMPILER=cc
TARGET = chibirb$(TARGET_EXTENSION)

CFLAGS=-std=c11
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Werror 

SRC_FILES=$(UNITY_ROOT)/src/unity.c src/ProductionCode.c test/TestProductionCode.c test/test_runners/TestProductionCode_Runner.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src

all: clean default

default:
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test/TestProductionCode.c test/test_runners/TestProductionCode_Runner.c
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET)
	./$(TARGET)

clean:
	$(CLEANUP)
