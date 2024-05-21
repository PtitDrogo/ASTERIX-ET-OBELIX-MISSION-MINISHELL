/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/21 12:22:55 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// compile this code with cc readline.c -lreadline
int 	empty_trash(t_garbage_collect *gc);
int		add_to_trash(t_garbage_collect **root, void *to_free);
int		basic_parsing(t_garbage_collect **gc, char *input, t_token **token, t_cmd **cmds);
int		theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **input);
char	*accurate_shell(t_garbage_collect **gc, t_env_node *env);


int main(int argc, char const *argv[], char **envp)
{
	t_env_node			*env_dup_root;
	t_garbage_collect	*gc;
	t_token				*token;
	t_cmd				*cmds;
	char				*input;
	char				*history;
	int					**pipes;

	token = NULL;
	cmds = NULL;
	gc = NULL;
	env_dup_root = NULL;
	// ft_memset(token, 0, sizeof(t_token));
	// ft_memset(cmds, 0, sizeof(t_cmd));
	// ft_memset(gc, 0, sizeof(t_garbage_collect));
	// ft_memset(env_dup_root, 0, sizeof(t_env_node));
	//foutre ca aux bons endroits
	// printf("launched shell\n");
	if (envp == NULL)
		return (1);
	generate_env_llist(&env_dup_root, &gc, envp);
	while (1) 
	{
		input = readline(NULL);	
		if (add_to_trash(&gc, input) == 0)
			empty_trash_exit(gc, MALLOC_ERROR);
		if (!input)
			break;
		// Check for EOF (Ctrl+D)
		
		// if ( basic_parsing(&gc, input, &token, &cmds) == 0);
		// printf("after basic parsing first cmd is %p\n", cmds);
		
		if (basic_parsing(&gc, input, &token, &cmds) && token)
		{
			expander(env_dup_root, &gc, cmds);
			int number_of_pipes = count_pipes(token);
			pipes = open_pipes(cmds, &gc, number_of_pipes);
			if (number_of_pipes == 0 && is_builtin(cmds->str))
				theo_basic_parsing(&env_dup_root, &gc, cmds->str);
			else
			{	
				int status = exec(env_dup_root, cmds, &gc, pipes, number_of_pipes);
			}
			//TOADD : Il faut se motiver a itoa status et a le str join avec ?=
			//pour pouvoir feed "?=status" dans la fonction export;
			// str_status = ft_strjoin("?=", itoa(status));
			// export(&env_dup_root, str_status, gc);
			//et normalement ca rajoute ca a l'env liste et du coup tu pourras l'echo tranquille
		}
		add_history(input); // TODO add condition so we dont add whitespace (or errors ? idk) to history;
	}
	
	// printf("Exit.\n");
	rl_clear_history();
	empty_trash(gc);
	return 0;
}

// this function will need to report 0 in case of error, trying to uniformize that
// 0 == ERR accross all functions
int	basic_parsing(t_garbage_collect **gc, char *input, t_token **token, t_cmd **cmds)
{
	char	**split_input;

	if (input == NULL)
		return (0);
	if (input[0] == '\0')
	{
		*token = NULL;
		return (0);
	}
	split_input = quote_split(input, gc);//ft_split(input, ' ');
	if (!split_input)
		return (0);
	if (parse(split_input, gc, token, cmds) == 0)
		return (0);
	return (1);
}

//En vrai je peux garder cette fonction pour run les builtins;
int	theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **cmd)
{
	if (cmd == NULL || cmd[0] == NULL)
		return (1);
	if (ft_strcmp(cmd[0], "unset") == 0)
		unset(*env_dup_root, cmd[1]);
	if (ft_strcmp(cmd[0], "export") == 0)
	{	
		if (cmd[1] == NULL)
			sorted_env_print(*env_dup_root, *gc);
		else
			export(env_dup_root, (void *)cmd[1], gc);
	}
	if (ft_strcmp(cmd[0], "env") == 0)
		env(*env_dup_root, *gc);
	if (ft_strcmp(cmd[0], "exit") == 0)
		ft_exit(&cmd[1], *gc);
	if (ft_strcmp(cmd[0], "pwd") == 0)
		pwd(gc);
	if (ft_strcmp(cmd[0], "cd") == 0)
		cd(cmd, gc, *env_dup_root);
	// if (ft_strcmp(cmd[0], "<<") == 0) // THIS WAS FOR TEST THIS IS BAD
	// 	here_doc(cmd[1] ,gc, 1);
	if (ft_strcmp(cmd[0], "echo") == 0)
		echo(cmd, gc);
	
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


