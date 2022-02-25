compile:
	@echo -e "\x1b[34m\n=> Compiling...\n\x1b[0m"
	g++ include/parse.hpp include/pre_process.hpp src/parse.cpp src/pre_process.cpp src/text_object.cpp src/address.cpp src/assemble.cpp src/analyze.cpp src/inspect.cpp src/error.cpp test/test.cpp

run:
	@echo -e "\x1b[34m\n=> Running...\n\x1b[0m"
	@echo -e "================================"
	./a.out

comprun: compile run

