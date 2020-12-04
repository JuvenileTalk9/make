CXX      = g++
RM       = rm -rf
OBJS     = main.o file_check.o
PROGRAM  = hello
CXXFLAGS = -O4 -Wall -I/home/centos/boost_1_74_0/build/include
LDFLAGS  = -L/home/centos/boost_1_74_0/build/lib
LIBS     = -lboost_filesystem
DEST     = /usr/local/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:
	$(RM) *.o $(PROGRAM)

install: $(PROGRAM)
	install -d $(DEST)
	install -s $(PROGRAM) $(DEST)
