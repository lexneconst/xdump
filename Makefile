
PROJECTNAME=white_queen
PROJECTCODE=carroll

PROJECT_DIR=$(shell pwd)

LIBCRT=-lgcc

PROJECTA=xdump
EXECPX=src
LIBEXE_EXP=$(EXECPX)
OUTPUT_DIR=bin

all: $(PROJECTA) debian

OBJDIRENM_SRC=$(EXECPX)
OBJLIBENM_SRC=$(shell find $(OBJDIRENM_SRC) -name '*.c')
OBJLIBENM_OBJ=$(OBJLIBENM_SRC:%.c=%.o)
$(PROJECTA): $(OBJLIBENM_OBJ)
	$(CC) $(OBJLIBENM_OBJ) -I$(PROJECT_DIR)/include -L$(PROJECT_DIR)  -L"$(PROJECT_DIR)/$(OUTPUT_DIR)" $(CFLAGS) $(LIBCRT) -o $(OUTPUT_DIR)/$@


OBJDIREXEMOD_SRC=$(LIBEXE_EXP)
OBJLIBEXEMOD_SRC=$(shell find $(OBJDIREXEMOD_SRC) -name '*.o')
OBJLIBEXEMOD_OBJ=$(OBJLIBEXEMOD_SRC:%.o=%.x)

PACKAGE_DIR = debian_pkgs

clean:
	rm -rf $(OUTPUT_DIR)/$(PROJECTA)
	rm -rf $(OBJLIBEXEMOD_SRC)
	rm -rf $(PACKAGE_DIR)
	rm -rf debian*.deb

debian:
	mkdir $(PACKAGE_DIR)
	mkdir $(PACKAGE_DIR)/usr
	mkdir $(PACKAGE_DIR)/usr/bin
	mkdir $(PACKAGE_DIR)/DEBIAN
	cp -rf bin/xdump $(PACKAGE_DIR)/usr/bin/xdump
	echo "\
Package: xdump \r\n\
Version: 1.0 \r\n\
Maintainer: Xdump Project \r\n\
Architecture: all \r\n\
Description: Computer dump Packet \r\n\
" | tee $(PACKAGE_DIR)/DEBIAN/control
	dpkg-deb --build $(PWD)/$(PACKAGE_DIR)
