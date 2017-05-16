############################
# Makefile for jobsplitter #
############################

# Source directory and bin directory
BIN    = ~/bin/
SRC    = .

CCFLAG   = -DLINUX 
CC       = /usr/bin/g++
cc       = /usr/bin/gcc

# all
all: 	splitCondor splitPBS

# splitCondor sort
splitCondorOBJ = splitCondor.o

splitCondor:	$(splitCondorOBJ) 
		$(CC) $(splitCondorOBJ) \
		-o $(BIN)/splitCondor

#  spitCondor object
splitCondor.o:	$(SRC)/splitCondor.C
		$(CC) $(CCFLAG)-c $(SRC)/splitCondor.C -o splitCondor.o

# splitPBS sort
splitPBSOBJ = splitPBS.o

splitPBS:	$(splitPBSOBJ) 
		$(CC) $(splitPBSOBJ) \
		-o $(BIN)/splitPBS

#  spitPBS object
splitPBS.o:	$(SRC)/splitPBS.C
		$(CC) $(CCFLAG)-c $(SRC)/splitPBS.C -o splitPBS.o

# Cleaning
clean:		
		rm -f *.o
		rm -f core
		rm -f *%
		rm -f a.out
