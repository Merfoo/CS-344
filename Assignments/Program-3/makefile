CXX = gcc
CXXFLAGS = -std=c99 -Wall -pedantic-errors

EXE = smallsh

HDRS = SmallSh.h BuiltInCommands.h Command.h VectorInt.h
SRCS := $(HDRS:.h=.c)
OBJS := $(SRCS:.c=.o)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.c $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(EXE)
