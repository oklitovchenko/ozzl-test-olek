.PHONY: all build link test clean server client

ifndef ($(HOME))
    export HOME = /tmp
endif

all:
	$(MAKE) -j1 link

link: build

build: clean server client test

server:
	cd source/server; qmake && $(MAKE)

client:
	cd source/client; qmake && $(MAKE)

test:
	mkdir run || true
	rm -f ./run/*
	./test.sh

clean:
	rm -rf bin build run
	rm -f ./*/*.o ./*/*/.qmake.* ./*/*/Makefile ./*/moc_*
