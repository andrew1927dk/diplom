-include local.defaults.mk
include local.mk

export BUILD_TARGET ?= $(DEFAULT_TARGET)

ifdef OPENOCD_HLA_SERIAL
export OPENOCD_HLA_SERIAL_STR:=-c 'adapter serial "$(OPENOCD_HLA_SERIAL)"'
endif

.PHONY: full all clean clean_all flash

all:
	make -C Target/$(BUILD_TARGET)

full:
	make -C Target/HUB BUILD_TARGET=HUB all
	make -C Target/DEVBOARD BUILD_TARGET=DEVBOARD all

clean:
	make -C Target/$(BUILD_TARGET) clean

clean_all:
	make -C Target/HUB clean
	make -C Target/DEVBOARD clean
	
docs:
	doxygen Doxyfile

docs_view: docs
	xdg-open ./Doc/html/index.html

docs_clean:
	-rm -fR Doc

flash:
	make -C Target/$(BUILD_TARGET) flash

reset:
	make -C Target/$(BUILD_TARGET) reset

erase:
	make -C Target/$(BUILD_TARGET) erase
