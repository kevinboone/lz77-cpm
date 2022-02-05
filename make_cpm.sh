
CFLAGS="-F -DCPM -DNAME=\"lz77\""

cpm cc $CFLAGS -o _bitio.asm bitio.c
cpm cc $CFLAGS -o _compat.asm compat.c
cpm cc $CFLAGS -o _getopt.asm getopt.c
cpm cc $CFLAGS -o _lz77dec.asm lz77dec.c
cpm cc $CFLAGS -o _lz77com.asm lz77com.c
cpm cc $CFLAGS -o _main.asm main.c
cpm cc $CFLAGS -o _maincat.asm maincat.c
cpm cc $CFLAGS -o _tree.asm tree.c
cpm cc $CFLAGS -o _error.asm error.c

cpm as -o bi.o _bitio.asm
cpm as -o co.o _compat.asm
cpm as -o ge.o _getopt.asm
cpm as -o lzc.o _lz77com.asm
cpm as -o lzd.o _lz77dec.asm
cpm as -o main.o _main.asm
cpm as -o maincat.o _maincat.asm
cpm as -o tr.o _tree.asm
cpm as -o er.o _error.asm
cpm as -o gc.o getchne.asm

cpm ln -o lz77.com main.o co.o ge.o lzc.o lzd.o bi.o tr.o er.o c.lib m.lib
cpm ln -o lz77cat.com maincat.o co.o ge.o lzd.o bi.o tr.o er.o  gc.o c.lib m.lib

