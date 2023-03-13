
# vim: ft=make noexpandtab

SCREENSAVER=1
C_FILES := src/main.c src/gridcity.c src/draw.c

DEFINES_GCC += -DBLOCKS_XPM

include maug/Makefile.inc

.PHONY: clean

all: grdcty.ale grdcty.sdl grdctyw.exe grdctyd.exe grdctynt.exe grdcty.html grdctyw3.exe

$(eval $(call DIRTOXPMS,blocks,src))

# Unix.Allegro

$(eval $(call TGTUNIXALE,grdcty))

# Unix.SDL

$(eval $(call TGTSDLICO,grdcty))

$(eval $(call TGTUNIXSDL,grdcty))

# WASM

$(eval $(call TGTWASMSDL,grdcty))

# Win386

$(eval $(call TGTWINICO,grdcty))

$(eval $(call TGTWIN386,grdcty))

# TODO: Until WinG works with Win386...
WING=1

$(eval $(call TGTWIN16,grdcty))

# DOS

$(eval $(call TGTDOSALE,grdcty))

# Windows NT

$(eval $(call TGTWINNT,grdcty))

clean:
	rm -rf $(CLEAN_TARGETS)

