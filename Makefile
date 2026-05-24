TARGET_SRV = bin/server
TARGET_CLI = bin/client

SRC_SRV = $(wildcard src/server/*.c)
OBJ_SRV = $(SRC_SRV:src/server/%.c=obj/server/%.o)

SRC_CLI = $(wildcard src/client/*.c)
OBJ_CLI = $(SRC_CLI:src/client/%.c=obj/client/%.o)

run: clean default
	./$(TARGET_SRV) -f database.db -n -l

default: $(TARGET_SRV) $(TARGET_CLI)

clean:
	rm -f obj/server/*.o
	rm -f obj/client/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SRV): $(OBJ_SRV)
	gcc -o $@ $?

$(OBJ_SRV): obj/server/%.o: src/server/%.c
	gcc -c $< -o $@ -Iinc

$(TARGET_CLI): $(OBJ_CLI)
	gcc -o $@ $?

$(OBJ_CLI): obj/client/%.o: src/client/%.c
	gcc -c $< -o $@ -Iinc
