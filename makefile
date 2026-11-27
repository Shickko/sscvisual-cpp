main:
	cmake . -B build
	make -C build
win64:
	cmake . -B build
	cmkae --build build
clean:
	rm -rf build/*
