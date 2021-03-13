CC=g++
CFLAGS=-Wall -g
LDFLAGS=-lGL -lGLEW -lassimp `pkg-config --static --libs glfw3`
BUILDDIR=_build
sources=$(shell echo *.cpp)
headers=$(shell echo *.h)
objects=$(patsubst %.cpp,$(BUILDDIR)/%.o,$(sources))

$(BUILDDIR)/rendering: $(objects) $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $(objects)  $(LDFLAGS)

$(BUILDDIR)/%.o: %.cpp %.h $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/rendering.o: rendering.cpp $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
