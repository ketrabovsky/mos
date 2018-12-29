include makefile.conf
NAME=main
STARTUP_DEFS=-D__STARTUP_CLEAR_BSS -D__START=main 

LDSCRIPTS=-L. -L$(BASE)/ldscripts -T link.ld
LFLAGS=$(USE_NANO) $(USE_NOHOST) $(LDSCRIPTS) $(GC) $(MAP) 

csrc = $(wildcard src/*.c)
obj = $(csrc:.c=.o)

binary: $(NAME)-$(CORE).elf
	$(OBJCOPY) -O binary $< $(NAME).bin

$(NAME)-$(CORE).elf: $(obj) src/thread_asm.S src/$(STARTUP) 
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

objdump: 
	$(OBJDUMP) -ds $(NAME)-$(CORE).elf | less

flash:
	@./jflash.sh $(NAME)

clean: 
	rm -f $(NAME)*.axf *.map src/*.o *.elf *.bin
