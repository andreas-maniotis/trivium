#CXX   := g++ -Wall -pedantic
CXX   := clang++ -Wall -pedantic
#CXX  := nvcc
FLAGS := -std=c++20 -I../../include  -g -O0 -fconstexpr-depth=1024



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
