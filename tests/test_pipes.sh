#!/bin/bash

# ============================================================================
# TEST PIPES - Pruebas de pipes en minishell
# ============================================================================

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Contadores
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
MEMORY_LEAKS=0

MINISHELL="./minishell"
TMP_DIR="/tmp/minishell_test_pipes"
VALGRIND_OUTPUT="${TMP_DIR}/valgrind_output.txt"

# Crear directorio temporal
mkdir -p "$TMP_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  MINISHELL PIPES TEST SUITE${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Función para ejecutar test con valgrind
run_test() {
    local description="$1"
    local command="$2"
    local should_work="$3"  # 1 = debería funcionar, 0 = no debería funcionar
    
    ((TOTAL_TESTS++))
    echo -e "${YELLOW}Test $TOTAL_TESTS:${NC} $description"
    echo -e "${BLUE}Command:${NC} $command"
    
    # Ejecutar con valgrind
    printf '%s\nexit\n' "$command" | timeout 10 valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > /dev/null 2> "$VALGRIND_OUTPUT"
    local valgrind_exit=$?
    
    # Analizar memory leaks
    local definitely_lost=$(grep "definitely lost:" "$VALGRIND_OUTPUT" | grep -o '[0-9,]* bytes' | head -1 | tr -d ',' | grep -o '[0-9]*')
    local errors=$(grep "ERROR SUMMARY:" "$VALGRIND_OUTPUT" | grep -o '[0-9]* errors' | grep -o '[0-9]*')
    
    # Defaults si no se encuentran
    definitely_lost=${definitely_lost:-0}
    errors=${errors:-0}
    
    # Verificar memory leaks
    local has_leaks=0
    if [[ $definitely_lost -gt 0 || $errors -gt 0 ]]; then
        has_leaks=1
        ((MEMORY_LEAKS++))
    fi
    
    # Ejecutar comando para verificar funcionalidad
    local exit_code
    printf '%s\nexit\n' "$command" | timeout 10 "$MINISHELL" > /dev/null 2>&1
    exit_code=$?
    
    # Evaluar resultado
    local test_passed=0
    if [[ $should_work -eq 1 ]]; then
        # Comando debería funcionar
        if [[ $exit_code -eq 0 ]]; then
            test_passed=1
        fi
    else
        # Comando NO debería funcionar
        if [[ $exit_code -ne 0 ]]; then
            test_passed=1
        fi
    fi
    
    # Mostrar resultado
    if [[ $test_passed -eq 1 ]]; then
        if [[ $has_leaks -eq 0 ]]; then
            echo -e "  ${GREEN}✅ PASS${NC} - No memory leaks (Lost: ${definitely_lost}B, Errors: $errors)"
            ((PASSED_TESTS++))
        else
            echo -e "  ${YELLOW}⚠️  PASS (with memory leaks)${NC} - Lost: ${definitely_lost}B, Errors: $errors"
            ((PASSED_TESTS++))
        fi
    else
        if [[ $has_leaks -eq 0 ]]; then
            echo -e "  ${RED}❌ FAIL${NC} - No memory leaks but wrong behavior"
            ((FAILED_TESTS++))
        else
            echo -e "  ${RED}❌ FAIL${NC} - Wrong behavior + Memory leaks (Lost: ${definitely_lost}B, Errors: $errors)"
            ((FAILED_TESTS++))
        fi
    fi
    echo ""
}

echo -e "${GREEN}=== PIPES FUNCIONALES ===${NC}"
run_test "Pipe simple echo cat" "echo hello | cat" 1
run_test "Pipe echo grep" "echo hello world | grep hello" 1
run_test "Pipe ls head" "ls | head -n 5" 1
run_test "Pipe cat sort" "echo -e \"c\\nb\\na\" | sort" 1
run_test "Pipe multiple" "echo hello | cat | cat" 1
run_test "Pipe con variables" "echo \$HOME | cat" 1
run_test "Pipe con comillas" "echo \"hello world\" | wc -w" 1

echo -e "${GREEN}=== PIPES CON BUILTINS ===${NC}"
run_test "Pipe builtin a externo" "pwd | cat" 1
run_test "Pipe externo a builtin" "echo test | cat" 1
run_test "Pipe env grep" "env | grep PATH" 1

echo -e "${GREEN}=== PIPES ERRÓNEOS (NO DEBERÍAN FUNCIONAR) ===${NC}"
run_test "Pipe sin comando después" "echo hello |" 0
run_test "Pipe sin comando antes" "| cat" 0
run_test "Pipe doble vacío" "echo hello || cat" 0
run_test "Pipe a comando inexistente" "echo hello | comandoinexistente123" 0
run_test "Pipe múltiple mal formado" "echo hello | | cat" 0

echo -e "${GREEN}=== PIPES COMPLEJOS ===${NC}"
run_test "Pipe con 3 comandos" "echo hello | cat | wc -c" 1
run_test "Pipe con 4 comandos" "echo hello | cat | cat | wc -w" 1
run_test "Pipe con argumentos múltiples" "echo hello world test | grep -o hello" 1

# Limpiar
rm -rf "$TMP_DIR"

# Mostrar resumen
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  PIPES TEST SUMMARY${NC}"
echo -e "${BLUE}======================================${NC}"
echo -e "Total tests: $TOTAL_TESTS"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
echo -e "${RED}Failed: $FAILED_TESTS${NC}"
echo -e "${YELLOW}Memory leaks: $MEMORY_LEAKS${NC}"

if [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi
