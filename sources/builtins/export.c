/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 16:11:59 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/26 20:28:56 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			unset(t_env_node *env_dup_root, char *env_to_find);
int			export(t_env_node **root, void *variable, t_garbage_collect **gc);
int			pop(t_env_node *env_dup_root, t_env_node *node_to_pop);
t_env_node *check_if_variable_exist(t_env_node *root, void *variable);
int			is_valid_env_name(char *name, t_garbage_collect *gc);
char		*get_env_name(const char *src, t_garbage_collect **gc);
char		*get_env_var(const char *src, t_garbage_collect **gc);


//TODO make this universal later
int	pop(t_env_node *env_dup_root, t_env_node *node_to_pop)
{	
	if (!env_dup_root || !node_to_pop)
		return (0); //gotta check later;
	while (env_dup_root->next && env_dup_root->next != node_to_pop)	
		env_dup_root = env_dup_root->next;
	if (env_dup_root->next == NULL)
		return (0); // we couldnt find the node to pop
	env_dup_root->next = env_dup_root->next->next;
	node_to_pop->variable = NULL;
	return (1);
}

int	export(t_env_node **root, void *variable, t_garbage_collect **gc)
{
	t_env_node	*new_node;
	t_env_node	*current;
	t_env_node	*same_name_node;
	
	if (is_valid_env_name(variable, *gc) == 0)
		return (2); //different error that SHOULDNT terminate the shell
	same_name_node = check_if_variable_exist(*root, variable);
	if (same_name_node)
	{
		same_name_node->variable_name = get_env_name(variable, gc);
		same_name_node->variable = get_env_var(variable, gc);
		return (1);
	}
	new_node = malloc_trash(sizeof(t_env_node), gc);
	if (!new_node)
		return (0);
	new_node->next = NULL;
	new_node->variable_name = get_env_name(variable, gc);
	new_node->variable = get_env_var(variable, gc);
	if ((*root) == NULL)
	{
		*root = new_node;
		return (1);
	}
	current = *root;
	while (current->next)
		current = current->next;
	current->next = new_node;
	return (1);
}

//this function checks if the env var name is valid;
int	is_valid_env_name(char *name, t_garbage_collect *gc)
{
	int	i;
	
	if (ft_isalpha(name[0]) == 0 && name[0] != '_')
	{	
		if (ft_printf_err("bash: export: `%s': not a valid identifier\n", name) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (0);
	}
	i = 1; // we start after the first letter;
	while (name[i] && name[i] != '=')
	{
		if (ft_isalnum(name[i]) == 0 && name[i] != '_')
		{	
			if (ft_printf_err("bash: export: `%s': not a valid identifier\n", name) == -1)
				perror_exit(gc, errno, WRITE_ERR_MSG);
			return (0);
		}
		i++;
	}
	return (1);
}

//this function checks if the variable exists already, if it does, it returns the node with the
//variable, otherwise it returns NULL
t_env_node *check_if_variable_exist(t_env_node *root, void *variable)
{
	size_t var_len;
	
	if (variable == NULL)
		return (NULL);
	if (is_char_in_str(variable, '=') == 0)
		return (NULL);
	var_len = len_to_char(variable, '=');
	while (root)
	{
		if (len_to_char(root->variable_name, '=') == var_len)
		{
			if (ft_strncmp(root->variable_name, variable, var_len) == 0)
				return (root);
		}
		root = root->next;
	}
	return (NULL);
}

char	*get_env_name(const char *src, t_garbage_collect **gc)
{
	int		i;
	int		j;
	char	*dest;

	i = 0;
	while (src[i] != '\0' && src[i] != '=')
		i++;
	dest = malloc_trash(sizeof(char) * i + 1, gc);
	if (!dest)
		return (NULL);
	dest[i] = '\0';
	j = i;
	i = 0;
	while (i < j)
	{
		dest[i] = src[i];
		i++;
	}
	return (dest);
}

char	*get_env_var(const char *src, t_garbage_collect **gc)
{
	int i;
	char	*to_return;
	i = 0;
	while (src[i] != '\0' && src[i] != '=')
		i++;
	if (src[i] == '\0')
		return (NULL);//Could be return empty malloc(but risk of false positive), would also make it different than failed malloc
	i++; // to go past the '='
	to_return = setter_gc(ft_strdup(&src[i]), gc);
	malloc_check(to_return, *gc);
	return (to_return);
}

int	generate_env_llist(t_env_node **env_dup_root, t_garbage_collect **gc, char **envp)
{
	int i;

	i = -1;
	while (envp[++i])
	{	
		if (export(env_dup_root, (void *)envp[i], gc) == 0)
			return (0);
	}
	return (1);
}
