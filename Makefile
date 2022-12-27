
# vim: ft=make noexpandtab

GRIDCITY_C_FILES := src/main.c src/gridcity.c src/draw.c

MD := mkdir -p

CFLAGS_GCC :=

CFLAGS_WATCOM := -imaug/src

ifneq ("$(BUILD)","RELEASE")
	CFLAGS_WATCOM += -d3 -we
endif

SANITIZE := -Werror -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined

gridcity: CC_GCC := gcc
gridcity: CFLAGS_GCC := $(shell pkg-config allegro --cflags) $(SANITIZE) -DRETROFLAT_API_ALLEGRO -DRETROFLAT_OS_UNIX -Imaug/src
gridcity: LDFLAGS_GCC := $(shell pkg-config allegro --libs) $(SANITIZE)

.PHONY: clean

all: gridcity

# Unix

gridcity: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC)

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

obj/win32/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -bt=nt_win -fo=$@ $(<:%.c=%)

clean:
	rm -rf obj gridcity gridctyd.exe *.err *.rex gridctyw.exe

