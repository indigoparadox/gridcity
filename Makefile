
# vim: ft=make noexpandtab

GRIDCITY_C_FILES := src/main.c src/gridcity.c src/draw.c

MD := mkdir -p

CFLAGS_GCC :=

SANITIZE := -Werror -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined

gridcity: CC_GCC := gcc
gridcity: CFLAGS_GCC := $(shell pkg-config allegro --cflags) $(SANITIZE) -DRETROFLAT_ALLEGRO -DRETROFLAT_MOUSE -DRETROFLAT_OS_UNIX -Imaug/src
gridcity: LDFLAGS_GCC := $(shell pkg-config allegro --libs) $(SANITIZE)

.PHONY: clean

all: gridcity

gridcity: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

gridctyw.exe: $(addprefix obj/win32/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	#$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)
	wcl386 -l=nt_win -fe=$@ -s -3s -k128k alleg.lib $^

gridctyd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	#$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)
	wcl386 -l=dos32a -fe=$@ -s -3s -k128k dos/clib3s.lib alleg.lib $^

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC)

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	#$(CC_GCC) $(CFLAGS_GCC) -c -o $@ $<
	wcc386 -DDOS -bt=dos32a -s -3s -fo=$@ $(<:%.c=%)

obj/win32/%.o: %.c
	$(MD) $(dir $@)
	#$(CC_GCC) $(CFLAGS_GCC) -c -o $@ $<
	wcc386 -bt=nt_win -s -3s -fo=$@ $(<:%.c=%)
clean:
	rm -rf obj gridcity gridctyd.exe *.err

