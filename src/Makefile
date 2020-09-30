CPP = /usr/local/opt/llvm/bin/clang
CPPFLAGS = -I/usr/local/opt/llvm/include -fopenmp
LDFLAGS = -L/usr/local/opt/llvm/lib

all:
	gcc -o tsp_brute tsp_brute.c

clang: tsp_brute.c
	$(CPP) $(CPPFLAGS) $^ -o tsp_brute $(LDFLAGS)

clean:
	rm *.exe *.o
