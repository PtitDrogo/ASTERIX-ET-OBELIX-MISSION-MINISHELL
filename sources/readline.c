/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/22 17:56:24 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// compile this code with cc readline.c -lreadline
int empty_trash(t_garbage_collect *gc);
int	add_to_trash(t_garbage_collect **root, void *to_free);
int	basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char *input);

bool	ft_is_name_lower(t_env_node *new_str, t_env_node *low_str);
int	count_nodes(t_env_node *new_node);
void	sorted_env_print(t_env_node *env_dup_root);
int		export_print(t_env_node *node_to_print);

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


void	sorted_env_print(t_env_node *env_dup_root)
{
	t_env_node	*lowest_name;
	t_env_node	*current;
	t_env_node	*last_node_printed;
	int			n_nodes;
	
	n_nodes = count_nodes(env_dup_root);
	last_node_printed = NULL;
	while (n_nodes)
	{
		current = env_dup_root;
		lowest_name = NULL;
		while (current)
		{
			if (ft_is_name_lower(current, lowest_name) && 
				ft_is_name_lower(last_node_printed, current)) //is lower but higher than last print;
			{	
				lowest_name = current;
			}
			current = current->next;
		}
		export_print(lowest_name);
		n_nodes--;
		last_node_printed = lowest_name;
	}
}

int		export_print(t_env_node *node_to_print)
{
	if (node_to_print == NULL)
		return (0);
	printf("declare -x %s", node_to_print->variable_name);
	if (node_to_print->variable)
		printf("=\"%s\"", node_to_print->variable);
	printf("\n"); //protect printfs later i guess;
	node_to_print  = node_to_print->next;
	return (1);
}

//this function checks if the new node name has lower ascii value
//than new node
bool	ft_is_name_lower(t_env_node *new_node, t_env_node *low_node)
{
	int	i;
	char	*low_str;
	char	*new_str;
	i = 0;
	if (low_node == NULL || new_node == NULL)
		return (true);
	if (new_node == low_node)
		return (false);
	low_str = low_node->variable_name;
	new_str = new_node->variable_name;
	if (low_str == NULL) //in theory these 2 never trigger but just in case
		return (true);
	if (new_str == NULL)
		return (false);
	while (low_str[i] && new_str[i])
	{
		if (new_str[i] < low_str[i])
			return (true);
		else if (new_str[i] > low_str[i])
			return (false);
		i++;
	}
	if (new_str[i] == '\0' && low_str[i])
		return (true);
	return (false);
}

int	count_nodes(t_env_node *new_node)
{
	int	i;

	i = 0;
	while (new_node)
	{
		i++;
		new_node = new_node->next;
	}
	return (i);
}