#include "../minishell.h"

// Variable global para la señal recibida
int g_signal_received = 0;

// Manejador de señales
void signal_handler(int signum)
{
    g_signal_received = signum;
    
    if (signum == SIGINT) // ctrl-C
    {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

// Configurar los manejadores de señales
void setup_signals(void)
{
    struct sigaction sa;
    
    // Inicializar la estructura sigaction a 0
    memset(&sa, 0, sizeof(sa));
    
    // Configurar el manejador
    sa.sa_handler = signal_handler;
    
    // Instalar el manejador para SIGINT (ctrl-C)
    sigaction(SIGINT, &sa, NULL);
    
    // Ignorar SIGQUIT (ctrl-\)
    signal(SIGQUIT, SIG_IGN);
}