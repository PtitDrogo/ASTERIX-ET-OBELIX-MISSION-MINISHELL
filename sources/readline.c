/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/26 20:59:41 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// compile this code with cc readline.c -lreadline
int 	empty_trash(t_garbage_collect *gc);
int		add_to_trash(t_garbage_collect **root, void *to_free);
int		basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char *input);
int		theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char *input);
char	*accurate_shell(t_garbage_collect **gc, t_env_node *env);


int main(int argc, char const *argv[], char **envp)
{
	t_env_node *env_dup_root;
	t_garbage_collect *gc;
	char	*input;
	char	*history;
	
	gc = NULL;
	env_dup_root = NULL;
	if (envp == NULL)
		return (1);
	if (generate_env_llist(&env_dup_root, &gc, envp) == 0) // in theory this cant happen since it exits if it fails;
	{
		ft_printf_err("failed to generate env\n");
		empty_trash_exit(gc, 1);
	}
	while (1) 
	{
		input = readline("myshell> ");	
		if (add_to_trash(&gc, input) == 0)
			exit(EXIT_FAILURE); // do cleanup later
		if (!input)
			break;
		// Check for EOF (Ctrl+D)
		
		// basic_parsing(&env_dup_root, &gc, input);
		theo_basic_parsing(&env_dup_root, &gc, input); //comment this out 
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

//This is just basic parsing to be able to test my builtins
int	theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char *input)
{
	char	**split_input;
	
	if (input == NULL || input[0] == '\0')
		return (1);
	split_input = (char **)setter_double_p_gc((void **)ft_split(input, ' '), gc);
	//this is temp so i dont check if its null
	if (ft_strcmp(split_input[0], "unset") == 0)
		unset(*env_dup_root, split_input[1]);
	if (ft_strcmp(split_input[0], "export") == 0)
	{	
		if (split_input[1] == NULL)
			sorted_env_print(*env_dup_root, *gc);
		else
			export(env_dup_root, (void *)split_input[1], gc);
	}
	if (ft_strcmp(split_input[0], "env") == 0)
		env(*env_dup_root, *gc);
	if (ft_strcmp(split_input[0], "exit") == 0)
		ft_exit(&split_input[1], *gc);
	if (ft_strcmp(split_input[0], "pwd") == 0)
		pwd(gc);
	if (ft_strcmp(split_input[0], "cd") == 0)
		cd(split_input[1], gc, *env_dup_root);
	if (ft_strcmp(split_input[0], "<<") == 0)
		here_doc(split_input[1] ,gc, 1);
	// if (ft_strcmp(split_input[0], "echo") == 0)
	// 	echo(split_input[1], *gc); //this will only print the first arg for now;
	
	return (0);
}



//Ignore ca c'est une experience mais remplacer le home par ~ c'est chiant
char	*prompt(t_garbage_collect **gc, t_env_node *env)
{
	char *pwd;

	pwd = accurate_shell(gc, env);
	

	return (NULL);
}

char	*accurate_shell(t_garbage_collect **gc, t_env_node *env)
{
	t_env_node *pwd;
	char		*backup_pwd;
	
	pwd = get_env_node(env, "PWD");
 	if (pwd != NULL)
		return (pwd->variable);
	else
	{
		backup_pwd = getcwd(NULL, 0);
		if (backup_pwd == NULL)
			perror_exit(*gc, errno, "Failed to get current path");
		setter_gc(backup_pwd, gc);
		return (backup_pwd);
	}
}
///FIN EXPERIENCE


