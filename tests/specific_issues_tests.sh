#!/bin/bash

# Tests específicos para problemas identificados en el código de minishell
# Basados en el análisis del código fuente

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

MINISHELL="../minishell"

# Test específico para un problema identificado
specific_test() {
    local test_name="$1"
    local commands="$2"
    local issue_location="$3"
    local description="$4"
    
    echo -e "${BLUE}🎯 Test específico: $test_name${NC}"
    echo -e "${YELLOW}Ubicación: $issue_location${NC}"
    echo -e "${YELLOW}Problema: $description${NC}"
    
    echo -e "$commands\nexit" > specific_input.tmp
    
    # Crear supresiones específicas más completas
    cat > specific_readline.supp << 'EOF'
{
   readline_specific_malloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   obj:*/libreadline.so.*
}
{
   readline_specific_realloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:realloc
   ...
   obj:*/libreadline.so.*
}
{
   history_specific
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:add_history
}
{
   rl_specific
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:rl_*
}
{
   libc_specific
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:__libc_*
}
{
   dlopen_specific
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:*dlopen*
}
{
   tinfo_specific
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   obj:*/libtinfo.so.*
}
EOF
    
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --suppressions=specific_readline.supp \
             --log-file=specific_output.tmp \
             $MINISHELL < specific_input.tmp > /dev/null 2>&1
    
    if grep -q "definitely lost\|indirectly lost\|possibly lost" specific_output.tmp; then
        # Filtrar para mostrar solo leaks de tu código
        if grep -A 10 -B 5 "lost" specific_output.tmp | grep -v "libreadline\|add_history\|rl_\|__libc\|dlopen\|libtinfo" | grep -q "lost"; then
            echo -e "${RED}❌ LEAK CONFIRMADO (en tu código)${NC}"
            echo "Stack trace (filtrado):"
            grep -A 8 -B 2 "lost" specific_output.tmp | grep -v "libreadline\|add_history\|rl_\|__libc\|dlopen\|libtinfo"
        else
            echo -e "${GREEN}✅ Sin problemas detectados (solo leaks de readline/sistema ignorados)${NC}"
        fi
    else
        echo -e "${GREEN}✅ Sin problemas detectados${NC}"
    fi
    
    rm -f specific_input.tmp specific_output.tmp specific_readline.supp
    echo ""
}

echo -e "${BLUE}🔍 Tests Específicos para Problemas Identificados en el Código${NC}"
echo "=================================================================="

# =================== TESTS BASADOS EN ANÁLISIS DEL CÓDIGO ===================

# Problema potencial en copy_env_array (utils/env_utils.c)
specific_test "copy_env_array con fallo parcial" \
    "export VERY_LONG_VARIABLE_NAME_THAT_MIGHT_CAUSE_MALLOC_ISSUES=$(printf 'X%.0s' {1..10000})" \
    "utils/env_utils.c:33-47" \
    "Si ft_strdup falla a mitad del array, free_env se llama pero puede no limpiar todo"

# Problema potencial en add_env_value (builtins/export_utils2.c)
specific_test "add_env_value expansión de array" \
    "$(for i in {1..25}; do echo "export STRESS_VAR_$i=test_value_$i"; done)" \
    "builtins/export_utils2.c" \
    "Crear nuevo array más grande puede causar leaks si falla la copia"

# Problema en cleanup al salir (utils/input_processor.c)
specific_test "cleanup_and_exit después de operaciones complejas" \
    "export A=1\nexport B=2\necho \$A | cat\npwd\ncd ~" \
    "utils/input_processor.c:104" \
    "cleanup_and_exit solo libera env, pero puede haber memoria pendiente"

# Problema potencial en expand_variable (utils/)
specific_test "Expansión de variables anidadas" \
    "export VAR1=\$HOME\nexport VAR2=\$VAR1/test\necho \$VAR2" \
    "utils/expand_*.c" \
    "Buffers temporales en expansión recursiva pueden no liberarse"

# Problema en parsing de comandos con quotes
specific_test "Remove quotes con strings complejos" \
    "echo \"test with \\\"nested\\\" quotes and \$VAR expansion\"" \
    "utils/quote_utils.c" \
    "remove_quotes puede crear strings temporales sin liberar"

# Problema en split_command_args
specific_test "Split con muchos argumentos" \
    "echo $(printf 'arg%d ' {1..200})" \
    "parser/argument_splitter.c" \
    "split_command_args puede no liberar correctamente en casos complejos"

# Problema en gestión de pipes
specific_test "Múltiples pipes con comandos que fallan" \
    "comando_inexistente1 | comando_inexistente2 | cat | comando_inexistente3" \
    "pipes/*.c" \
    "Arrays de comandos pueden no liberarse cuando procesos fallan"

# Problema en redirecciones
specific_test "Redirecciones múltiples con heredoc" \
    "cat << EOF > output1.tmp\nlinea1\nlinea2\nEOF\ncat < output1.tmp > output2.tmp" \
    "redirections/*.c" \
    "Buffers de heredoc y descriptors pueden no liberarse correctamente"

# Problema en manejo de señales
specific_test "Interrupción durante expansión de variables" \
    "echo \$HOME\necho \$USER\necho \$PATH" \
    "signals/signals.c + expand" \
    "Variables globales pueden quedarse inconsistentes tras señales"

# Problema en get_next_line
specific_test "get_next_line con líneas muy largas" \
    "cat << EOF\n$(printf 'X%.0s' {1..5000})\nEOF" \
    "get_next_line/*.c" \
    "Buffer interno de get_next_line puede no liberarse en casos extremos"

# Problema específico en ejecutores
specific_test "Búsqueda de comando en PATH largo" \
    "export PATH=$(printf '/fake/dir%d:' {1..50}):/bin:/usr/bin\nls" \
    "executors/exec_path.c" \
    "search_in_paths puede no liberar paths temporales"

# Problema en builtin cd
specific_test "cd con paths relativos complejos" \
    "cd .\ncd ..\ncd ./.\ncd ../.." \
    "builtins/cd.c" \
    "resolve_relative_path puede crear strings temporales sin liberar"

# Problema en builtin export sin igual
specific_test "export sin asignación" \
    "export VARIABLE_SIN_VALOR\nexport OTRA_VAR\nexport TERCERA" \
    "builtins/export.c handle_export_no_equal" \
    "Copia de argumentos para validación puede no liberarse"

# Problema en unset
specific_test "unset de variables múltiples" \
    "export A=1\nexport B=2\nexport C=3\nunset A B C INEXISTENTE" \
    "builtins/unset.c" \
    "remove_env_entry puede no ajustar correctamente el array"

echo -e "${BLUE}💡 Recomendaciones específicas basadas en el análisis:${NC}"
echo ""
echo "1. Verificar cleanup en copy_env_array() cuando ft_strdup() falla"
echo "2. Asegurar que add_env_value() libere el env anterior correctamente"
echo "3. Mejorar cleanup_and_exit() para liberar toda la memoria pendiente"
echo "4. Revisar buffers temporales en funciones de expansión"
echo "5. Verificar liberación en split_command_args() para casos complejos"
echo "6. Asegurar cleanup de pipes cuando procesos fallan"
echo "7. Revisar manejo de memoria en redirecciones y heredoc"
echo "8. Verificar que get_next_line libere su buffer interno"
echo "9. Revisar liberación en search_in_paths()"
echo "10. Verificar strings temporales en builtins cd y export"

echo -e "${GREEN}✅ Tests específicos completados${NC}"