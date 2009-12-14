TARGET=tokamak_draw

$(TARGET): $(TARGET).c
	gcc -o $(TARGET) gl2ps.c $(TARGET).c -lglut

.PHONY:clean
clean:
	@rm -f *.o $(TARGET)