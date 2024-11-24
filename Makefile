INC_DIR = inc
SRC_DIR = src
TEST_DIR = test
UNITY_SRC_DIR = unity/src
BUILD_DIR = build
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -ggdb -std=c99
LDFLAGS=-lSDL2 -lSDL2_ttf
TARGET_GAME = game
TARGET_TEST = tests

# Make Directories 
$(shell mkdir -p $(BUILD_DIR))

.PHONY: test
all: test game
test:
	$(CC) $(CFLAGS) -I $(INC_DIR) -I $(UNITY_SRC_DIR) $(TEST_DIR)/test_game_logic.c $(SRC_DIR)/game_logic.c $(UNITY_SRC_DIR)/unity.c -o $(BUILD_DIR)/$(TARGET_TEST)
game:
	$(CC) $(CFLAGS) -I $(INC_DIR) $(LDFLAGS) $(SRC_FILES) -o $(BUILD_DIR)/$(TARGET_GAME)
clean:
	rm -rf $(BUILD_DIR)/*