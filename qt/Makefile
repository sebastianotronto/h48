all: nissyqt

nissyqt:
	mkdir -p build
	cmake . -B build
	cd build && make
	cp build/appnissyqt ./run

run:
	QT_LOGGING_RULES="*.debug=true; qt.*.debug=false" ./run

clean:
	rm -rf run build

.PHONY: all nissyqt run clean
