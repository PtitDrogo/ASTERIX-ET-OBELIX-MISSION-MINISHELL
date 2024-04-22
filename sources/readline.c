/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/22 19:45:18 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// compile this code with cc readline.c -lreadline
int empty_trash(t_garbage_collect *gc);
int	add_to_trash(t_garbage_collect **root, void *to_free);
int	basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char *input);



int main(int argc, char const *argv[], char **envp) 
{
	t_env_node *env_dup_root;
	t_garbage_collect *gc;
	char	*input;

	
	gc = NULL;
	env_dup_root = NULL;
	if (envp == NULL)
		return (1);
	if (generate_env_llist(&env_dup_root, &gc, envp) == 0)
	{
		write(2, "failed to generate env\n", 23);
		empty_trash(gc);
		return (1); // this will be its own function later
	}
	while (1) 
	{
		input = readline("myshell> ");
		if (add_to_trash(&gc, input) == 0)
			exit(EXIT_FAILURE); // do cleanup later
		if (!input)
			break;
		basic_parsing(&env_dup_root, &gc, input);
		// Check for EOF (Ctrl+D)

		// Skip empty commands
		if (*input) 
			add_history(input);
	}
	
	printf("Exit.\n");
	rl_clear_history();
	empty_trash(gc);
	return 0;
}

//This is just basic parsing to be able to test my builtins
int	basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char *input)
{
	char	**split_input;

	if (input == NULL || input[0] == '\0')
		return (1);
	split_input = ft_split(input, ' ');

	setter_double_p_gc((void **)split_input, gc);
	parse(split_input, gc);
	/*if (ft_strncmp(split_input[0], "unset", ft_strlen("unset")) == 0)
		unset(*env_dup_root, split_input[1]);
	if (ft_strncmp(split_input[0], "export", ft_strlen("export")) == 0)
	{	
		if (split_input[1] == NULL)
			sorted_env_print(*env_dup_root);
		else
		{
			export(env_dup_root, (void *)split_input[1], gc);
		}
	}
	if (ft_strncmp(split_input[0], "env", ft_strlen("env")) == 0)
		env(*env_dup_root);*/
	//ft_free_array((void **)split_input); broken af, I need what's inside even after
	return (0);
}



