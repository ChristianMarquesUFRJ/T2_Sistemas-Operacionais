###########################################################################
#  Universidade Federal do Rio de Janeiro
#  Disciplina: Sistemas Operacionais - EEL770
#  Professor: Rodrigo Souza Couto
#  Desenvolvedor: Chritian Marques de Oliveira Silva
#  DRE: 117.214.742
#  Trabalho 2: Locks e Variáveis de Condição
###########################################################################

# Compilador e linkeditor
CC = g++
LD = g++

# Flags de compilação e linkedição
CFLAGS = -Wall -std=c++11
LFLAGS = -Wall -std=c++11 -pthread

# Flags do objeto e executável a serem gerados
OBJ = main.o
EXEC = run

#Implicit Rules
.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: $(EXEC)

$(EXEC): $(OBJ)
	$(LD) $(OBJ) -o $(EXEC) $(LFLAGS)
	make -s clean-objs

clean:
	rm -f $(OBJ) $(EXEC)

clean-execs:
	rm -f $(EXEC)

clean-objs:
	rm -f $(OBJ)