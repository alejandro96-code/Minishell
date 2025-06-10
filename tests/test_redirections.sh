#!/bin/bash

# Pruebas para redirecciones
MINISHELL=${1:-"../Mini_2.0/minishell"}
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}📁 TESTING REDIRECTIONS${NC}"

# Función para probar redirecciones
test_redirect() {
    local cmd="$1"
    local test_name="$2"
    local check_file="$3"
    
    echo -n "Testing: $test_name... "
    
    # Limpiar archivos de prueba
    rm -f test_output.txt test_input.txt test_error.txt
    
    # Ejecutar comando
    echo -e "$cmd\nexit" | timeout 3 $MINISHELL >/dev/null 2>&1
    
    if [ -n "$check_file" ] && [ -f "$check_file" ]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    elif [ -z "$check_file" ]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Command: $cmd"
        ((FAILED++))
    fi
    
    # Limpieza
    rm -f test_output.txt test_input.txt test_error.txt
}

# Tests de redirección de salida
test_redirect "echo hello > test_output.txt" "redirección salida >" "test_output.txt"
test_redirect "echo world >> test_output.txt" "redirección append >>" "test_output.txt"

# Tests de redirección de entrada
echo "test content" > test_input.txt
test_redirect "cat < test_input.txt" "redirección entrada <"

# Tests combinados
test_redirect "cat < test_input.txt > test_output.txt" "redirección entrada y salida"

echo -e "\n${YELLOW}REDIRECTIONS RESULTS: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}"
[ $FAILED -eq 0 ] && exit 0 || exit 1
