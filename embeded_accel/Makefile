#Make file for building the project
#Sanjay Nair

CC=gcc
CFLAGS=-Wall -ansi -pedantic
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=accelerator

all: $(SOURCES) $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFALGS)

clean:
	   rm -f $(TARGET) $(OBJECTS)

# For testing purpose
T_FLDR=./test
T_FILES=$(basename $(wildcard $(T_FLDR)/*.bin))
test_clean:
	rm -f $(T_FLDR)/*.s.out
	
test: all test_clean
		@$(foreach t, $(T_FILES), ./$(TARGET) -i $(t).bin -o $(t).s.out; diff $(t).s.out $(t).out;)
