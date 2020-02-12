CXX = g++ -O3
#oglflags = -lsfml-graphics -lsfml-window -lsfml-system -lpthread

default: perlin.o main.o 
	$(CXX) -o defaultOut main.o perlin.o 

debug: 
	$(CXX) -g -Wall -o debugOut main.cpp perlin.cpp 

%.o: %.cpp %.h
	$(CXX) -c $< $(oglflags)

clean:
	rm -rf *.o ./defaultOut ./debugOut *.gch
