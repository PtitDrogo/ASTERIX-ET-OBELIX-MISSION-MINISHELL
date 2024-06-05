/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_split_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:30:52 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 17:41:07 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	count_words(char *input)
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

size_t	extract_len(char *input)
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

char	*extract_redirection(char *input, char *res, size_t *index)
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

void	extract(char *input, char *res, size_t *index)
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
