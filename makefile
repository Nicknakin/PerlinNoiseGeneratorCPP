CXX = g++
#oglflags = -lsfml-graphics -lsfml-window -lsfml-system -lpthread

default: main.o 
	$(CXX) -o defaultOut main.cpp 

debug: 
	$(CXX) -g -Wall -o debugOut main.cpp 

%.o: %.cpp %.h
	$(CXX) -c $< $(oglflags)

clean:
	rm -rf *.o ./defaultOut ./debugOut *.gch
