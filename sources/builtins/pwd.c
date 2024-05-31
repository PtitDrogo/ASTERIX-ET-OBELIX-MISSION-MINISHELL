/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 19:05:05 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 18:58:07 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//pwd actually asks the operating system for the current path
//and doesnt use the env at all
int pwd(t_garbage_collect **gc)
{
    char *pwd;
    char *backup_pwd; //fill this later;

    
    backup_pwd = "hello I am backup pwd";
    pwd = getcwd(NULL, 0);
    setter_gc(pwd, gc);
    if (pwd)
    {
        if (printf("%s\n", pwd) == -1)
            perror_exit(*gc, errno, WRITE_ERR_MSG);
    }
    else
    {
        if (printf("%s\n",backup_pwd) == -1)
            perror_exit(*gc, errno, WRITE_ERR_MSG);
    }
    return (1);
}