main:
	cmake . -B build
	make -C build
clean:
	rm -rf build/*
