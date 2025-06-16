#CXX   := g++ -Wall -pedantic
#CXX    := /opt/gcc/13/bin/g++
CXX   := clang++ -Wall -pedantic
FLAGS := -std=c++20 -I../../include  -g -O0



all:  $(patsubst %.cpp,  bin/%,  $(wildcard *.cpp)) ;



./bin/%:  %.cpp
	-mkdir -p ./bin/dep
	$(CXX) $(FLAGS) -o $@ $< -MMD -MP
	mv $@.d ./bin/dep



./bin/%: ./bin/dep/%.d



-include $(patsubst %.cpp,  ./bin/dep/%.d,  $(wildcard *.cpp))



.PHONY:
clean:
	-rm -rf ./bin
