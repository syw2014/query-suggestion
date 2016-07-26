
PUB_DIR:=../include/
SRC_INC:= ./

PROG1 = t_normalize_unit
PROG2 = t_dictionary_unit
PROG3 = t_segment_unit
PROG4 = t_build_unit
PROG5 = t_suggestion_unit

CFLAGS = -W -Wall -I../ 

CC=g++
#all: $(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG5)
all: $(PROG5)

LDFLAGS=-lboost_system -lboost_serialization -lboost_filesystem -lboost_unit_test_framework


# normalize unit test
#$(PROG1): $(PROG1).cc
#	$(CC) -g -o $(PROG1) $(PROG1).cc $(CFLAGS) $(LDFLAGS) -I $(SRC_INC) -I $(PUB_DIR)

# dictionary unit test
#$(PROG2): $(PROG2).cc
#	$(CC) -g -o $(PROG2) $(PROG2).cc $(CFLAGS) $(LDFLAGS) -I $(SRC_INC) -I $(PUB_DIR)

# segment unit test
#$(PROG3): $(PROG3).cc
#	$(CC) -g -o $(PROG3) $(PROG3).cc $(CFLAGS) $(LDFLAGS) -I $(SRC_INC) -I $(PUB_DIR)

# build engine unit test
#$(PROG4): $(PROG4).cc
#	$(CC) -g -o $(PROG4) $(PROG4).cc $(CFLAGS) $(LDFLAGS) -I $(SRC_INC) -I $(PUB_DIR)

# suggestion unit test
$(PROG5): $(PROG5).cc
	$(CC) -g -o $(PROG5) $(PROG5).cc $(CFLAGS) $(LDFLAGS) -I $(SRC_INC) -I $(PUB_DIR)

run:
	./$(PROG1)
	./$(PROG2)
	./$(PROG3)
	./$(PROG4)
	./$(PROG5)

clean:
	rm -rf $(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG5) *.exe *.dSYM *.obj *.exp .*o *.lib .*.txt
