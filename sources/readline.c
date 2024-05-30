/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 07:15:55 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int 	empty_trash(t_garbage_collect *gc);
int		add_to_trash(t_garbage_collect **root, void *to_free);
int		basic_parsing(t_garbage_collect **gc, char *input, t_token **token, t_cmd **cmds);
int		theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **cmd, int backup_fds[2]);
char	*accurate_shell(t_garbage_collect **gc, t_env_node *env);
bool	is_ascii(unsigned char c);
int		verify_input(char *input);
char    **rebuild_env_no_gc(t_env_node *root);
void	recycle_trash(t_garbage_collect	**gc, t_env_node	**env_dup_root);
void	secure_dup2_no_exit(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes);

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
	if (envp == NULL)
		return (1);
	generate_env_llist(&env_dup_root, &gc, envp);
	status = exit_status(0); // Initializing it, not sure if needed
	while (1) 
	{
		status = exit_status(-1);
		signal(SIGINT, new_prompt);
		signal(SIGQUIT, SIG_IGN);
		input = readline("myshell> ");
		if (add_to_trash(&gc, input) == 0)
			empty_trash_exit(gc, MALLOC_ERROR);
		if (!input)
			break;
		if (verify_input(input) && basic_parsing(&gc, input, &token, &cmds) && token)
		{
			signal(SIGINT, cancel_cmd);
			char *str_status = ft_itoa(exit_status(-1));
			setter_gc(str_status, &gc);
			malloc_check(str_status, gc);
			if (exit_status(parse_all_here_docs(cmds, &gc, env_dup_root, str_status)) == EXIT_SUCCESS)
			{
				expander(env_dup_root, &gc, cmds, str_status); //WORK IN PROGRESS
				int number_of_pipes = count_pipes(token);
				pipes = open_pipes(cmds, &gc, number_of_pipes);
				if (number_of_pipes == 0 && is_builtin(cmds->str))
				{	
					
					int backup_fds[2];
					int process_status;
					backup_fds[0] = dup(0);
					backup_fds[1] = dup(1);

					process_status = process_behavior(cmds, &gc, token);
					if (process_status == 0)
						exit_status(theo_basic_parsing(&env_dup_root, &gc, cmds->str, backup_fds));
					
					//PUT STD back to normal
					dup2(backup_fds[0], STDIN_FILENO);
					dup2(backup_fds[1], STDOUT_FILENO);
					close(backup_fds[0]);
					close(backup_fds[1]);
					if (process_status == 1)
						exit_status(1); //if regular fail exit status is 1;
					else if (process_status == 2)
						empty_trash_exit(gc, errno); //if a write failed we exit shell
					//
				}
				else
					exit_status(exec(env_dup_root, cmds, &gc, pipes, number_of_pipes, cmds, token));
			}
		}
		if (verify_input(input))
			add_history(input);
		recycle_trash(&gc, &env_dup_root);
		// ft_printf("- Errno : %i -", exit_status(-1));
	}
	rl_clear_history();
	empty_trash(gc);
	return (0);
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
int	theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **cmd, int backup_fds[2])
{
	if (cmd == NULL || cmd[0] == NULL)
		return (1);
	if (ft_strcmp(cmd[0], "unset") == 0)
		return (unset(*env_dup_root, cmd[1]));
	if (ft_strcmp(cmd[0], "export") == 0)
	{	
		if (cmd[1] == NULL)
			return(sorted_env_print(*env_dup_root, *gc));
		else
			return(export(env_dup_root, (void *)cmd[1], gc));
	}
	if (ft_strcmp(cmd[0], "env") == 0)
		return (env(*env_dup_root, *gc));
	if (ft_strcmp(cmd[0], "exit") == 0)
		return (ft_exit(&cmd[1], *gc, backup_fds));
	if (ft_strcmp(cmd[0], "pwd") == 0)
		pwd(gc);
	if (ft_strcmp(cmd[0], "cd") == 0)
		return (cd(cmd, gc, *env_dup_root));
	if (ft_strcmp(cmd[0], "echo") == 0)
		return(echo(cmd, gc));
	return (0);
}

// for cat /dev/urandom | ./minishell otherwise it crashes;
int		verify_input(char *input)
{
	int i;
	
	i = 0;
	if (input[0] == '\n')
		return (0);
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

//no exit here;
int		print_open_err_msg(int errnumber, char *file)
{
	if (errnumber == ENOENT)
		if (ft_printf2("bash: %s: No such file or directory\n", file) == -1)
			return (2);
	if (errnumber == EACCES)
		if (ft_printf2("bash: %s: Permission denied\n", file) == -1)
			return (2);
	if (errnumber == EISDIR)
		if (ft_printf2("bash: %s: Is a directory\n", file) == -1)
			return (2);
	if (errnumber == EMFILE)
		if (ft_printf2("bash: %s: Too many files opened", file) == -1)
			return (2);
	return (1);
}
//no exit
void	secure_dup2_no_exit(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes)
{
	if (dup2(new_fd, old_fd) == -1)
		perror("Error duplicating file descriptor");
	return ;
}

//Ignore ca c'est une experience mais remplacer le home par ~ c'est chiant
// char	*prompt(t_garbage_collect **gc, t_env_node *env)
// {
// 	char *pwd;

// 	pwd = accurate_shell(gc, env);
	

// 	return (NULL);
// }

// char	*accurate_shell(t_garbage_collect **gc, t_env_node *env)
// {
// 	t_env_node *pwd;
// 	char		*backup_pwd;
	
// 	pwd = get_env_node(env, "PWD");
//  	if (pwd != NULL)
// 		return (pwd->variable);
// 	else
// 	{
// 		backup_pwd = getcwd(NULL, 0);
// 		if (backup_pwd == NULL)
// 			perror_exit(*gc, errno, "Failed to get current path");
// 		setter_gc(backup_pwd, gc);
// 		return (backup_pwd);
// 	}
// }
///FIN EXPERIENCE


