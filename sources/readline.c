/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:35:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 13:46:06 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	execute_valid_input(t_data *data);
static void	open_pipe_n_exec(t_data *data);
static void	handle_solo_builtin(t_data *data);
static int	big_parser(t_data *data, char *input,
				t_token **token, t_cmd **cmds);

int	main(int argc, char const *argv[], char **envp)
{
	t_data	data;

	ft_memset(&data, 0, sizeof(data));
	generate_env_llist(&(data.env), &data.gc, envp);
	get_init_pwd(&data.gc, &data.env, argc, argv);
	data.status = exit_status(0);
	while (1)
	{
		data.input = NULL;
		data.status = exit_status(-1);
		signal(SIGINT, new_prompt);
		signal(SIGQUIT, SIG_IGN);
		data.input = readline(prompt(&data.gc, data.env));
		if (add_to_trash(&data.gc, data.input) == 0)
			empty_trash_exit(data.gc, MALLOC_ERROR);
		if (!data.input)
			break ;
		if (verify_input(data.input) && big_parser(&data,
				data.input, &data.token, &data.cmds) && data.token)
			execute_valid_input(&data);
		if (verify_input(data.input))
			add_history(data.input);
		recycle_trash_new(&data.gc, data.env);
	}
	return (minishell_graceful_exit(data.gc));
}

static void	execute_valid_input(t_data *data)
{
	before_expand_innit(data);
	if (exit_status(parse_all_here_docs(data)) == EXIT_SUCCESS)
	{
		expander(data);
		open_pipe_n_exec(data);
	}
	return ;
}

static void	open_pipe_n_exec(t_data *data)
{
	int	number_of_pipes;

	number_of_pipes = count_pipes(data->token);
	data->pipes = open_pipes(data->cmds, &data->gc, number_of_pipes);
	if (number_of_pipes == 0 && is_builtin(data->cmds->str))
		handle_solo_builtin(data);
	else
	{
		exit_status(exec(data, data->pipes, number_of_pipes));
	}
}

static void	handle_solo_builtin(t_data *data)
{
	int	backup_fds[2];
	int	process_status;

	backup_fds[0] = dup(0);
	backup_fds[1] = dup(1);
	if (backup_fds[0] == -1 || backup_fds[1] == -1)
		empty_trash_exit(data->gc, 1);
	process_status = process_behavior(data->cmds, data->token);
	close_all_heredoc_pipes(data->cmds, data->gc);
	if (process_status == 0)
		exit_status(builtin_parse(&data->env, &data->gc,
				data->cmds->str, backup_fds));
	if (dup2(backup_fds[0], 0) == -1 || dup2(backup_fds[1], 1) == -1)
		empty_trash_exit(data->gc, 1);
	if (close(backup_fds[0]) == -1 || close(backup_fds[1]) == -1)
		empty_trash_exit(data->gc, 1);
	if (process_status == 1)
		exit_status(1);
	else if (process_status == 2)
		empty_trash_exit(data->gc, 1);
}

static int	big_parser(t_data *data, char *input, t_token **token, t_cmd **cmds)
{
	char	**split_input;

	if (input == NULL)
		return (0);
	if (input[0] == '\0')
	{
		*token = NULL;
		return (0);
	}
	data->str_status = setter_gc(ft_itoa(data->status), &data->gc);
	malloc_check(data->str_status, data->gc);
	input = expand_single_str(data, input, STD_EX);
	split_input = quote_split(input, &data->gc);
	if (!split_input)
		return (0);
	if (parse(split_input, &data->gc, token, cmds) == 0)
		return (0);
	return (1);
}
