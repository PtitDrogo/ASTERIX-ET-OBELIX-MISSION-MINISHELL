/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 18:11:38 by garivo            #+#    #+#             */
/*   Updated: 2024/06/02 22:19:29 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_quoted_words(char *input, size_t *i,
	int *firstchar)
{
	size_t	quote;
	char	separator;

	separator = 0;
	if (input[*i] == '"' || input[*i] == '\'')
		separator = input[*i];
	quote = *i;
	if (separator != 0)
	{
		while (input[++*i] && input[*i] != separator)
			*i = *i;
		if (!input[*i] || ++*i == -1)
			return (ft_printf("Unclosed quote\n"), -1);
		*firstchar = 1;
		if (*i != quote + 2)
			return (1);
		else
			return (2);
	}
	return (0);
}

static size_t	count_unquoted_words(char *input, size_t *i,
	int quoted_count, int *firstchar)
{
	size_t	wc;

	wc = 0;
	if (quoted_count == 1 && *firstchar && ++wc > 0 || quoted_count == 2)
		*firstchar = 0;
	else if (input[(*i)] == ' ')
	{
		while (input[(*i)] == ' ')
			(*i)++;
		*firstchar = 1;
	}
	else if ((input[(*i)] == '<' || input[(*i)] == '>'
			|| input[(*i)] == '|') && ++wc > 0)
	{
		if (input[(*i) + 1] == input[(*i)])
			(*i)++;
		(*i)++;
		*firstchar = 1;
	}
	else if (*firstchar && ++wc > 0 && ++(*i) > 0)
		*firstchar = 0;
	else
		(*i)++;
	return (wc);
}

static size_t	count_words(char *input)
{
	size_t	i;
	size_t	wc;
	int		quoted_count;
	int		firstchar;

	wc = 0;
	firstchar = 1;
	i = 0;
	while (input[i])
	{
		quoted_count = count_quoted_words(input, &i, &firstchar);
		if (quoted_count == -1)
			return (0);
		wc += count_unquoted_words(input, &i, quoted_count, &firstchar);
	}
	return (wc);
}

static size_t	get_extract_len(char *input)
{
	char	separator;
	size_t	i;
	size_t	quote_start;
	size_t	len;

	i = 0;
	len = 0;
	while (input[i] && input[i] != ' ' && input[i] != '<' && input[i] != '>')
	{
		quote_start = i;
		separator = 0;
		if ((input[i] == '\'' || input[i] == '"') && ++i && ++len)
		{
			separator = input[i - 1];
			while (input[i] && input[i] != separator && ++len)
				i++;
			if (input[i] && i++ - quote_start == 1)
				len -= 1;
			else
				len++;
		}
		else if (++i > 0 && ++len > 0)
			len = len;
	}
	return (len);
}

static size_t	extract_len(char *input)
{
	char	separator;
	size_t	i;

	i = 0;
	if (input[i] == '<' || input[i] == '>' || input[i] == '|')
	{
		if (input[i + 1] == input[i])
			return (2);
		return (1);
	}
	return (get_extract_len(input));
}

static char	*extract_redirection(char *input, char *res, size_t *index)
{
	size_t	i;

	i = 0;
	if ((input[i] == '<' || input[i] == '>'
			|| input[i] == '|') && ++(*index) > 0)
	{
		if (input[i + 1] == input[i] && ++(*index) > 0)
			return (ft_strncat(&input[i], res, 2));
		return (ft_strncat(&input[i], res, 1));
	}
	return (NULL);
}

static void	extract(char *input, char *res, size_t *index)
{
	size_t	i;
	char	separator;
	size_t	quote_start;

	i = 0;
	if (extract_redirection(input, res, index) != NULL)
		return ;
	while (input[i] && input[i] != ' ' && input[i] != '<'
		&& input[i] != '>' && input[i] != '|')
	{
		quote_start = i;
		separator = 0;
		if ((input[i] == '\'' || input[i] == '"') && ++i && ++*index)
		{
			separator = input[i - 1];
			while (input[i] && input[i] != separator && ++*index)
				i++;
			if (i - quote_start > 1)
				ft_strncat(&input[quote_start], res, i - quote_start + 1);
		}
		else
			ft_strncat(&input[i], res, 1);
		i++;
		(*index)++;
	}
}

static char	**quote_splitting(char *input, char **res, t_garbage_collect **gc)
{
	size_t	i;
	size_t	j;
	size_t	start;
	size_t	len;

	i = 0;
	j = 0;
	while (input[i])
	{
		start = i;
		len = extract_len(&input[i]);
		if (len > 0)
		{
			res[j] = malloc_trash(len + 1, gc);
			*res[j] = '\0';
		}
		extract(&input[i], res[j], &i);
		if (len > 0)
			j++;
		else if (start == i)
			i++;
	}
	res[j] = NULL;
	return (res);
}

char	**quote_split(char *input, t_garbage_collect **gc)
{
	char	**res;

	if (count_words(input) == 0)
		return (NULL);
	res = malloc_trash((count_words(input) + 1) * sizeof(char *), gc);
	return (quote_splitting(input, res, gc));
}
