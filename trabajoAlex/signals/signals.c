#include "../minishell.h"

// Manejador global para la señal SIGINT (Ctrl+C)
void sigint_handler(int sig)
{
    (void)sig;
    int ret;
    ret = write(STDOUT_FILENO, "\n", 1);
    (void)ret; // Para evitar warning de variable no utilizada
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

// Manejador global para la señal SIGQUIT (Ctrl+\)
void sigquit_handler(int sig)
{
    (void)sig;
    // No hace nada, simplemente ignora la señal
}

// Configura los manejadores de señales
void setup_signal_handlers(void)
{
    struct sigaction sa_int, sa_quit;
    
    // Configurar SIGINT (Ctrl+C)
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
    
    // Configurar SIGQUIT (Ctrl+\)
    sa_quit.sa_handler = sigquit_handler;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}

// Configuración de señales para procesos hijo
void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL);  // Comportamiento por defecto en los hijos
    signal(SIGQUIT, SIG_DFL); // Comportamiento por defecto en los hijos
}

// Restaura el comportamiento por defecto de las señales
void reset_signal_handlers(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
