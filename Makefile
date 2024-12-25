NAME	=	ircserv

RED         = "\033[31m"
GREEN       = "\033[32m"
END_COLOR   = "\033[0m"
BOLD        = "\033[1m"

include .env
CXX			=	c++
FLAGS		=	$(STDFLAGS) $(CXXFLAGS) $(ENVFLAGS)
CXXFLAGS	=	-Wall -Wextra -Werror
STDFLAGS	=	-std=c++98
ENVFLAGS	=	-DOPLOGIN=\"$(OPLOGIN)\" -DOPPASS=\"$(OPPASS)\"
INCLUDE		=	-I$(INC_DIR)

RM	=	rm -rf

INC_DIR =	includes
SRC_DIR	=	srcs
OBJ_DIR	=	obj
OBJS 	=	$(addprefix $(OBJ_DIR)/,$(SRCS:.cpp=.o))
SRCS	=	main.cpp \
			Server.cpp \
			User.cpp \
			Commands.cpp \
			Channel.cpp \
			MessageCodes.cpp \

all:	$(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp | $(OBJ_DIR)
				@$(CXX) $(FLAGS) $(INCLUDE) -o $@ -c $<

$(NAME): $(OBJS)
	@$(CXX) $(FLAGS) -o $(NAME) $(OBJS)
	@echo $(GREEN)$(BOLD)$(NAME) $(END_COLOR)$(GREEN)successfully created$(END_COLOR)

clean:
		@$(RM) $(OBJ_DIR)
		@echo $(RED)Object files directory removed $(END_COLOR)

fclean:	clean
		@$(RM) $(NAME)
		@echo $(RED)$(BOLD)$(NAME) $(END_COLOR)$(RED)removed $(END_COLOR)

re:	fclean
	@$(MAKE)  --no-print-directory all
	@echo $(GREEN)Cleaned and rebuild $(BOLD)$(NAME)!$(END_COLOR)

.PHONY: all clean fclean re