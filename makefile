CC := g++
CFLAGS := -O -Wall -std=c++11 `sdl2-config --cflags`
LFLAGS := `sdl2-config --libs` -lSDL2_ttf

default: bin/demo

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -I ./include -c -o $@ $<

bin/demo: build/sdl_trigger.o build/visual_demo.o build/util.o
	$(CC) $^ $(LFLAGS) -o bin/demo

run: bin/demo
	./bin/demo

clean:
	rm -f build/*.o
	rm -f bin/demo

debug-%:
	@echo $* = $($*)

.PHONY: clean