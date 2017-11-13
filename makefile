ifeq ($(BUILD),debug)
	FLAGS = -O0 -Wall -g -DDEBUG
else
	FLAGS = -O3 -Wall -Werror -s -DNDEBUG
endif

GENERAL_HELPER = ~/programming_projects/c/general/bin

PKG_CONFIG_LIBS = gtk+-3.0 glib-2.0 gdk-pixbuf-2.0

MAIN_PKG_CONFIG_LIBS = $(PKG_CONFIG_LIBS)

TEST_PKG_CONFIG_LIBS = $(PKG_CONFIG_LIBS) cmockery2

bin/main :
	gcc $(FLAGS) `pkg-config --cflags $(MAIN_PKG_CONFIG_LIBS)` \
		-o bin/main \
		$(GENERAL_HELPER)/general_helper \
		src/main.c `pkg-config --libs $(MAIN_PKG_CONFIG_LIBS)`

clean:
	rm -f bin/*

release:
	make clean
	make

debug:
	make clean
	make "BUILD=debug"


