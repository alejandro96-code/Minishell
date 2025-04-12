#include "../minishell.h"

// Redirigir la entrada desde un archivo
int redirect_input(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror(filename);
        return 1;
    }
    
    // Duplicar el descriptor de archivo a la entrada estándar
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

// Redirigir la salida a un archivo
int redirect_output(char *filename, int append)
{
    int flags = O_WRONLY | O_CREAT;
    
    // Si append es 1, usamos O_APPEND, si no, O_TRUNC
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
        
    int fd = open(filename, flags, 0644);
    if (fd < 0)
    {
        perror(filename);
        return 1;
    }
    
    // Duplicar el descriptor de archivo a la salida estándar
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

// Implementación del heredoc (<<)
// Lee línea por línea desde stdin hasta encontrar el delimitador, escribiéndolas en un pipe
static void read_heredoc_input(int write_fd, char *delimiter)
{
    char *line = NULL;
    size_t bufsize = 0;
    ssize_t bytes_written;

    printf("> ");
    while (getline(&line, &bufsize, stdin) != -1)
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';
        if (strcmp(line, delimiter) == 0)
            break;

        bytes_written = write(write_fd, line, strlen(line));
        if (bytes_written == -1)
            perror("write");

        bytes_written = write(write_fd, "\n", 1);
        if (bytes_written == -1)
            perror("write");

        printf("> ");
    }
    free(line);
}

// Implementación del heredoc (<<): crea un pipe y redirige su lectura a stdin
int heredoc(char *delimiter)
{
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (1);
    }

    read_heredoc_input(pipefd[1], delimiter);
    close(pipefd[1]);

    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        perror("dup2");
        close(pipefd[0]);
        return (1);
    }

    close(pipefd[0]);
    return (0);
}

// Función para procesar las redirecciones en los argumentos
// Procesa una redirección individual según el tipo y avanza el índice
static void process_redirection(char **args, int *i)
{
    if (!args[*i + 1])
        return;

    if (strcmp(args[*i], "<") == 0)
        redirect_input(args[++(*i)]);
    else if (strcmp(args[*i], "<<") == 0)
        heredoc(args[++(*i)]);
    else if (strcmp(args[*i], ">") == 0)
        redirect_output(args[++(*i)], 0);
    else if (strcmp(args[*i], ">>") == 0)
        redirect_output(args[++(*i)], 1);
    (*i)++;
}

// Filtra los argumentos quitando las redirecciones y aplicándolas
void handle_redirections(char ***args)
{
    char **new_args = malloc(sizeof(char *) * 100);
    int i = 0, j = 0;

    while ((*args)[i])
    {
        if (strcmp((*args)[i], "<") == 0 || strcmp((*args)[i], "<<") == 0 ||
            strcmp((*args)[i], ">") == 0 || strcmp((*args)[i], ">>") == 0)
        {
            process_redirection(*args, &i);
            continue;
        }
        new_args[j++] = (*args)[i++];
    }
    new_args[j] = NULL;
    free(*args);
    *args = new_args;
}
