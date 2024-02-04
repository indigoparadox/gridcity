
# vim: ft=make noexpandtab

#SDL_VER_UNIX=2
#WING=1
VDP=1
SCREENSAVER=1
C_FILES := src/main.c src/gridcity.c src/draw.c

DEFINES_GCC += -DBLOCKS_XPM

include maug/Makefile.inc

.PHONY: clean

all: grdcty.ale grdcty.sdl grdctyw.exe grdctyd.exe grdctynt.exe grdcty.html grdctyw3.exe

$(eval $(call DIRTOXPMS,blocks,src))

# Nintendo DS

$(eval $(call TGTNDSLIBN,grdcty,grdcty.bmp))

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

$(eval $(call TGTWIN16,grdcty))

# DOS

$(eval $(call TGTDOSALE,grdcty))

# Windows NT

$(eval $(call TGTWINNT,grdcty))

$(eval $(call TGTWINNTGCC,grdcty))

clean:
	rm -rf $(CLEAN_TARGETS)

