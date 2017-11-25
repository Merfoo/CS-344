CXX = gcc
CXX_FLAGS = -std=c99 -Wall

KEYGEN_EXE = keygen
KEYGEN_HDRS = Keygen.h
KEYGEN_SRCS = $(KEYGEN_HDRS:.h=.c)
KEYGEN_OBJS = $(KEYGEN_SRCS:.c=.o)

.PHONY: all clean

all: $(KEYGEN_EXE)

$(KEYGEN_EXE): $(KEYGEN_OBJS)
	$(CXX) $(CXX_FLAGS) $(KEYGEN_OBJS) -o $@

$(KEYGEN_OBJS): $(KEYGEN_HDRS)

%.o: %.c
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -f $(KEYGEN_OBJS) $(KEYGEN_EXE)
