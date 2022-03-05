compile:
	@echo -e "\x1b[34m\n=> Compiling...\n\x1b[0m"
	g++ \
		src/parse.cpp src/pre_process.cpp src/text_object.cpp src/address.cpp \
		src/assemble.cpp src/analyze.cpp src/error.cpp main.cpp \
		-o montador -Wall

run:
	@echo -e "\x1b[34m\n=> Running...\n\x1b[0m"
	@echo -e "================================"
	./montador -o code1.asm output.obj

comprun: compile run

