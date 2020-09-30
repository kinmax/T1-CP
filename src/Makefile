CPP = /usr/local/opt/llvm/bin/clang
CPPFLAGS = -I/usr/local/opt/llvm/include -fopenmp
LDFLAGS = -L/usr/local/opt/llvm/lib

all:
	gcc -o salesman salesman.c

clang: salesman.c
	$(CPP) $(CPPFLAGS) $^ -o salesman $(LDFLAGS)

clean:
	rm *.exe *.o
