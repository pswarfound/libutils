
all:
	make -C src -j
	make -C test -j
	
clean:
	make -C src clean
	make -C test clean