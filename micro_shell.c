#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

/*
** Affiche un message d'erreur sur STDERR selon le format du sujet
** Concatène msg et path si fourni, suivi d'un retour à la ligne
*/
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

/*
** Affiche "error: fatal" et termine le programme
** Utilisée pour les erreurs système critiques (pipe, fork, dup2, etc.)
*/
void ft_fatal()
{
    ft_error("error: fatal", NULL);
    exit(1);
}

/*
** Calcule la longueur d'une chaîne de caractères
** Fonction utilitaire simple pour parcourir les chaînes
*/
int ft_strlen(char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

/*
** Implémente la commande built-in "cd"
** argv: tableau des arguments (cd + chemin)
** i: nombre d'arguments
** Vérifie qu'il y a exactement 2 arguments (cd + chemin)
** Gère les erreurs selon les spécifications du sujet
*/
void ft_cd(char **argv, int i)
{
    if (i != 2)
        ft_error("error: cd: bad arguments", NULL);
    else if (chdir(argv[1]) != 0)
        ft_error("error: cd: cannot change directory to ", argv[1]);
}

/*
** Fonction récursive d'exécution des commandes avec support des pipes
** argv: tableau des arguments de la commande
** i: nombre d'arguments
** envp: variables d'environnement
** 
** Logique:
** 1. Cherche s'il y a un pipe "|" dans les arguments
** 2. Si pipe trouvé: crée un processus fils pour la partie gauche,
**    redirige sa sortie vers le pipe, puis appelle récursivement
**    pour la partie droite en redirigeant l'entrée
** 3. Si pas de pipe: exécute directement la commande
*/
void ft_exec(char **argv, int i, char **envp)
{
    int fd[2];
    int has_pipe = 0;
    int j = 0;
    
    // Cherche la position du premier pipe
    while (j < i && strcmp(argv[j], "|"))
        j++;
    if (j < i)
        has_pipe = 1;
        
    if (has_pipe)
    {
        // Crée le pipe pour connecter les deux commandes
        if (pipe(fd) == -1)
            ft_fatal();
        
        int pid = fork();
        if (pid == -1)
            ft_fatal();
        else if (pid == 0)  // Processus fils: partie gauche du pipe
        {
            argv[j] = NULL;  // Termine la commande au pipe
            if (dup2(fd[1], STDOUT_FILENO) == -1)  // Redirige sortie vers pipe
                ft_fatal();
            if (close(fd[0]) == -1 || close(fd[1]) == -1)
                ft_fatal();
            if (execve(argv[0], argv, envp) == -1)
                ft_error("error: cannot execute ", argv[0]);
            exit(1);
        }
        else  // Processus parent: partie droite du pipe
        {
            if (dup2(fd[0], STDIN_FILENO) == -1)  // Redirige entrée depuis pipe
                ft_fatal();
            if (close(fd[0]) == -1 || close(fd[1]) == -1)
                ft_fatal();
            while (waitpid(-1, NULL, WUNTRACED) != -1)
                ;
            ft_exec(argv + j + 1, i - j - 1, envp);  // Récursion pour la suite
        }
    }
    else  // Pas de pipe: exécution simple
    {
        int pid = fork();
        if (pid == -1)
            ft_fatal();
        else if (pid == 0)  // Processus fils
        {
            if (execve(argv[0], argv, envp) == -1)
                ft_error("error: cannot execute ", argv[0]);
            exit(1);
        }
        else  // Processus parent: attendre la fin du fils
        {
            while (waitpid(-1, NULL, WUNTRACED) != -1)
                ;
        }
    }
}

/*
** Fonction principale: parse les arguments et exécute les commandes
** Divise la ligne de commande en blocs séparés par ";"
** Pour chaque bloc:
** - Si c'est "cd": appelle ft_cd
** - Sinon: appelle ft_exec pour gérer les pipes et l'exécution
*/
int main(int argc, char **argv, char **envp)
{
    int i = 1;      // Index courant dans argv
    int start = 1;  // Début du bloc de commande courant
    
    // Parcourt tous les arguments jusqu'à la fin
    while (i <= argc)
    {
        // Trouve la fin d'un bloc (séparateur ";" ou fin des arguments)
        if (i == argc || !strcmp(argv[i], ";"))
        {
            if (start < i)  // S'il y a des arguments dans ce bloc
            {
                if (!strcmp(argv[start], "cd"))
                    ft_cd(argv + start, i - start);
                else
                    ft_exec(argv + start, i - start, envp);
            }
            start = i + 1;  // Commence le prochain bloc après le ";"
        }
        i++;
    }
    return 0;
}
