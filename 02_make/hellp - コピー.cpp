CXX		= g++
RM		= rm -rf
OBJS	= main.o calc.o
PROGRAM = hello
DEST    = /usr/local/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) -o $(PROGRAM)

clean:
	$(RM) *.o $(PROGRAM)

install: $(PROGRAM)
	install -d $(DEST)
	install -s $(PROGRAM) $(DEST)
