CC = g++
CXXFLAGS = -std=c++11 -g -Wall

MAIN = main

all:
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp

clean:
	@rm -rf $(MAIN)

commit:
	git add .
	git commit -m "$$ARGS"
	git push
