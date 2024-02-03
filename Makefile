# Vulcalien's GBA Makefile
#
# Made for the 'gcc' compiler

# === DETECT OS ===
ifeq ($(OS),Windows_NT)
	CURRENT_OS := WINDOWS
else
	CURRENT_OS := UNIX
endif

# ========= EDIT HERE =========
OUT_FILENAME := template

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRC_SUBDIRS := sound scene level

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

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -O3 -fomit-frame-pointer -marm -mcpu=arm7tdmi\
            -Wall -pedantic

ASFLAGS  := -mcpu=arm7tdmi

LDFLAGS := -Tlnkscript -nostartfiles
LDLIBS  :=
# =============================

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

# === OTHER ===
# list of source file extensions
SRC_EXT := s c

# list of source files
SRC := $(foreach EXT,$(SRC_EXT),\
           $(wildcard $(SRC_DIR)/*.$(EXT))\
           $(foreach DIR,$(SRC_SUBDIRS),\
               $(wildcard $(SRC_DIR)/$(DIR)/*.$(EXT))))

# list of object files
OBJ := $(foreach EXT,$(SRC_EXT),\
           $(patsubst $(SRC_DIR)/%.$(EXT),$(OBJ_DIR)/$(EXT)/%.o,\
               $(filter %.$(EXT),$(SRC))))

OBJ_DIRECTORIES := $(foreach EXT,$(SRC_EXT),\
                       $(OBJ_DIR)/$(EXT)\
                       $(foreach DIR,$(SRC_SUBDIRS),\
                           $(OBJ_DIR)/$(EXT)/$(DIR)))

OUT_ELF := $(BIN_DIR)/$(OUT_FILENAME).elf
OUT     := $(BIN_DIR)/$(OUT_FILENAME).gba

# === TARGETS ===
.PHONY: all run build clean

all: build

run:
	$(EMULATOR) ./$(OUT)

build: $(OUT)

clean:
	@$(RM) $(RMFLAGS) $(BIN_DIR) $(OBJ_DIR)

# generate .gba file
$(OUT): $(OUT_ELF)
	$(OBJCOPY) -O binary $^ $@

# generate .elf file
$(OUT_ELF): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# compile .s files
$(OBJ_DIR)/s/%.o: $(SRC_DIR)/%.s | $(OBJ_DIRECTORIES)
	$(AS) $(ASFLAGS) -o $@ $<

# compile .c files
$(OBJ_DIR)/c/%.o: $(SRC_DIR)/%.c | $(OBJ_DIRECTORIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIRECTORIES):
	$(MKDIR) $(MKDIRFLAGS) "$@"

-include $(OBJ:.o=.d)
