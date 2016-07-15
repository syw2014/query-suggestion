
PUB_DIR:=../include/
SRC_INC:= ./

#PROG = t_normalize_unit
PROG = t_dictionary_unit
CFLAGS = -W -Wall -I../ 
SOURCES = $(PROG).cc 

CC=g++
all: $(PROG) 

LDFLAGS=-lboost_system -lboost_serialization -lboost_filesystem -lboost_unit_test_framework
$(PROG): $(SOURCES) 
	$(CC) -o $(PROG) $(SOURCES) $(CFLAGS) $(LDFLAGS) -I $(SRC_INC) -I $(PUB_DIR)




clean:
	rm -rf $(PROG) *.exe *.dSYM *.obj *.exp .*o *.lib
