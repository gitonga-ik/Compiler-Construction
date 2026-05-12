scanner: main.o scanner.o
	g++ $^ -o $@

%.o: %.cpp
	g++ -c $< -o $@

%.cpp: %.l
	flex -i -o $@ $<

clean:
	rm -f *.o scanner scanner.cpp recursive ll1 partial parser icg

recursive:
	g++ recursive_parser.cpp -o recursive

# ll1:
# 	g++ ll1_parser.cpp parse_table.cpp terminals.cpp production_rules.cpp -o ll1

# parse: clean scanner ll1
# 	./scanner input.txt ; ./ll1 tokens.out

# partial:
# 	g++ partial_tree.cpp parse_table.cpp terminals.cpp production_rules.cpp -o partial

# parse_partial: clean scanner partial
# 	./scanner input.txt ; ./partial tokens.out

icg:
	g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -o icg ll1_parser.cpp parse_table.cpp production_rules.cpp terminals.cpp ast.cpp ast_builder.cpp icg.cpp

ir: clean scanner icg
	./scanner input.txt ; ./icg tokens.out
	
all: clean scanner icg ir