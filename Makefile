CC		= g++
CPPFLAGS	= -std=c++11 -Wall -pthread
OBJS	= lib.o Bus.o Block.o BreakPoint.o CpuModule.o MemModule.o TimerModule.o DisplayModule.o logutil.o main.o
DEST	= /usr/local/bin
TARGET	= ancient-micom

all:	$(TARGET)
$(TARGET): $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS)

%.cpp.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

install:	$(TARGET)
	cp -p $(TARGET) $(DEST)
	
