CFLAGS	+= -g $(shell pkg-config --cflags libgnomeui-2.0) $(shell pkg-config --cflags libgnome-menu)
LDLIBS += $(shell pkg-config --libs libgnomeui-2.0) $(shell pkg-config --libs libgnome-menu)

all: appmenu2menuorg

install:
	install -d -m 755 ${DESTDIR}/usr/bin
	install -m 755 appmenu2menuorg ${DESTDIR}/usr/bin

clean:
	rm -f appmenu2menuorg *.o
