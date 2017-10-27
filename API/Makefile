IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -lbluetooth $(shell pkg-config --cflags glib-2.0)

ODIR=.

LIBS=-lbluetooth $(shell pkg-config --libs glib-2.0)

_DEPS = att.h btio.h gattrib.h gatt.h ble_api.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = utils.o att.o btio.o gattrib.o gatt.o ble_api.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ble_api: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 