CC = clang
CFLAGS = -Wall -Wextra -g
SRC_DIR = src
OBJ_DIR = build
SRC = $(SRC_DIR)/*.c
OUT = $(OBJ_DIR)/sudoku

$(OUT): $(SRC)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -rf $(OBJ_DIR)

run: $(OUT)
	./$(OUT)