# Flags y compilador
CC := g++
# CCFLAG := -std=c++17 -Wextra -g -Wall -Werror -Wconversion -Wpedantic -Wunused -Wcast-align -Wshadow -Wold-style-cast -Wpointer-arith -Wcast-qual -Wno-missing-braces
CCFLAG := -w -std=c++2a -Wall -Werror -Wconversion

CCOBJFLAG := $(CCFLAG) -c
SQL := -lmysqlclient
GPIO := -llgpio
FS := -lstdc++fs
CURL := -lcurl
GTK := `pkg-config --cflags --libs gtk+-3.0`
DYN := -export-dynamic -rdynamic -lm
WPI := -lwiringPi
# Directorios
BIN_PATH := build
OBJ_PATH := obj
SRC_PATH := src
LIB_PATH := lib

# Nombre de todos los fuentes binarios, con directorio incluido (ej: src/timer.cc)
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))

# Nombre de todos los binarios (ej: timer)
TARGETS := $(basename $(notdir $(SRC)))

# Nombre de todos las librerias, con directorio incluido (ej: common.cc)
LIB := $(foreach x, $(LIB_PATH), $(wildcard $(addprefix $(x)/*,.c*)))

# Nombre de todos los objetos, con directorio incluido (ej: obj/common.o)
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(LIB)))))
.SECONDARY: $(OBJ)

# Regla por default: compilar todos los binarios
all: $(TARGETS) 

# Regla para compilar objetos
$(OBJ_PATH)/%.o: $(LIB_PATH)/%.c*
	@echo "Compilando $@"
	@$(CC) $(CCOBJFLAG) -o $@ $< $(GTK) $(DYN)
	
# Reglas para compilar y linkear los binarios
$(TARGETS): $(OBJ)
	@echo "Compilando y linkeando $(BIN_PATH)/_$@"
	@$(CC) -o $(BIN_PATH)/_$@ $(CCFLAG) $(SRC_PATH)/$@.c* $(OBJ) $(SQL) $(GPIO) $(FS) $(CURL) $(WPI) $(GTK) $(DYN)

# gui: $(OBJ_PATH)/user_view.o $(OBJ_PATH)/main_view.o
# 	@echo "Compilando GUI"

# Borrado de objetos y binarios
clean:
	@echo "Borrando binarios y objetos"
	@rm -f $(BIN_PATH)/* $(OBJ_PATH)/*





