/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 12:35:12 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int 	empty_trash(t_gc *gc);
int		add_to_trash(t_gc **root, void *to_free);
int		basic_parsing(t_gc **gc, char *input, t_token **token, t_cmd **cmds);
int		theo_basic_parsing(t_env **env_dup_root, t_gc **gc, char **cmd, int backup_fds[2]);
char	*accurate_shell(t_gc **gc, t_env *env);
bool	is_ascii(unsigned char c);
int		verify_input(char *input);
char    **rebuild_env_no_gc(t_env *root);
void	recycle_trash(t_gc	**gc, t_env	**env_dup_root);
void	secure_dup2_no_exit(int new_fd, int old_fd, int **pipes, t_gc *gc, int number_of_pipes);
char	*prompt(t_gc **gc, t_env *env);
char	*accurate_shell(t_gc **gc, t_env *env);

int main(int argc, char const *argv[], char **envp)
{
	// t_env				*env_dup_root;
	// t_gc				*gc;
	// t_token				*token;
	// t_cmd				*cmds;
	// char				*input;
	// char				*history;
	// int					**pipes;
	// int					status;
	t_data				data;

	// token = NULL;
	// cmds = NULL;
	// gc = NULL;
	// env_dup_root = NULL;
	// status = 0;

	ft_memset(&data, 0, sizeof(data));
	generate_env_llist(&(data.env_dup_root), &data.gc, envp);
	data.status = exit_status(0); // Initializing it, not sure if needed
	while (1) 
	{
		data.status = exit_status(-1);
		signal(SIGINT, new_prompt);
		signal(SIGQUIT, SIG_IGN);
		data.input = readline(prompt(&data.gc, data.env_dup_root));
		if (add_to_trash(&data.gc, data.input) == 0)
			empty_trash_exit(data.gc, MALLOC_ERROR);
		if (!data.input)
		{	
			ft_printf("exit\n");
			break ;
		}
		if (verify_input(data.input) && basic_parsing(&data.gc, data.input, &data.token, &data.cmds) && data.token)
		{
			signal(SIGINT, cancel_cmd);
			signal(SIGQUIT, cancel_cmd);
			char *str_status = ft_itoa(exit_status(-1));
			setter_gc(str_status, &data.gc);
			malloc_check(str_status, data.gc);
			if (exit_status(parse_all_here_docs(data.cmds, &data.gc, data.env_dup_root, str_status)) == EXIT_SUCCESS)
			{
				expander(data.env_dup_root, &data.gc, data.cmds, str_status); //WORK IN PROGRESS
				int number_of_pipes = count_pipes(data.token);
				data.pipes = open_pipes(data.cmds, &data.gc, number_of_pipes);
				if (number_of_pipes == 0 && is_builtin(data.cmds->str))
				{	
					
					int backup_fds[2];
					int process_status;
					backup_fds[0] = dup(0);
					backup_fds[1] = dup(1);

					process_status = process_behavior(data.cmds, &data.gc, data.token);
					close_all_heredoc_pipes(data.cmds, data.gc);
					if (process_status == 0)
						exit_status(theo_basic_parsing(&data.env_dup_root, &data.gc, data.cmds->str, backup_fds));
					
					//PUT STD back to normal
					dup2(backup_fds[0], STDIN_FILENO);
					dup2(backup_fds[1], STDOUT_FILENO);
					close(backup_fds[0]);
					close(backup_fds[1]);
					if (process_status == 1)
						exit_status(1); //if regular fail exit status is 1;
					else if (process_status == 2)
						empty_trash_exit(data.gc, errno); //if a write failed we exit shell
					//
				}
				else
					exit_status(exec(&data, data.pipes, number_of_pipes));
			}
		}
		if (verify_input(data.input))
			add_history(data.input);
		recycle_trash_new(&data.gc, data.env_dup_root);
	}
	rl_clear_history();
	empty_trash(data.gc);
	return (0);
}

int	basic_parsing(t_gc **gc, char *input, t_token **token, t_cmd **cmds)
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

int	theo_basic_parsing(t_env **env_dup_root, t_gc **gc, char **cmd, int backup_fds[2])
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
	if (input[0] == '\0')
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
void	secure_dup2_no_exit(int new_fd, int old_fd, int **pipes, t_gc *gc, int number_of_pipes)
{
	if (dup2(new_fd, old_fd) == -1)
		perror("Error duplicating file descriptor");
	return ;
}

// Ignore ca c'est une experience mais remplacer le home par ~ c'est chiant
char	*prompt(t_gc **gc, t_env *env)
{
	char *pwd;

	pwd = accurate_shell(gc, env);
	if (pwd)
	{
		pwd = setter_gc(ft_strjoin("Minishell:", pwd), gc);
		pwd = setter_gc(ft_strjoin(pwd, "$ "), gc);
	}
	else
		pwd = ("Minishell:$ ");
	return (pwd);
}

char	*accurate_shell(t_gc **gc, t_env *env)
{
	t_env *pwd;
	char		*backup_pwd;
	
	pwd = get_env_node(env, "PWD");
 	if (pwd != NULL)
		return (pwd->variable);
	else
	{
		backup_pwd = getcwd(NULL, 0);
		setter_gc(backup_pwd, gc);
		return (backup_pwd);
	}
}


