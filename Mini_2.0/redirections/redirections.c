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

// Implementación del heredoc (<<) con expansión de variables
static void read_heredoc_input(int write_fd, char *delimiter, char **env)
{
    char *line = NULL;
    char *expanded_line;
    size_t bufsize = 0;
    ssize_t bytes_written;

    printf("> ");
    while (getline(&line, &bufsize, stdin) != -1)
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';
            
        // Verificar si llegamos al delimitador
        if (strcmp(line, delimiter) == 0)
            break;
            
        // Expandir variables en la línea
        expanded_line = expand_variable(line, env);
            
        // Escribir la línea expandida en el pipe
        bytes_written = write(write_fd, expanded_line, strlen(expanded_line));
        if (bytes_written == -1)
            perror("write");
            
        bytes_written = write(write_fd, "\n", 1);
        if (bytes_written == -1)
            perror("write");
            
        // Liberar la memoria de la línea expandida
        free(expanded_line);
            
        printf("> ");
    }
    free(line);
}

// Implementación del heredoc (<<): crea un pipe y redirige su lectura a stdin
int heredoc(char *delimiter, char **env)
{
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (1);
    }

    read_heredoc_input(pipefd[1], delimiter, env);
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
static void process_redirection(char **args, int *i, char **env)
{
    if (!args[*i + 1])
        return;

    // Expandir variables en el argumento de redirección y quitar comillas
    char *expanded_filename = expand_variable(args[*i + 1], env);
    char *cleaned_filename = remove_quotes(expanded_filename);

    if (strcmp(args[*i], "<") == 0)
    {
        redirect_input(cleaned_filename);
        (*i)++;
    }
    else if (strcmp(args[*i], "<<") == 0)
    {
        // Para heredoc no expandimos el delimitador, solo lo limpiamos
        char *cleaned_delimiter = remove_quotes(args[*i + 1]);
        heredoc(cleaned_delimiter, env);
        
        if (cleaned_delimiter != args[*i + 1])
            free(cleaned_delimiter);
        (*i)++;
    }
    else if (strcmp(args[*i], ">") == 0)
    {
        redirect_output(cleaned_filename, 0);
        (*i)++;
    }
    else if (strcmp(args[*i], ">>") == 0)
    {
        redirect_output(cleaned_filename, 1);
        (*i)++;
    }

    // Liberar la memoria si se crearon nuevas cadenas
    if (expanded_filename != args[*i])
        free(expanded_filename);
    if (cleaned_filename != expanded_filename)
        free(cleaned_filename);
    
    (*i)++;
}

// Filtra los argumentos quitando las redirecciones y aplicándolas
void handle_redirections(char ***args, char **env)
{
    char **new_args = malloc(sizeof(char *) * 100);
    int i = 0, j = 0;

    while ((*args)[i])
    {
        if (strcmp((*args)[i], "<") == 0 || strcmp((*args)[i], "<<") == 0 ||
            strcmp((*args)[i], ">") == 0 || strcmp((*args)[i], ">>") == 0)
        {
            process_redirection(*args, &i, env);
            continue;
        }
        new_args[j++] = (*args)[i++];
    }
    new_args[j] = NULL;
    free(*args);
    *args = new_args;
}
