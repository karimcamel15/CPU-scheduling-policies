CC = g++
CFLAGS = -std=c++17 -Wall -Wextra
SRC = lab6.cpp
OBJ = lab6.o
EXEC = lab6

# Test case groups
TESTS_01 = 01a-input.txt 01b-input.txt 01c-input.txt 01d-input.txt
TESTS_02 = 02a-input.txt 02b-input.txt 02c-input.txt 02d-input.txt
TESTS_03 = 03a-input.txt 03b-input.txt 03c-input.txt 03d-input.txt
TESTS_04 = 04a-input.txt 04b-input.txt 04c-input.txt 04d-input.txt
TESTS_05 = 05a-input.txt 05b-input.txt 05c-input.txt 05d-input.txt
TESTS_06 = 06a-input.txt 06b-input.txt 06c-input.txt 06d-input.txt
TESTS_07 = 07a-input.txt 07b-input.txt 07c-input.txt 07d-input.txt
TESTS_08 = 08a-input.txt 08b-input.txt
TESTS_09 = 09a-input.txt 09b-input.txt
TESTS_10 = 10a-input.txt 10b-input.txt
TESTS_11 = 11a-input.txt
TESTS_12 = 12a-input.txt

# All test cases
TESTS = $(TESTS_01) $(TESTS_02) $(TESTS_03) $(TESTS_04) $(TESTS_05) \
        $(TESTS_06) $(TESTS_07) $(TESTS_08) $(TESTS_09) $(TESTS_10) \
        $(TESTS_11) $(TESTS_12)

# Targets
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

# Clean before running all tests
test_all: clean $(EXEC)
	@for test in $(TESTS); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 1
test1: clean $(EXEC)
	@for test in $(TESTS_01); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 2
test2: clean $(EXEC)
	@for test in $(TESTS_02); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 3
test3: clean $(EXEC)
	@for test in $(TESTS_03); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 4
test4: clean $(EXEC)
	@for test in $(TESTS_04); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 5
test5: clean $(EXEC)
	@for test in $(TESTS_05); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 6
test6: clean $(EXEC)
	@for test in $(TESTS_06); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 7
test7: clean $(EXEC)
	@for test in $(TESTS_07); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 8
test8: clean $(EXEC)
	@for test in $(TESTS_08); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 9
test9: clean $(EXEC)
	@for test in $(TESTS_09); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 10
test10: clean $(EXEC)
	@for test in $(TESTS_10); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 11
test11: clean $(EXEC)
	@for test in $(TESTS_11); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean before running test group 12
test12: clean $(EXEC)
	@for test in $(TESTS_12); do \
		echo "Running test: $$test"; \
		cat $$test | ./$(EXEC); \
		echo ""; \
	done

# Clean up
clean:
	rm -f $(OBJ) $(EXEC)


