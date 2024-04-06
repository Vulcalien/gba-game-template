# Vulcalien's GBA Makefile

# === Detect OS ===
ifeq ($(OS),Windows_NT)
    CURRENT_OS := WINDOWS
else
    CURRENT_OS := UNIX
endif

# === Basic Info ===
OUT_FILENAME := template

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRC_SUBDIRS := sound scene

# === Compilation ===
CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -O3 -fomit-frame-pointer -marm -mcpu=arm7tdmi\
            -Wall -pedantic

ASFLAGS := -mcpu=arm7tdmi

LDFLAGS := -Tlnkscript -nostartfiles
LDLIBS  :=

ifeq ($(CURRENT_OS),UNIX)
    CC      := arm-none-eabi-gcc
    AS      := arm-none-eabi-as
    OBJCOPY := arm-none-eabi-objcopy

    EMULATOR := mgba-qt
else ifeq ($(CURRENT_OS),WINDOWS)
    CC      :=
    AS      :=
    OBJCOPY :=

    EMULATOR :=
endif

# === Extensions & Commands ===
OBJ_EXT := o
ELF_EXT := elf
GBA_EXT := gba

ifeq ($(CURRENT_OS),UNIX)
    MKDIR      := mkdir
    MKDIRFLAGS := -p

    RM      := rm
    RMFLAGS := -rfv
else ifeq ($(CURRENT_OS),WINDOWS)
    MKDIR      := mkdir
    MKDIRFLAGS :=

    RM      := rmdir
    RMFLAGS := /Q /S
endif

# === Resources ===

# list of source file extensions
SRC_EXT := s c

# list of source directories
SRC_DIRS := $(SRC_DIR)\
            $(foreach SUBDIR,$(SRC_SUBDIRS),$(SRC_DIR)/$(SUBDIR))

# list of source files
SRC := $(foreach DIR,$(SRC_DIRS),\
         $(foreach EXT,$(SRC_EXT),\
           $(wildcard $(DIR)/*.$(EXT))))

# list of object files
OBJ := $(SRC:%=$(OBJ_DIR)/%.$(OBJ_EXT))

# list of object directories
OBJ_DIRS := $(SRC_DIRS:%=$(OBJ_DIR)/%)

# output files
OUT_ELF := $(BIN_DIR)/$(OUT_FILENAME).$(ELF_EXT)
OUT     := $(BIN_DIR)/$(OUT_FILENAME).$(GBA_EXT)

# === Targets ===

.PHONY: all run build clean release

all: build

run:
	$(EMULATOR) $(OUT)

build: $(OUT)

clean:
	@$(RM) $(RMFLAGS) $(BIN_DIR) $(OBJ_DIR)

# generate GBA file
$(OUT): $(OUT_ELF)
	$(OBJCOPY) -O binary $^ $@

# generate ELF file
$(OUT_ELF): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# compile .s files
$(OBJ_DIR)/%.s.$(OBJ_EXT): %.s | $(OBJ_DIRS)
	$(AS) $(ASFLAGS) -o $@ $<

# compile .c files
$(OBJ_DIR)/%.c.$(OBJ_EXT): %.c | $(OBJ_DIRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# create directories
$(BIN_DIR) $(OBJ_DIRS):
	$(MKDIR) $(MKDIRFLAGS) "$@"

release:
	scripts/release.sh "$(OUT)"

-include $(OBJ:.$(OBJ_EXT)=.d)
