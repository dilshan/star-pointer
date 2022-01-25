DEVICE          = stm32f103c8t6
OPENCM3_DIR     = libopencm3
OBJS            += systick.o transform.o protocol.o datetime.o i2c.o configmanager.o logger.o sysrtc.o adxl345.o hmc5883l.o main.o

CFLAGS          += -Os -ggdb3
CPPFLAGS		+= -MD -DSYS_CPU_FREQ=72000000
LDFLAGS         += -static -nostartfiles 
LDLIBS          += -Wl,--start-group -lm -lc -lgcc -lnosys -Wl,--end-group 

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

.PHONY: clean all

all: starpointer.elf starpointer.hex

clean:
	$(Q)$(RM) -rf starpointer.* *.o *.d

size:
	$(SIZE) starpointer.elf

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk