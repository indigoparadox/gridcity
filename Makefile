
# vim: ft=make noexpandtab

GRIDCITY_C_FILES := src/main.c src/gridcity.c

MD := mkdir -p

CFLAGS_GCC :=

SANITIZE := -Werror -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined

gridcity: CC_GCC := gcc
gridcity: CFLAGS_GCC := $(shell pkg-config allegro --cflags) $(SANITIZE)
gridcity: LDFLAGS_GCC := $(shell pkg-config allegro --libs) $(SANITIZE)

.PHONY: clean

all: gridcity

gridcity: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(GRIDCITY_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC)

clean:
	rm -rf obj gridcity *.err

