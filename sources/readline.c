/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 19:10:09 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int 	empty_trash(t_gc *gc);
int		add_to_trash(t_gc **root, void *to_free);
int		basic_parsing(t_gc **gc, char *input, t_token **token, t_cmd **cmds);
int		builtin_parse(t_env **env, t_gc **gc, char **cmd, int backup_fds[2]);
char	*accurate_shell(t_gc **gc, t_env *env);
bool	is_ascii(unsigned char c);
int		verify_input(char *input);
void	secure_dup2_no_exit(int new_fd, int old_fd, int **pipes, t_gc *gc, int number_of_pipes);
char	*prompt(t_gc **gc, t_env *env);
char	*accurate_shell(t_gc **gc, t_env *env);
void	handle_solo_builtin(t_data *data);
void	open_pipe_n_exec(t_data *data);
void	execute_valid_input(t_data *data);
void	minishell_graceful_exit(t_gc *gc);

int main(int argc, char const *argv[], char **envp)
{
	t_data				data;

	ft_memset(&data, 0, sizeof(data));
	generate_env_llist(&(data.env), &data.gc, envp);
	data.status = exit_status(0);
	while (1) 
	{
		data.status = exit_status(-1);
		signal(SIGINT, new_prompt);
		signal(SIGQUIT, SIG_IGN);
		data.input = readline(prompt(&data.gc, data.env));
		if (add_to_trash(&data.gc, data.input) == 0)
			empty_trash_exit(data.gc, MALLOC_ERROR);
		if (!data.input)
			break ;
		if (verify_input(data.input) && basic_parsing(&data.gc, data.input, &data.token, &data.cmds) && data.token)
			execute_valid_input(&data);
		if (verify_input(data.input))
			add_history(data.input);
		recycle_trash_new(&data.gc, data.env);
	}
	minishell_graceful_exit(data.gc);
	return (0);
}

void	execute_valid_input(t_data *data)
{
	before_expand_innit(data);
	if (exit_status(parse_all_here_docs(data)) == EXIT_SUCCESS)
	{
		expander(data);
		open_pipe_n_exec(data);
	}
	return ;
}
void	open_pipe_n_exec(t_data *data)
{
	int number_of_pipes;
	
	number_of_pipes = count_pipes(data->token);
	data->pipes = open_pipes(data->cmds, &data->gc, number_of_pipes);
	if (number_of_pipes == 0 && is_builtin(data->cmds->str))
		handle_solo_builtin(data);
	else
		exit_status(exec(data, data->pipes, number_of_pipes));
}


void	handle_solo_builtin(t_data *data)
{
	int backup_fds[2];
	int process_status;
	
	backup_fds[0] = dup(0);
	backup_fds[1] = dup(1);

	process_status = process_behavior(data->cmds, &data->gc, data->token);
	close_all_heredoc_pipes(data->cmds, data->gc);
	if (process_status == 0)
		exit_status(builtin_parse(&data->env, &data->gc, data->cmds->str, backup_fds));
	
	dup2(backup_fds[0], STDIN_FILENO);
	dup2(backup_fds[1], STDOUT_FILENO);
	close(backup_fds[0]);
	close(backup_fds[1]);
	if (process_status == 1)
		exit_status(1);
	else if (process_status == 2)
		empty_trash_exit(data->gc, errno);
	
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
	split_input = quote_split(input, gc);
	if (!split_input)
		return (0);
	if (parse(split_input, gc, token, cmds) == 0)
		return (0);
	return (1);
}

int	builtin_parse(t_env **env, t_gc **gc, char **cmd, int backup_fds[2])
{
	if (cmd == NULL || cmd[0] == NULL)
		return (1);
	if (ft_strcmp(cmd[0], "unset") == 0)
		return (unset(*env, cmd[1]));
	if (ft_strcmp(cmd[0], "export") == 0)
	{	
		if (cmd[1] == NULL)
			return(sorted_env_print(*env, *gc));
		else
			return(export(env, (void *)cmd[1], gc));
	}
	if (ft_strcmp(cmd[0], "env") == 0)
		return (env_builtin(*env, *gc));
	if (ft_strcmp(cmd[0], "exit") == 0)
		return (ft_exit(&cmd[1], *gc, backup_fds));
	if (ft_strcmp(cmd[0], "pwd") == 0)
		pwd(gc);
	if (ft_strcmp(cmd[0], "cd") == 0)
		return (cd(cmd, gc, *env));
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


