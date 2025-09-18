/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bon_microshell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmetee-b <cmetee-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 10:36:46 by cmetee-b          #+#    #+#             */
/*   Updated: 2025/09/18 13:22:11 by cmetee-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

void    ft_error(char *msg, char *path)
{
    while (*msg)
        write(STDERR_FILENO, msg++, 1);
    if (path)
    {
        while (*path)
            write(STDERR_FILENO, path++, 1);
    }
    write(STDERR_FILENO, "\n", 1);
}

void    ft_exec(char **av, int i, int tmp_fd, char **env)
{
    av[i] = NULL;
    dup2(tmp_fd, STDIN_FILENO);
    close(tmp_fd);
    execve(av[0], av, env);
    ft_error("error: cannot execute ", av[0]);
    exit(1);
}

int main(int ac, char **av, char **env)
{
    int i;
    int fd[2];
    int tmp_fd;
    (void)ac;

    i = 0;
    tmp_fd = dup(STDIN_FILENO);
    
    while (av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        if (strcmp(av[0], "cd") == 0)
        {
            if (i != 2)
                ft_error("error: cd: bad arguments", NULL);
            else if (chdir(av[1]) != 0)
                ft_error("error: cd: cannot change directory to ", av[1]);
        }
        else if (i != 0 && (av[i] == NULL || strcmp(av[i], ";") == 0))
        {
            if (fork() == 0)
                ft_exec(av, i, tmp_fd, env);
            else
            {
                close(tmp_fd);
                while(waitpid(-1, NULL, WUNTRACED) != -1)
                    ;
                tmp_fd = dup(STDIN_FILENO);
            }
        }
        else if (i != 0 && strcmp(av[i], "|") == 0)
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                ft_exec(av, i, tmp_fd, env);
            }
            else{
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return (0);
}
