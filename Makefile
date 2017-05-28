all: release

debug:
	cd .build/debug && make

release:
	cd .build/release && make

clean:
	cd .build/debug && make clean && cd ../release && make clean