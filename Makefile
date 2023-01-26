
# vim: ft=make noexpandtab

GRIDCITY_C_FILES := src/main.c src/gridcity.c src/draw.c

MD := mkdir -p
CC_GCC := gcc

DEFINES_GLOBAL = -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"

CFLAGS_GCC := -Imaug/src $(DEFINES_GLOBAL)
CFLAGS_WATCOM := -imaug/src $(DEFINES_GLOBAL)
LDFLAGS_GCC := -lm

ifneq ("$(BUILD)","RELEASE")
	CFLAGS_GCC += -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined -DDEBUG -DDEBUG_LOG -DDEBUG_THRESHOLD=1
	LDFLAGS_GCC += -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
	CFLAGS_WATCOM += -d3 -we -DDEBUG -DDEBUG_LOG -DDEBUG_THRESHOLD=1
endif

.PHONY: clean

all: gridcity.ale gridcity.sdl gridctyw.exe gridctyd.exe gridctnt.exe gridcity.html

# Unix.Allegro

gridcity.ale: $(addprefix obj/$(shell uname -s)-allegro/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC) $(shell pkg-config allegro --libs)

obj/$(shell uname -s)-allegro/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC) -DRETROFLAT_OS_UNIX \
		$(shell pkg-config allegro --cflags) -DRETROFLAT_API_ALLEGRO

# Unix.SDL

gridcity.sdl: $(addprefix obj/$(shell uname -s)-sdl/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC) $(shell pkg-config sdl2 --libs) -lSDL_ttf

obj/$(shell uname -s)-sdl/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC) -DRETROFLAT_OS_UNIX \
		$(shell pkg-config sdl2 --cflags) -DRETROFLAT_API_SDL2

# WASM

gridcity.html: $(addprefix obj/wasm/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	emcc -o $@ $^ -s USE_SDL=2 -s USE_SDL_TTF=2

obj/wasm/%.o: %.c
	$(MD) $(dir $@)
	emcc -c -o $@ $< -DRETROFLAT_OS_WASM -DRETROFLAT_API_SDL2 -s USE_SDL=2 -Imaug/src -s USE_SDL_TTF=2

# Win386

gridctyw.rex: $(addprefix obj/win16/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	wcl386 -l=win386 -fe=$@ $^

gridctyw.exe: gridctyw.rex
	wbind $< -s $(WATCOM)/binw/win386.ext -R $@

obj/win16/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -DRETROFLAT_OS_WIN -DRETROFLAT_API_WIN16 $(CFLAGS_WATCOM) \
		-i=$(WATCOM)/h/win -bt=windows -fo=$@ $(<:%.c=%)

# DOS

gridctyd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	wcl386 -l=dos32a -fe=$@ -s -3s -k128k dos/clib3s.lib alleg.lib $^

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	#$(CC_GCC) $(CFLAGS_GCC) -c -o $@ $<
	wcc386 -DRETROFLAT_OS_DOS -DRETROFLAT_API_ALLEGRO $(CFLAGS_WATCOM) \
		-bt=dos32a -s -3s -fo=$@ $(<:%.c=%)

# Windows NT

gridctnt.exe: $(addprefix obj/win32/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	wcl386 -l=nt_win -fe=$@ $^

obj/win32/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -DRETROFLAT_SCREENSAVER \
		-DRETROFLAT_OS_WIN -DRETROFLAT_API_WIN32 $(CFLAGS_WATCOM) \
		-i=$(WATCOM)/h/nt -bt=nt -fo=$@ $(<:%.c=%)

clean:
	rm -rf obj gridcity gridctyd.exe *.err *.rex gridctyw.exe gridctnt.exe

