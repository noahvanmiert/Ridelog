SOURCES = src/main.cpp src/app/app.cpp src/app/workout.cpp 
BIN = bin
INCLUDE = src/

out:
	g++ $(SOURCES) -I$(INCLUDE) -std=c++17 -pedantic -Wall -Wextra -o $(BIN)/main.out

run:
	./$(BIN)/main.out

clean:
	rm -rf $(BIN)/*