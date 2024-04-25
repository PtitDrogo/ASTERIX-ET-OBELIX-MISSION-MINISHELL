#---------------------------------Variables---------------------------------#

OBJS_DIR	=	.objs
DEPS_DIR	=	.deps
SRCS_DIR	=	sources
HEADER_DIR	=	includes
LIBFT		=	./libft/libft.a
NAME		=	minishell
OBJS	=	$(patsubst $(SRCS_DIR)%.c, $(OBJS_DIR)%.o, $(SRCS))
DEPS 	= 	$(OBJS:.o=.d)
#------------------------------------------------------------------------#

#---------------------------------Sources---------------------------------#
SRCS =		$(SRCS_DIR)/builtins/export.c \
			$(SRCS_DIR)/garbage_collector.c \
			$(SRCS_DIR)/builtins/unset.c \
			$(SRCS_DIR)/utils.c \
			$(SRCS_DIR)/builtins/env.c \
			$(SRCS_DIR)/builtins/exit.c \
			$(SRCS_DIR)/readline.c \
      		$(SRCS_DIR)/parser.c \
			$(SRCS_DIR)/lexer.c \
			$(SRCS_DIR)/builtins/export_utils.c \
			$(SRCS_DIR)/expander.c \
			$(SRCS_DIR)/errors.c \
			$(SRCS_DIR)/builtins/echo.c \
			$(SRCS_DIR)/builtins/cd.c \
			$(SRCS_DIR)/builtins/pwd.c \
			$(SRCS_DIR)/builtins/ls.c \
			$(SRCS_DIR)/here_doc.c 
#------------------------------------------------------------------------#

#---------------------------------Compilation & Linking---------------------------------#
CC		=	cc
RM		=	rm -f

CFLAGS	=	-g3 
LINKLIBS = -L libft/ -lft 
INCLUDES = -I $(HEADER_DIR) -I libft -MMD -MP

#------------------------------------------------------------------------#


#---------------------------------Pretty---------------------------------#
YELLOW	=	\033[1;33m
GREEN	=	\033[1;32m
RESET	=	\033[0m
UP		=	"\033[A"
CUT		=	"\033[K"
#------------------------------------------------------------------------#


all: $(NAME)
-include $(DEPS)

$(LIBFT): FORCE
	@$(MAKE) --no-print-directory -C ./libft

FORCE:

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(@D)
	@echo "$(YELLOW)Compiling [$<]$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 
	@printf $(UP)$(CUT)

$(NAME): $(OBJS) $(LIBFT) Makefile
	@echo "$(YELLOW)Compiling [$<]$(RESET)"
	@$(CC) $(OBJS) $(CFLAGS) -lreadline $(INCLUDES) $(LINKLIBS) -o $@ 
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) compiled!$(RESET)"

clean:
	@echo "$(YELLOW)cleaning files$(RESET)"
	@$(RM) $(OBJS) rm -rf $(OBJS_DIR)
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) files deleted !$(RESET)"
	@$(MAKE) --no-print-directory -C ./libft clean

fclean:	clean
	@echo "$(YELLOW)cleaning files$(RESET)"
	@$(RM) $(NAME)
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) executable deleted !$(RESET)"
	@$(RM) $(LIBFT)
	@echo "$(GREEN)libft.a executable deleted !$(RESET)"

re:	fclean $(NAME)

.PHONY:	all clean fclean re bonus FORCE
