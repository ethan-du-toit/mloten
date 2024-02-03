VPATH = ../src
CFLAGS += -O3 -flto 
src = $(wildcard $(VPATH)/*.c)
obj = $(notdir $(src:.c=.o))
dep = $(obj:.o=.d)  # one dependency file for each source

LDFLAGS = -lm

rv: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) `sdl2-config --cflags --libs`

-include $(dep)   # include all dep files in the makefile

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) rv

.PHONY: cleandep
cleandep:
	rm -f $(dep)
