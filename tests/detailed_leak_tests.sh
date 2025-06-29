#!/bin/bash

# Tests específicos para detectar memory leaks en áreas críticas de minishell
# Se enfoca en las funciones más propensas a leaks

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

MINISHELL="../minishell"

# Test específico con análisis detallado
detailed_leak_test() {
    local test_name="$1"
    local commands="$2"
    local description="$3"
    
    echo -e "${BLUE}🔍 Test detallado: $test_name${NC}"
    echo -e "${YELLOW}Descripción: $description${NC}"
    
    echo -e "$commands\nexit" > detailed_input.tmp
    
    # Crear supresiones específicas para este test
    cat > detailed_readline.supp << 'EOF'
{
   readline_all_malloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   obj:*/libreadline.so.*
}
{
   readline_all_realloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:realloc
   ...
   obj:*/libreadline.so.*
}
{
   history_all
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:add_history
}
{
   rl_functions
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:rl_*
}
{
   libc_initialization
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:__libc_*
}
EOF
    
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --suppressions=detailed_readline.supp \
             --log-file=detailed_output.tmp \
             $MINISHELL < detailed_input.tmp > /dev/null 2>&1
    
    echo "Resultados detallados:"
    # Filtrar solo leaks que NO sean de readline/history
    if grep -q "definitely lost\|indirectly lost\|possibly lost" detailed_output.tmp; then
        # Verificar si los leaks están relacionados con nuestro código
        if grep -A 10 -B 5 "lost" detailed_output.tmp | grep -v "libreadline\|add_history\|rl_\|__libc" | grep -q "lost"; then
            echo -e "${RED}❌ MEMORY LEAK DETECTADO (en tu código)${NC}"
            echo "Detalles completos:"
            grep -A 10 -B 5 "lost" detailed_output.tmp | grep -v "libreadline\|add_history\|rl_\|__libc"
        else
            echo -e "${GREEN}✅ Sin leaks detectados (solo leaks de readline ignorados)${NC}"
        fi
    else
        echo -e "${GREEN}✅ Sin leaks detectados${NC}"
    fi
    
    echo "---"
    rm -f detailed_input.tmp detailed_output.tmp detailed_readline.supp
}

echo -e "${YELLOW}🔬 Tests detallados de memory leaks en minishell${NC}"
echo "============================================================"

# =================== TESTS DE GESTIÓN DE ENVIRONMENT ===================
echo -e "${YELLOW}=== TESTS DE GESTIÓN DE ENVIRONMENT ===${NC}"

detailed_leak_test "Environment copy y cleanup" \
    "export NEW_VAR=test_value\nenv\nunset NEW_VAR" \
    "Verifica que copy_env_array() y las funciones de cleanup liberen correctamente la memoria"

detailed_leak_test "Múltiples exports consecutivos" \
    "export VAR1=val1\nexport VAR2=val2\nexport VAR3=val3\nexport VAR4=val4\nexport VAR5=val5" \
    "Verifica que add_env_value() no cause leaks al expandir el array de environment"

detailed_leak_test "Export y unset alternados" \
    "export TEST=1\nunset TEST\nexport TEST=2\nunset TEST\nexport TEST=3\nunset TEST" \
    "Verifica que remove_env_entry() libere correctamente la memoria al eliminar variables"

# =================== TESTS DE PARSING Y EXPANSIÓN ===================
echo -e "${YELLOW}=== TESTS DE PARSING Y EXPANSIÓN ===${NC}"

detailed_leak_test "Expansión de variables complejas" \
    "echo \$HOME/\$USER/\$PWD/\$PATH" \
    "Verifica que expand_variable() y process_quotes_and_vars() liberen buffers temporales"

detailed_leak_test "Parse de comandos con muchos argumentos" \
    "echo arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10" \
    "Verifica que split_command_args() y parse_command_input() liberen arrays de argumentos"

detailed_leak_test "Remove quotes complejo" \
    "echo \"test1\" 'test2' \"test3\" 'test4'" \
    "Verifica que remove_quotes() libere strings temporales correctamente"

# =================== TESTS DE PIPES Y PROCESOS ===================
echo -e "${YELLOW}=== TESTS DE PIPES Y PROCESOS ===${NC}"

detailed_leak_test "Pipeline con múltiples comandos" \
    "echo test | cat | cat | wc -l" \
    "Verifica que split_and_validate_commands() y execute_child_command() liberen memoria"

detailed_leak_test "Comando externo con PATH search" \
    "ls -la" \
    "Verifica que find_command_path() y search_in_paths() liberen paths temporales"

# =================== TESTS DE REDIRECCIONES ===================
echo -e "${YELLOW}=== TESTS DE REDIRECCIONES ===${NC}"

detailed_leak_test "Múltiples redirecciones" \
    "echo test1 > file1.tmp\necho test2 >> file1.tmp\ncat < file1.tmp" \
    "Verifica que las funciones de redirección liberen descriptores y buffers"

detailed_leak_test "Heredoc con expansión" \
    "cat << EOF\nHola \$USER\nTu home es \$HOME\nEOF" \
    "Verifica que heredoc() y write_expanded_line() liberen líneas expandidas"

# =================== TESTS DE ERROR HANDLING ===================
echo -e "${YELLOW}=== TESTS DE ERROR HANDLING ===${NC}"

detailed_leak_test "Comando con error de sintaxis" \
    "echo test ||\necho test >>" \
    "Verifica que los errores de parsing liberen memoria parcialmente parseada"

detailed_leak_test "Comando inexistente" \
    "comando_que_no_existe_en_absoluto" \
    "Verifica que execute_external() libere memoria incluso cuando falla exec"

# =================== TESTS DE CLEANUP EN EXIT ===================
echo -e "${YELLOW}=== TESTS DE CLEANUP EN EXIT ===${NC}"

detailed_leak_test "Exit después de múltiples operaciones" \
    "export VAR1=val1\nexport VAR2=val2\necho test | cat\npwd\ncd .." \
    "Verifica que cleanup_and_exit() libere toda la memoria acumulada"

echo -e "${BLUE}✅ Tests detallados completados${NC}"