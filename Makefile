#________________________________________________________________
# VARIABLES
#
NAME = btree
ASG_NICKNAME = HW05

# FILENAMES
EXECUTABLE = test_$(NAME)
SRC_C      = $(NAME).c btree_print.c btree_assert.c btree_wrappers.c btree_insert.c btree_query.c btree_delete.c test_btree.c
SRC_H      = $(NAME).h

# SYSTEM
SHELL  = /bin/bash
CC     = gcc
CFLAGS = -g -std=c11 -Wall -Wshadow -Wvla -pedantic -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable

# SYSTEM - for gcov (coverage testing)
EXECUTABLE_GCOV = $(EXECUTABLE)_gcov
CFLAGS_GCOV = $(CFLAGS) -fprofile-arcs -ftest-coverage

#________________________________________________________________
# RULES
#
$(EXECUTABLE): $(SRC_C) $(SRC_H)
	$(CC) $(CFLAGS) $(SRC_C) -o $(EXECUTABLE) 

test:  $(EXECUTABLE)
	./$(EXECUTABLE)

valgrind:  $(EXECUTABLE)
	valgrind --leak-check=full --track-origins=yes ./$(EXECUTABLE)

gdb:  $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

clean:
	rm -v -f $(EXECUTABLE) $(EXECUTABLE_GCOV) *.c.gcov *.gcda *.gcno

coverage: $(SRC_C) $(TEST_C)
	$(CC) $(CFLAGS_GCOV) $(SRC_C) $(TEST_C) -o $(EXECUTABLE_GCOV)
	./$(EXECUTABLE_GCOV)
	gcov -f $(SRC_C)

submit: 
	368submit $(ASG_NICKNAME) *.*

.PHONY: test valgrind submit clean coverage gdb


# Updated 20231115-161829:  Added SRC_H to compilation.  Submit *.* and Makefile. --ajq

# vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab ft=make:
