## Second task make file
CXX= mpicxx
CXXFLAGS= 
CXXSANITIZEFLAGS= -fsanitize=address -fno-omit-frame-pointer
EXEC= mpirun
EXECFLAGS= -np
QFILE= job.sh
compile:
	$(CXX) $(CXXFLAGS) -o temptask2 Term.cpp
sanitize:
	$(CXX) $(CXXSANITIZEFLAGS) -o temptask2 Term.cpp
debug: compile
	$(EXEC) $(EXECFLAGS) 4 temptask2 1 0 1 0.02 0.0002 0.1 1
test: compile
	qsub $(QFILE)
