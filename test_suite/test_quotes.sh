#!/bin/bash

# Pruebas para manejo de comillas
MINISHELL=${1:-"../Mini_2.0/minishell"}
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}📝 TESTING QUOTES${NC}"

# Función para probar comillas
test_quotes() {
    local cmd="$1"
    local expected="$2"
    local test_name="$3"
    
    echo -n "Testing: $test_name... "
    
    result=$(echo -e "$cmd\nexit" | $MINISHELL 2>/dev/null | grep -v "exit" | tail -n +2 | tr -d '\n')
    
    if [[ "$result" == *"$expected"* ]] || [[ -z "$expected" && -z "$result" ]]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Expected: '$expected'"
        echo "  Got: '$result'"
        ((FAILED++))
    fi
}

# Tests con comillas simples
test_quotes "echo 'hello world'" "hello world" "comillas simples básicas"
test_quotes "echo 'hello   world'" "hello   world" "comillas simples con espacios"
test_quotes "echo '\$HOME'" "\$HOME" "comillas simples no expanden variables"

# Tests con comillas dobles
test_quotes "echo \"hello world\"" "hello world" "comillas dobles básicas"
test_quotes "echo \"hello   world\"" "hello   world" "comillas dobles con espacios"
test_quotes "echo \"\$USER\"" "$USER" "comillas dobles expanden variables"

# Tests mixtos
test_quotes "echo 'hello' world" "hello world" "comillas simples + texto"
test_quotes "echo \"hello\" world" "hello world" "comillas dobles + texto"

# Tests anidados (si están implementados)
test_quotes "echo \"It's working\"" "It's working" "comillas dobles con apostrofe"
test_quotes "echo 'He said \"hello\"'" "He said \"hello\"" "comillas simples con dobles dentro"

echo -e "\n${YELLOW}QUOTES RESULTS: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}"
[ $FAILED -eq 0 ] && exit 0 || exit 1
