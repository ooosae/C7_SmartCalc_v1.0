# Компилятор

CC := gcc

# Опции компилятора

CFLAGS := -std=c11 -Wall -Werror -pedantic -Wextra -Wfloat-equal -Wfloat-conversion -Wvla 
UNITFLAGS := -lcheck
COV_FLAG :=
OS:=$(shell uname -s)
ifeq ($(OS), Darwin)
	UNITFLAGS += $(shell pkg-config --cflags --libs check)
else
	UNITFLAGS += -lsubunit -lrt -lm -lpthread
endif

INSTALL_DIR := ./application

# Объектные файлы

OBJS := ./out/bonus_part.o ./out/expression_calculation.o ./out/expression_parsing.o ./out/graph.o ./out/stack.o ./out/smart_calc.o
UNITOBJS := ./out/tests.o

.PHONY : all clean gcov_report del_obj dvi install uninstall

all : clean install

test : unit_tests.exe
	./unit_tests.exe || echo

valgrind: clean unit_tests.exe
	valgrind --tool=memcheck --leak-check=yes --log-file=valgrind_out.txt --leak-check=full -s ./unit_tests.exe || echo

smart_calc.exe: $(OBJS) ./out/main.o
	$(CC) -o $@  $^ -lm `pkg-config --libs gtk+-3.0`

unit_tests.exe : $(OBJS) $(UNITOBJS)
	$(CC) -o $@ $^ $(UNITFLAGS) -lm -fprofile-arcs -ftest-coverage

gcov_report : COV_FLAG += --coverage
gcov_report : del_obj test
	lcov -t "coverage" -o coverage.info -c -d . 
	genhtml -o report coverage.info
	rm -rf ./*.gc*
	rm -rf ./*.gd*
	open ./report/index.html

del_obj :
	rm -rf ./out
	
install : smart_calc.exe
	mkdir -p $(INSTALL_DIR)
	cp $^ $(INSTALL_DIR)
	rm -rf $^

uninstall :
	rm -rf $(INSTALL_DIR)
	
s21_math.a : ./out/s21_math.o
	ar rc $@ $^
	ranlib $@
	cp $@ lib$@

./out/main.o: ./main.c
	mkdir -p ./out
	$(CC) -std=c11 `pkg-config gtk+-3.0 --cflags` -c $< -o $@


./out/%.o: ./%.c ./%.h
	mkdir -p ./out
	$(CC) $(CFLAGS) $(COV_FLAG) -c $< -o $@
	
./out/%.o : ./my.check
	checkmk my.check > tests.c
	mkdir -p ./out
	$(CC) $(CFLAGS) $(UNITFLAGS) -c tests.c
	mv ./*.o ./out/

dvi:
	open ./readme.html

dist: clean
	zip -r SmartCalc_v1.0.zip *

clean :
	rm -f ./*.exe
	rm -rf ./out
	rm -rf ./*.a
	rm -rf ./*.gc*
	rm -rf ./*.gd*
	rm -rf ./report
	rm -rf ./*.info
	rm -rf tests.c
	rm -rf valgrind_out.txt

