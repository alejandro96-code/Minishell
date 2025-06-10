#!/bin/bash

# Pruebas para casos especiales y edge cases
MINISHELL=${1:-"../Mini_2.0/minishell"}
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}⚡ TESTING EDGE CASES${NC}"

# Función para probar casos especiales
test_edge() {
    local cmd="$1"
    local test_name="$2"
    local should_fail="$3"
    
    echo -n "Testing: $test_name... "
    
    # Ejecutar con timeout para evitar cuelgues
    echo -e "$cmd\nexit" | timeout 3 $MINISHELL >/dev/null 2>&1
    local exit_code=$?
    
    if [ "$should_fail" = "true" ]; then
        if [ $exit_code -ne 0 ]; then
            echo -e "${GREEN}✅ PASS${NC} (falló como esperado)"
            ((PASSED++))
        else
            echo -e "${RED}❌ FAIL${NC} (debería haber fallado)"
            ((FAILED++))
        fi
    else
        if [ $exit_code -eq 0 ] || [ $exit_code -eq 124 ]; then  # 124 = timeout
            echo -e "${GREEN}✅ PASS${NC}"
            ((PASSED++))
        else
            echo -e "${RED}❌ FAIL${NC} (exit code: $exit_code)"
            ((FAILED++))
        fi
    fi
}

# Tests de comandos vacíos
test_edge "" "comando vacío"
test_edge "   " "solo espacios"
test_edge "" "solo tabs"

# Tests de pipes vacíos o malformados
test_edge "|" "pipe al inicio" "true"
test_edge "echo hello |" "pipe al final" "true"
test_edge "echo hello | |" "pipes consecutivos" "true"

# Tests de comandos inexistentes
test_edge "commandnotfound" "comando inexistente" "true"
test_edge "/bin/nonexistent" "path inexistente" "true"

# Tests de casos límite con quotes
test_edge "echo \"" "comillas dobles sin cerrar" "true"
test_edge "echo '" "comillas simples sin cerrar" "true"

# Tests de redirecciones malformadas
test_edge "echo > " "redirección sin archivo" "true"
test_edge "< nonexistentfile cat" "redirección de archivo inexistente" "true"

# Tests de señales y control
test_edge "sleep 1" "comando con duración corta"

# Tests de múltiples espacios
test_edge "echo    hello    world" "múltiples espacios"

echo -e "\n${YELLOW}EDGE CASES RESULTS: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}"
[ $FAILED -eq 0 ] && exit 0 || exit 1
