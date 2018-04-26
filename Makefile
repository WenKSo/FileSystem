# Name: Wenkang Su, Donald Tang
# wsu1, dtang4
# I pledge my honor that I have abided by the Stevens Honor System. 
# Name of your program:
NAME = main

# List of all .cpp source code files included in your program (separated by spaces):
SRC = main.cpp


SRCPATH=./
OBJ=$(addprefix $(SRCPATH), $(SRC:.cpp=.o))

RM=rm -f 
INCPATH=includes
CPPFLAGS += -I $(INCPATH)
CPPFLAGS += -std=c++11


all: $(OBJ)
	g++ $(OBJ) -o $(NAME) -lpthread

clean:
	-$(RM) *~
	-$(RM) *#*
	-$(RM) *swp
	-$(RM) *.core
	-$(RM) *.stackdump
	-$(RM) $(SRCPATH)*.o
	-$(RM) $(SRCPATH)*.obj
	-$(RM) $(SRCPATH)*~
	-$(RM) $(SRCPATH)*#*
	-$(RM) $(SRCPATH)*swp
	-$(RM) $(SRCPATH)*.core
	-$(RM) $(SRCPATH)*.stackdump
	-$(RM) $(NAME)

fclean: clean
	-$(RM) $(NAME)

re: fclean all

