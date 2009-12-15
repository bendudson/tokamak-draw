CFLAGS=-Wall
INCLUDE=
LIB=-lglut

CC=gcc
LD=gcc

TARGET=tokamak_draw
SRC=tokamak_draw.c gl2ps.c model.c parse_nextline.c
INC=tokamak_draw.h model.h

OBJ = $(SRC:%.c=%.o)

.PHONY:all
all: $(TARGET)

$(TARGET): $(OBJ) Makefile	
	@echo "  Building " $(TARGET)
	@$(LD) -o $(TARGET) $(OBJ) $(LIB)

$(OBJ): $(@F:.o=.c) $(INC) Makefile
%.o: %.c
	@echo "  Compiling " $(@F:.o=.c)
	@$(CC) $(CFLAGS) -c $(@F:.o=.c) -o $@ $(INCLUDE)

.PHONY:clean
clean:
	@rm -f $(OBJ) $(TARGET)