EXEC = dasm
CC = g++
$(EXEC):
	$(CC) -std=c++11 assign2.cpp -o $(EXEC)
	rm -f *.o
clean:
	rm -f *.o core a.out $(EXEC)