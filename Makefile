all: rel

debug:
	cd .build/debug && make

rel:
	cd .build/release && make

clean:
	cd .build/debug && make clean && cd ../release && make clean
