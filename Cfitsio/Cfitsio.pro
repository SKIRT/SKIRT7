#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = cfitsio
QT -= core gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization and suppress all warnings
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3 -w

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    crc32.h \
    deflate.h \
    eval_defs.h \
    eval_tab.h \
    fitsio.h \
    fitsio2.h \
    fpack.h \
    group.h \
    grparser.h \
    inffast.h \
    inffixed.h \
    inflate.h \
    inftrees.h \
    longnam.h \
    region.h \
    trees.h \
    zconf.h \
    zlib.h \
    zutil.h

SOURCES += \
    adler32.c \
    buffers.c \
    cfileio.c \
    checksum.c \
    crc32.c \
    deflate.c \
    drvrfile.c \
    drvrmem.c \
    editcol.c \
    edithdu.c \
    eval_f.c \
    eval_l.c \
    eval_y.c \
    fits_hcompress.c \
    fits_hdecompress.c \
    fitscore.c \
    getcol.c \
    getcolb.c \
    getcold.c \
    getcole.c \
    getcoli.c \
    getcolj.c \
    getcolk.c \
    getcoll.c \
    getcols.c \
    getcolsb.c \
    getcolui.c \
    getcoluj.c \
    getcoluk.c \
    getkey.c \
    group.c \
    grparser.c \
    histo.c \
    imcompress.c \
    infback.c \
    inffast.c \
    inflate.c \
    inftrees.c \
    iraffits.c \
    modkey.c \
    pliocomp.c \
    putcol.c \
    putcolb.c \
    putcold.c \
    putcole.c \
    putcoli.c \
    putcolj.c \
    putcolk.c \
    putcoll.c \
    putcols.c \
    putcolsb.c \
    putcolu.c \
    putcolui.c \
    putcoluj.c \
    putcoluk.c \
    putkey.c \
    quantize.c \
    region.c \
    ricecomp.c \
    scalnull.c \
    swapproc.c \
    trees.c \
    uncompr.c \
    wcssub.c \
    wcsutil.c \
    zcompress.c \
    zuncompress.c \
    zutil.c
