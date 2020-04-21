## Third task make file
CXX= clang
CXXFLAGS=-lpthread -lm
CXXSANITIZEFLAGS= -fsanitize=address -fno-omit-frame-pointer -lpthread -lm
EXEC= ./
EXECFLAGS=
QFILE= job.sh
compile:
	$(CXX) $(CXXFLAGS) -o MK.o MK.c
sanitize:
	$(CXX) $(CXXSANITIZEFLAGS) -o MK.o MK.c
debug: compile
	$(EXEC)$(EXECFLAGS)MK.o 10 9
test: compile
	qsub $(QFILE)
