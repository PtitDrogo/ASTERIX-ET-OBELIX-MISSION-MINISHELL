/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 19:05:05 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/23 19:47:51 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//pwd actually asks the operating system for the current path
//and doesnt use the env at all
int pwd(t_garbage_collect **gc)
{
    char *pwd;
    
    pwd = getcwd(NULL, 0);
    if (pwd == NULL)
        perror_exit(*gc, errno, "Failed to get current path");
    setter_gc(pwd, gc);
    if (printf("%s\n", pwd) == -1)
        perror_exit(*gc, errno, "Printf failed");
    return (1);
}