#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void ft_error(char *msg, char *path)
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

void ft_fatal()
{
    ft_error("error: fatal", NULL);
    //exit(1);
    kill(0, SIGINT);
}

int ft_strlen(char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

void ft_cd(char **argv, int i)
{
    if (i != 2)
        ft_error("error: cd: bad arguments", NULL);
    else if (chdir(argv[1]) != 0)
        ft_error("error: cd: cannot change directory to ", argv[1]);
}

void ft_exec_simple(char **argv, char **envp)
{
    int pid = fork();
    if (pid == -1)
        ft_fatal();
    else if (pid == 0)
    {
        if (execve(argv[0], argv, envp) == -1)
            ft_error("error: cannot execute ", argv[0]);
        exit(1);
    }
    else
    {
        if (waitpid(pid, NULL, 0) == -1)
            ft_fatal();
    }
}

void ft_exec(char **argv, int i, char **envp)
{
    int fd[2];
    int j = 0;
    
    while (j < i && strcmp(argv[j], "|"))
        j++;
    if (j < i)
    {
        if (pipe(fd) == -1)
            ft_fatal();
        
        int pid = fork();
        if (pid == -1)
            ft_fatal();
        else if (pid == 0)
        {
            argv[j] = NULL;
            if (dup2(fd[1], STDOUT_FILENO) == -1)
                ft_fatal();
            if (close(fd[0]) == -1 || close(fd[1]) == -1)
                ft_fatal();
            if (execve(argv[0], argv, envp) == -1)
                ft_error("error: cannot execute ", argv[0]);
            exit(1);
        }
        else
        {
            if (dup2(fd[0], STDIN_FILENO) == -1)
                ft_fatal();
            if (close(fd[0]) == -1 || close(fd[1]) == -1)
                ft_fatal();
            if (waitpid(pid, NULL, 0) == -1)
                ft_fatal();
            ft_exec(argv + j + 1, i - j - 1, envp);
        }
    }
    else
        ft_exec_simple(argv, envp);
}

int main(int argc, char **argv, char **envp)
{
    int i = 1;
    int start = 1;
    
    while (i <= argc)
    {
        if (i == argc || !strcmp(argv[i], ";"))
        {
            if (start < i)
            {
                if (!strcmp(argv[start], "cd"))
                    ft_cd(argv + start, i - start);
                else
                    ft_exec(argv + start, i - start, envp);
            }
            start = i + 1;
        }
        i++;
    }
    return 0;
}
