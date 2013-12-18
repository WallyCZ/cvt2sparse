CC=gcc

CFLAGS=-Wall -g -std=gnu99

OCTOTHORPE=../octothorpe
OCTOTHORPE_LIBRARY=$(OCTOTHORPE)/MINGW32_release/octothorpe.a
PORG=../porg
PORG_LIBRARY=$(PORG)/MINGW32_release/porg.a

all: cvt2sparse.c
	$(CC) cvt2sparse.c $(CFLAGS) -I$(OCTOTHORPE) -I$(PORG) -D_DEBUG -o cvt2sparse.exe $(OCTOTHORPE_LIBRARY) $(OCTOTHORPE)/MINGW32_release/lisp.o $(PORG_LIBRARY) -lpsapi
