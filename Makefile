CC		= g++
CPPFLAGS	= -std=c++11 -Wall -pthread
OBJS	= lib.o Bus.o Block.o BreakPoint.o CpuModule.o MemModule.o TimerModule.o logutil.o main.o
TARGET	= vcpu

all:	$(TARGET)
$(TARGET): $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS)

%.cpp.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
