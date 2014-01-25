# build environment
CPU = armv6j
PREFIX = arm-none-eabi
BOARD = kernel/arch/
BUILD = kernel/build/
SOURCE = kernel/
DATA = kernel/data/
LIBRARIES = kernel/libraries
INCLUDE = kernel/include

#board c files
OBJS        = $(patsubst $(BOARD)%.c, $(BUILD)%.o, $(wildcard $(BOARD)*.c))

#board assembly files
OBJS        += $(patsubst $(BOARD)%.s, $(BUILD)%.o, $(wildcard $(BOARD)*.s))

#kernel c files
OBJS        += $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(wildcard $(SOURCE)*.c))
OBJS        += $(patsubst $(SOURCE)structures/%.c, $(BUILD)%.o, $(wildcard $(SOURCE)structures/*.c))

#Data
OBJS        += $(patsubst $(DATA)%.bin, $(BUILD)%.o, $(wildcard $(DATA)*.bin))

# Build flags
INCLUDES    	:= -I $(INCLUDE)
BASEFLAGS   	:= -ffreestanding -std=gnu99
OPTIMIZEFLAGS	:= -march=$(CPU) -O3 -flto -fuse-linker-plugin
WARNFLAGS   	:= -Wall -Wextra -Werror
CFLAGS      	:=  $(INCLUDES) $(BASEFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) 
ASFLAGS     	:= -D__ASSEMBLY__ $(CFLAGS)
LINKERFLAGS 	:= -nostdlib -nostartfiles -nodefaultlibs $(CFLAGS) -L$(LIBRARIES) -lcsud

#build rules
all: kernel.img
  
kernel.elf: $(OBJS) link-arm-eabi.ld
	$(PREFIX)-gcc $(OBJS) -L. -Tlink-arm-eabi.ld $(LINKERFLAGS) -o $@
 
kernel.img: kernel.elf
	$(PREFIX)-objcopy kernel.elf -O binary kernel.img
	$(RM) -f kernel.elf
 
clean:
	$(RM) -f $(OBJS) kernel.elf kernel.img
 
dist-clean: clean
	$(RM) -f *.d
	

# Kernel C
$(BUILD)%.o: $(SOURCE)%.c
	$(PREFIX)-gcc $(CFLAGS) -c $< -o $@

# Structures
$(BUILD)%.o: $(SOURCE)structures/%.c
	$(PREFIX)-gcc $(CFLAGS) -c $< -o $@
	
# C board
$(BUILD)%.o: $(BOARD)%.c
	$(PREFIX)-gcc $(CFLAGS) -c $< -o $@
	
# Assembly
$(BUILD)%.o: $(BOARD)%.s
	$(PREFIX)-gcc $(ASFLAGS) -c $< -o $@

# Data
$(BUILD)%.o: $(DATA)%.bin
	$(PREFIX)-objcopy -I binary -O $(PREFIX) -B arm $< $@
