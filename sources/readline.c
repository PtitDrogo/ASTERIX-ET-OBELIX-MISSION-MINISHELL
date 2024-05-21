/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/21 20:36:22 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// compile this code with cc readline.c -lreadline
int 	empty_trash(t_garbage_collect *gc);
int		add_to_trash(t_garbage_collect **root, void *to_free);
int		basic_parsing(t_garbage_collect **gc, char *input, t_token **token, t_cmd **cmds);
int		theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **input);
char	*accurate_shell(t_garbage_collect **gc, t_env_node *env);
bool	is_ascii(unsigned char c);
int		verify_input(char *input);
char    **rebuild_env_no_gc(t_env_node *root);
// bool	is_env_node(t_garbage_collect	*gc, t_env_node	*env_dup_root);
// int		free_and_pop_gc(t_garbage_collect **env_dup_root, t_garbage_collect *node_to_pop);
void	recycle_trash(t_garbage_collect	**gc, t_env_node	**env_dup_root);

int main(int argc, char const *argv[], char **envp)
{
	t_env_node			*env_dup_root;
	t_garbage_collect	*gc;
	t_token				*token;
	t_cmd				*cmds;
	char				*input;
	char				*history;
	int					**pipes;
	int					status;

	token = NULL;
	cmds = NULL;
	gc = NULL;
	env_dup_root = NULL;
	status = 0;
	// ft_memset(token, 0, sizeof(t_token));
	// ft_memset(cmds, 0, sizeof(t_cmd));
	// ft_memset(gc, 0, sizeof(t_garbage_collect));
	// ft_memset(env_dup_root, 0, sizeof(t_env_node));
	//foutre ca aux bons endroits
	// printf("launched shell\n");
	if (envp == NULL)
		return (1);
	generate_env_llist(&env_dup_root, &gc, envp);
	//export(&env_dup_root, ft_strjoin("?=", itoa(0)), gc);
	while (1) 
	{
		signal(SIGINT, new_prompt);
		signal(SIGQUIT, SIG_IGN);
		input = readline("myshell> ");
		if (add_to_trash(&gc, input) == 0)
			empty_trash_exit(gc, MALLOC_ERROR);
		if (!input)
			break;
		// Check for EOF (Ctrl+D)
		//GROS caca pour tenter de faire passer le testeur, a ne pas keep;
		// if (strcmp(input, "echo $?") == 0)
		// 	printf("%i\n", status);
		
		//J'ai rajoute un verify input sinon cat /dev/urandom/ fait crash le programme
		if (verify_input(input) && basic_parsing(&gc, input, &token, &cmds) && token)
		{
			expander(env_dup_root, &gc, cmds, status); //WORK IN PROGRESS
			int number_of_pipes = count_pipes(token);
			pipes = open_pipes(cmds, &gc, number_of_pipes);
			if (number_of_pipes == 0 && is_builtin(cmds->str))
				theo_basic_parsing(&env_dup_root, &gc, cmds->str);
			else
				status = exec(env_dup_root, cmds, &gc, pipes, number_of_pipes);
		}
		if (verify_input(input))
			add_history(input);
		recycle_trash(&gc, &env_dup_root);
		// ft_printf("Errno : %i\n", status);
	}
	
	// printf("Exit.\n");
	rl_clear_history();
	empty_trash(gc);
	return 0;
}

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
//TODO, add redirection for solo exec lol;
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
	if (ft_strcmp(cmd[0], "echo") == 0)
		echo(cmd, gc);
	
	return (0);
}

// for cat /dev/urandom | ./minishell otherwise it crashes;
int		verify_input(char *input)
{
	int i;
	
	i = 0;
	while (input[i])
	{
		if (is_ascii((unsigned char)input[i]) == false)
			return (0);
		i++;
	}
	return (1);
}

bool	is_ascii(unsigned char c)
{
	return (c <= 127);
}

//empties gc and reset the env;
//Recycle attempt by not touching the initial env dup list;

void	recycle_trash(t_garbage_collect	**gc, t_env_node	**env_dup_root)
{
	char **env_save;

	env_save = rebuild_env_no_gc(*env_dup_root);
	empty_trash(*gc);
	*gc = NULL;
	*env_dup_root = NULL;
	generate_env_llist(env_dup_root, gc, env_save);
	ft_free_array((void **)env_save);
	return ;
}
char    **rebuild_env_no_gc(t_env_node *root)
{
    int		number_of_variables;
	char	**envp;
	int		i;	
	
	i = 0;
	number_of_variables = count_nodes(root);
    envp = malloc(sizeof(char *) * (number_of_variables + 1));
	while (root)
    {
        if (root->variable)
		{
			envp[i] = ft_strjoin_and_add(root->variable_name, root->variable, '=');
			if (envp[i] == NULL)
				ft_free_array((void **)envp);
		}
		else
		{
			envp[i] = ft_strdup(root->variable_name);
			if (envp[i] == NULL)
				ft_free_array((void **)envp);
		}
        root = root->next;
		i++;
    }
	envp[i] = NULL;
	return (envp);
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


