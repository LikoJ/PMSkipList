CC = g++
AR = ar crv

FLAGS = -g -std=c++11

LIBS = -lpmem

OBJECT := arena.o \
		  skiplist.o

TARGET = libpmskiplist.a

$(TARGET) : $(OBJECT)
	$(AR) $@ $^

.cpp.o :
	$(CC) -c $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm $(TARGET) *.o