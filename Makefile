#Linux:
DEBUG_FLAGS   = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
RELEASE_FLAGS = -O2 
C_FLAGS   	  = $(RELEASE_FLAGS)
COMPILER	  = g++

SRC = src
BIN = bin
OBJ = obj

BIN_FILE = $(BIN)/run

IN_INTERFACE = 595
OUT_INTERFACE = 598
FILTER_RULES_FILE = rules.fr

all: create_dir $(OBJ)/ipv4.o $(OBJ)/filter.o
	$(COMPILER) $(C_FLAGS) $(SRC)/main.cpp $(OBJ)/ipv4.o $(OBJ)/filter.o -o $(BIN_FILE)

run:
	sudo ./$(BIN_FILE) $(IN_INTERFACE) $(OUT_INTERFACE)

$(OBJ)/ipv4.o: $(SRC)/ipv4.cpp $(SRC)/ipv4.h
	$(COMPILER) -c $(C_FLAGS) $(SRC)/ipv4.cpp -o $(OBJ)/ipv4.o

$(OBJ)/filter.o: $(SRC)/filter.cpp $(SRC)/filter.h
	$(COMPILER) -c $(C_FLAGS) $(SRC)/filter.cpp -o $(OBJ)/filter.o

alpine: configure all
	./$(BIN_FILE) $(IN_INTERFACE) $(OUT_INTERFACE) $(FILTER_RULES_FILE)

configure:
	sh scripts/configure_alpine.sh

clean:
	rm -r $(BIN)
	rm -r $(OBJ)

create_dir:
	-@mkdir -p $(BIN)
	-@mkdir -p $(OBJ)

