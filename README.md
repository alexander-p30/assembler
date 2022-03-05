# Identificação

Estevan Alexander de Paula - 170009611

# Como compilar?

O trabalho pode ser compilado a partir da Makefile, rodando ```make compile```, ou manualmente, com:
```bash
g++ \
  src/parse.cpp src/pre_process.cpp src/text_object.cpp src/address.cpp \
  src/assemble.cpp src/analyze.cpp src/error.cpp main.cpp \
  -o montador -Wall
```

# Como rodar?

Seguindo a especificação do trabalho, o programa é rodado no formato a seguir:
```bash
./montador {modo} {arquivo_de_entrada} {arquivo_de_saida}
```

Em que modo pode ser:<br>
  -p: pré-processa diretivas IF e EQU e detecta erros nessas duas diretivas;<br>
  -m: pré-processa macros e detecta erros relacionados a macros e os mesmos erros do modo p;<br>
  -o: monta o arquivo e detecta todos os tipos de erro implementados.<br>

O arquivo de entrada, deve ser um arquivo de texto contendo um código do assembly inventado.

