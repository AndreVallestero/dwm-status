CC ?= gcc

dep_path = /usr

INCLUDES = -I$(dep_path)/include
LIBRARIES = -L$(dep_path)/lib

CFLAGS = -Wall -ggdb3 -Og $(INCLUDES)
LDFLAGS = $(LIBRARIES)

TARGET = dwm-status
c_files = dwm-status.c
objects = $(c_files:.c=.o)
headers = 

all: $(TARGET)

$(TARGET): $(objects) 
	$(CC) -o $@ $^ $(LDFLAGS) 

$(objects): %.o: %.c $(headers) Makefile
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(objects)