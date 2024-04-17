OBJS_DIR	=	.objs
SRCS_DIR	=	sources
HEADER_DIR	=	includes
HEADER_FILE =	libft.h 

SRCS	=	$(SRCS_DIR)/ft_isalnum.c \
			$(SRCS_DIR)/ft_isprint.c \
			$(SRCS_DIR)/ft_memcmp.c \
			$(SRCS_DIR)/ft_putchar_fd.c \
			$(SRCS_DIR)/ft_split.c \
			$(SRCS_DIR)/ft_strlcat.c \
			$(SRCS_DIR)/ft_strncmp.c \
			$(SRCS_DIR)/ft_substr.c \
			$(SRCS_DIR)/ft_atoi.c \
			$(SRCS_DIR)/ft_isalpha.c \
			$(SRCS_DIR)/ft_itoa.c \
			$(SRCS_DIR)/ft_putendl_fd.c \
			$(SRCS_DIR)/ft_strchr.c \
			$(SRCS_DIR)/ft_strlcpy.c \
			$(SRCS_DIR)/ft_strnstr.c \
			$(SRCS_DIR)/ft_tolower.c \
			$(SRCS_DIR)/ft_bzero.c \
			$(SRCS_DIR)/ft_isascii.c \
			$(SRCS_DIR)/ft_memcpy.c \
			$(SRCS_DIR)/ft_memmove.c \
			$(SRCS_DIR)/ft_putnbr_fd.c \
			$(SRCS_DIR)/ft_strdup.c \
			$(SRCS_DIR)/ft_strlen.c \
			$(SRCS_DIR)/ft_strrchr.c \
			$(SRCS_DIR)/ft_toupper.c \
			$(SRCS_DIR)/ft_calloc.c \
			$(SRCS_DIR)/ft_isdigit.c \
			$(SRCS_DIR)/ft_memchr.c \
			$(SRCS_DIR)/ft_memset.c \
			$(SRCS_DIR)/ft_putstr_fd.c \
			$(SRCS_DIR)/ft_strjoin.c \
			$(SRCS_DIR)/ft_strmapi.c \
			$(SRCS_DIR)/ft_strtrim.c \
			$(SRCS_DIR)/ft_striteri.c \
			$(SRCS_DIR)/ft_lstnew_bonus.c \
			$(SRCS_DIR)/ft_lstadd_front_bonus.c \
			$(SRCS_DIR)/ft_lstsize_bonus.c \
			$(SRCS_DIR)/ft_lstlast_bonus.c \
			$(SRCS_DIR)/ft_lstadd_back_bonus.c \
			$(SRCS_DIR)/ft_lstdelone_bonus.c \
			$(SRCS_DIR)/ft_lstclear_bonus.c \
			$(SRCS_DIR)/ft_lstiter_bonus.c \
			$(SRCS_DIR)/ft_lstmap_bonus.c \
			$(SRCS_DIR)/get_next_line.c \
			$(SRCS_DIR)/get_next_line_utils.c \
			$(SRCS_DIR)/ft_strjoin_free_s1.c \
			$(SRCS_DIR)/ft_free_array.c \
			$(SRCS_DIR)/ft_printf.c \
			$(SRCS_DIR)/ft_putncount_str.c \
			$(SRCS_DIR)/ft_putunsign.c \
			$(SRCS_DIR)/ft_putncount_char.c \
			$(SRCS_DIR)/ft_putnbr.c \
			$(SRCS_DIR)/ft_print_ptr.c


OBJS			=	$(patsubst $(SRCS_DIR)%.c, $(OBJS_DIR)%.o, $(SRCS))

CC	=	cc
RM	=	rm -f
CFLAGS	=	-Wall -Wextra -Werror -I.

NAME	=	libft.a

YELLOW	=	\033[1;33m
GREEN	=	\033[1;32m
RESET	=	\033[0m
UP		=	"\033[A"
CUT		=	"\033[K"

all:	$(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c $(HEADER_DIR)/$(HEADER_FILE)
	@mkdir -p $(@D)
	@echo "$(YELLOW)Compiling [$<]$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(HEADER_DIR)
	@printf $(UP)$(CUT)

$(NAME):	$(OBJS)
	@echo "$(YELLOW)Compiling [$<]$(RESET)"
	@ar rcs $(NAME) $^
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) compiled!$(RESET)"

clean:
	@echo "$(YELLOW)cleaning files$(RESET)"
	@$(RM) $(OBJS) rm -rf $(OBJS_DIR)
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) files deleted !$(RESET)"


fclean:	clean
	@echo "$(YELLOW)cleaning files$(RESET)"
	@$(RM) $(NAME)
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) executable deleted !$(RESET)"

re:	fclean $(NAME)

.PHONY:	all clean fclean re
