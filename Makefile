
# vim: ft=make noexpandtab

SCREENSAVER=1
C_FILES := src/main.c src/gridcity.c src/draw.c

DEFINES_GCC += -DBLOCKS_XPM

include maug/Makefile.inc

.PHONY: clean

all: grdcty.ale grdcty.sdl grdctyw.exe grdctyd.exe grdctynt.exe grdcty.html

$(eval $(call DIRTOXPMS,blocks,src))

# Unix.Allegro

$(eval $(call TGTUNIXALE,grdcty,src/blocks_xpm.h))

# Unix.SDL

$(eval $(call TGTUNIXSDL,grdcty,src/blocks_xpm.h))

# WASM

$(eval $(call TGTWASMSDL,grdcty,src/blocks_xpm.h))

# Win386

$(eval $(call TGTWIN386,grdcty,src/blocks_xpm.h))

# DOS

$(eval $(call TGTDOSALE,grdcty,src/blocks_xpm.h))

# Windows NT

$(eval $(call TGTWINNT,grdcty,src/blocks_xpm.h))

clean:
	rm -rf $(CLEAN_TARGETS)

