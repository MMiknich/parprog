## Third task make file
CXX= clang++
CXXFLAGS=-lpthread -lm
CXXSANITIZEFLAGS= -fsanitize=address -fno-omit-frame-pointer -lpthread -lm
EXEC= ./
EXECFLAGS=
QFILE= job.sh
compile:
	$(CXX) $(CXXFLAGS) -o perenos.o perenos.cpp
sanitize:
	$(CXX) $(CXXSANITIZEFLAGS) -o perenos.o perenos.cpp
debug: compile
	$(EXEC)$(EXECFLAGS)perenos.o 11 4
test: compile
	sh $(QFILE)
