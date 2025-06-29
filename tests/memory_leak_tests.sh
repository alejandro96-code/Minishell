#!/bin/bash

# Script de tests para detectar memory leaks en minishell
# Requiere valgrind instalado

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

MINISHELL="../minishell"
VALGRIND_CMD="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --suppressions=readline.supp"
TEST_COUNT=0
LEAK_COUNT=0

# Función para ejecutar un test
run_test() {
    local test_name="$1"
    local commands="$2"
    local timeout_duration="${3:-10}"
    
    echo -e "${BLUE}🧪 Test: $test_name${NC}"
    TEST_COUNT=$((TEST_COUNT + 1))
    
    # Crear archivo temporal con comandos
    echo -e "$commands\nexit" > test_input.tmp
    
    # Ejecutar con valgrind y timeout
    timeout $timeout_duration $VALGRIND_CMD $MINISHELL < test_input.tmp > valgrind_output.tmp 2>&1
    local exit_code=$?
    
    # Verificar si hay leaks REALES (no "still reachable" de readline)
    if grep -q "definitely lost\|indirectly lost\|possibly lost" valgrind_output.tmp; then
        # Filtrar para ver si los leaks son de nuestro código o de readline
        local real_leaks=$(grep -A 10 -B 5 "definitely lost\|indirectly lost\|possibly lost" valgrind_output.tmp | grep -v "libreadline\|add_history\|rl_\|__libc\|dlopen\|libtinfo" | grep -c "definitely lost\|indirectly lost\|possibly lost" || echo "0")
        
        if [ "$real_leaks" -gt 0 ]; then
            echo -e "${RED}❌ LEAK DETECTADO (en tu código)${NC}"
            LEAK_COUNT=$((LEAK_COUNT + 1))
            echo "Detalles del leak:"
            grep -A 10 -B 5 "definitely lost\|indirectly lost\|possibly lost" valgrind_output.tmp | grep -v "libreadline\|add_history\|rl_\|__libc\|dlopen\|libtinfo"
            echo "---"
        else
            echo -e "${GREEN}✅ Sin leaks detectados (solo leaks de readline ignorados)${NC}"
        fi
    else
        if [ $exit_code -eq 124 ]; then
            echo -e "${YELLOW}⏱️  TIMEOUT (puede indicar leak o bucle infinito)${NC}"
            LEAK_COUNT=$((LEAK_COUNT + 1))
        else
            echo -e "${GREEN}✅ Sin leaks detectados${NC}"
        fi
    fi
    
    # Limpiar archivos temporales
    rm -f test_input.tmp valgrind_output.tmp
    echo ""
}

# Función para crear archivo de supresiones de readline mejorado
create_readline_suppressions() {
    cat > readline.supp << 'EOF'
{
   readline_malloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:readline
}
{
   readline_realloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:realloc
   ...
   fun:readline
}
{
   readline_calloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:calloc
   ...
   fun:readline
}
{
   history_malloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:add_history
}
{
   history_realloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:realloc
   ...
   fun:add_history
}
{
   rl_initialize
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:rl_initialize
}
{
   rl_redisplay
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:rl_redisplay
}
{
   rl_replace_line
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:rl_replace_line
}
{
   libc_dlopen
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:dlopen*
}
{
   libc_locale
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   fun:setlocale
}
EOF
}

echo -e "${YELLOW}🔧 Iniciando tests de memory leaks para minishell${NC}"
echo "========================================================"

# Verificar que valgrind esté instalado
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}❌ Error: valgrind no está instalado${NC}"
    echo "Instalar con: sudo apt-get install valgrind"
    exit 1
fi

# Verificar que minishell existe
if [ ! -f "$MINISHELL" ]; then
    echo -e "${RED}❌ Error: minishell no encontrado en $MINISHELL${NC}"
    echo "Compilar primero con: make"
    exit 1
fi

# Crear archivo de supresiones
create_readline_suppressions

echo -e "${BLUE}📋 Ejecutando tests categorizados...${NC}\n"

# =================== TESTS BÁSICOS ===================
echo -e "${YELLOW}=== TESTS BÁSICOS ===${NC}"

run_test "Comando simple" "ls"
run_test "Comando inexistente" "comando_que_no_existe"
run_test "Múltiples comandos simples" "pwd\nls\necho hola"

# =================== TESTS DE BUILTINS ===================
echo -e "${YELLOW}=== TESTS DE BUILTINS ===${NC}"

run_test "builtin echo simple" "echo hola mundo"
run_test "builtin echo con flags" "echo -n test"
run_test "builtin echo múltiple" "echo test1\necho test2\necho test3"

run_test "builtin pwd" "pwd"
run_test "builtin pwd múltiple" "pwd\npwd\npwd"

run_test "builtin env" "env"
run_test "builtin env después de export" "export TEST=valor\nenv"

run_test "builtin cd home" "cd ~"
run_test "builtin cd directorio inexistente" "cd /directorio/inexistente"
run_test "builtin cd con PATH relativo" "cd .\ncd .."

run_test "builtin export simple" "export TEST=valor"
run_test "builtin export múltiple" "export VAR1=val1\nexport VAR2=val2\nexport VAR3=val3"
run_test "builtin export sin valor" "export NUEVA_VAR"
run_test "builtin export con espacios" "export VAR=\"valor con espacios\""

run_test "builtin unset" "export TEST=valor\nunset TEST"
run_test "builtin unset múltiple" "export A=1\nexport B=2\nunset A B"

# =================== TESTS DE VARIABLES DE ENTORNO ===================
echo -e "${YELLOW}=== TESTS DE VARIABLES DE ENTORNO ===${NC}"

run_test "Expansión de variable simple" "echo \$HOME"
run_test "Expansión de variable inexistente" "echo \$VAR_INEXISTENTE"
run_test "Expansión múltiple" "echo \$HOME \$USER \$PATH"
run_test "Variable con comillas" "echo \"\$HOME\""
run_test "Expansión de \$?" "echo \$?"

# =================== TESTS DE QUOTES ===================
echo -e "${YELLOW}=== TESTS DE QUOTES ===${NC}"

run_test "Comillas simples" "echo 'hola mundo'"
run_test "Comillas dobles" "echo \"hola mundo\""
run_test "Comillas mixtas" "echo 'hola' \"mundo\""
run_test "Comillas anidadas" "echo \"valor: '\$HOME'\""
run_test "Comillas vacías" "echo '' \"\""

# =================== TESTS DE PIPES ===================
echo -e "${YELLOW}=== TESTS DE PIPES ===${NC}"

run_test "Pipe simple" "echo hola | cat"
run_test "Pipe doble" "echo hola | cat | cat"
run_test "Pipe triple" "echo test | cat | cat | cat"
run_test "Pipe con ls y grep" "ls | grep minishell" 15
run_test "Pipe con comandos builtin" "env | grep HOME"

# =================== TESTS DE REDIRECCIONES ===================
echo -e "${YELLOW}=== TESTS DE REDIRECCIONES ===${NC}"

run_test "Redirección de salida" "echo test > output.tmp"
run_test "Redirección de entrada" "cat < minishell.h"
run_test "Redirección append" "echo test >> output.tmp"
run_test "Redirecciones múltiples" "echo test1 > file1.tmp\necho test2 > file2.tmp"

# =================== TESTS DE HEREDOC ===================
echo -e "${YELLOW}=== TESTS DE HEREDOC ===${NC}"

run_test "Heredoc simple" "cat << EOF\nhola\nmundo\nEOF"
run_test "Heredoc con variables" "cat << EOF\n\$HOME\n\$USER\nEOF"

# =================== TESTS DE PARSING COMPLEJO ===================
echo -e "${YELLOW}=== TESTS DE PARSING COMPLEJO ===${NC}"

run_test "Comando con muchos argumentos" "echo a b c d e f g h i j k l m n o p q r s t u v w x y z"
run_test "Múltiples pipes y redirecciones" "echo test | cat > output.tmp"
run_test "Comando largo con variables" "echo \$HOME/\$USER/test/\$PATH"

# =================== TESTS DE SEÑALES ===================
echo -e "${YELLOW}=== TESTS DE SEÑALES ===${NC}"

run_test "Comando con Ctrl+C simulado" "sleep 1"
run_test "Múltiples comandos interrumpidos" "sleep 1\nsleep 1"

# =================== TESTS DE EDGE CASES ===================
echo -e "${YELLOW}=== TESTS DE EDGE CASES ===${NC}"

run_test "Input vacío" ""
run_test "Solo espacios" "   "
run_test "Solo tabs" "\t\t\t"
run_test "Comando con espacios extra" "  echo   test  "

run_test "Pipe al final (error)" "echo test |"
run_test "Redirección sin archivo" "echo test >"
run_test "Caracteres especiales" "echo \$\$ \$0 \$1"

# =================== TESTS DE MEMORIA INTENSIVOS ===================
echo -e "${YELLOW}=== TESTS DE MEMORIA INTENSIVOS ===${NC}"

run_test "Muchas variables export" "$(for i in {1..50}; do echo "export VAR$i=valor$i"; done)" 20
run_test "Comando muy largo" "echo $(printf 'a%.0s' {1..1000})" 15
run_test "Muchos pipes" "echo test$(printf ' | cat%.0s' {1..10})" 20

# =================== TESTS DE ESTRÉS ===================
echo -e "${YELLOW}=== TESTS DE ESTRÉS ===${NC}"

run_test "Muchos comandos seguidos" "$(for i in {1..20}; do echo "echo test$i"; done)" 15
run_test "Export y unset alternados" "$(for i in {1..10}; do echo -e "export VAR$i=val$i\nunset VAR$i"; done)" 15

# =================== RESUMEN ===================
echo "========================================================"
echo -e "${BLUE}📊 RESUMEN DE TESTS${NC}"
echo -e "Total de tests ejecutados: ${YELLOW}$TEST_COUNT${NC}"
echo -e "Tests con memory leaks: ${RED}$LEAK_COUNT${NC}"
echo -e "Tests sin leaks: ${GREEN}$((TEST_COUNT - LEAK_COUNT))${NC}"

if [ $LEAK_COUNT -eq 0 ]; then
    echo -e "${GREEN}🎉 ¡Felicidades! No se detectaron memory leaks${NC}"
else
    echo -e "${RED}⚠️  Se detectaron memory leaks en $LEAK_COUNT tests${NC}"
    echo -e "${YELLOW}💡 Revisar las funciones de cleanup y liberación de memoria${NC}"
fi

# Limpiar archivos temporales
rm -f readline.supp output.tmp file1.tmp file2.tmp

echo -e "${BLUE}✅ Tests completados${NC}"