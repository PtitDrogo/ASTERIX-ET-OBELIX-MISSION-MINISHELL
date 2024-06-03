/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:45:35 by tfreydie         ###   ########.fr       */
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
	t_env			*env_dup_root;
	t_gc	*gc;
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

	generate_env_llist(&env_dup_root, &gc, envp);
	status = exit_status(0); // Initializing it, not sure if needed
	while (1) 
	{
		status = exit_status(-1);
		signal(SIGINT, new_prompt);
		signal(SIGQUIT, SIG_IGN);
		input = readline(prompt(&gc, env_dup_root));
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
					close_all_heredoc_pipes(cmds, gc);
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
					exit_status(exec(env_dup_root, cmds, &gc, pipes, number_of_pipes, token));
			}
		}
		if (verify_input(input))
			add_history(input);
		recycle_trash_new(&gc, env_dup_root);
	}
	rl_clear_history();
	empty_trash(gc);
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


