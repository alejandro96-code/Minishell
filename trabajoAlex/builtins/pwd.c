#include <stdio.h>
#include <string.h>

/*
    Muestra el directorio actual de trabajo usando getcwd.
*/

int builtin_pwd(char **env)
{
    char cwd[4096];

    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
    (void)env;
    return (0);
}
