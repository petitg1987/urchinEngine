all: rel

debug:
	cd .build/debug && make

rel:
	cd .build/release && make

clean: clean-debug clean-rel

clean-debug:
	cd .build/debug && make clean

clean-rel:
	cd .build/release && make clean
