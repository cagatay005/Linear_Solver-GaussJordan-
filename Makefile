CC := gcc
STD := -std=c11
WARN := -Wall -Wextra -Wpedantic -Wconversion -Wshadow
INCLUDES := -Iinclude

# MinGW'nin eski printf implementasyonu %zu gibi C99/C11 boyut
# belirteclerini desteklemez; bu makro MinGW'ye kendi ANSI uyumlu
# printf'ini kullanmasini soyler. Diger platformlarda (Linux/macOS)
# etkisizdir.
ifeq ($(OS),Windows_NT)
    PLATFORM_DEFS := -D__USE_MINGW_ANSI_STDIO=1
    EXE_EXT := .exe
else
    PLATFORM_DEFS :=
    EXE_EXT :=
endif

DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2

CFLAGS := $(STD) $(WARN) $(INCLUDES) $(PLATFORM_DEFS) $(DEBUG_FLAGS)

SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:.c=.o)

TARGET := solver$(EXE_EXT)

.PHONY: all run test clean release

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

release: CFLAGS := $(STD) $(WARN) $(INCLUDES) $(PLATFORM_DEFS) $(RELEASE_FLAGS)
release: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "== test_unique =="
	@./$(TARGET) tests/test_unique.txt < tests/mode_quiet.txt
	@echo "== test_row_swap =="
	@./$(TARGET) tests/test_row_swap.txt < tests/mode_quiet.txt
	@echo "== test_infinite =="
	@./$(TARGET) tests/test_infinite.txt < tests/mode_quiet.txt
	@echo "== test_inconsistent =="
	@./$(TARGET) tests/test_inconsistent.txt < tests/mode_quiet.txt
	@echo "== test_rectangular =="
	@./$(TARGET) tests/test_rectangular.txt < tests/mode_quiet.txt
	@echo "== test_decimal =="
	@./$(TARGET) tests/test_decimal.txt < tests/mode_quiet.txt
	@echo "== test_underdetermined =="
	@./$(TARGET) tests/test_underdetermined.txt < tests/mode_quiet.txt
	@echo "== test_near_singular =="
	@./$(TARGET) tests/test_near_singular.txt < tests/mode_quiet.txt

clean:
	rm -f $(OBJS) $(TARGET) solution_report.txt
