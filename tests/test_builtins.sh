#!/bin/bash

# ============================================================================
# TEST BUILTINS - Pruebas de comandos built-in de minishell
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
TMP_DIR="/tmp/minishell_test_builtins"
VALGRIND_OUTPUT="${TMP_DIR}/valgrind_output.txt"

# Crear directorio temporal
mkdir -p "$TMP_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  MINISHELL BUILTINS TEST SUITE${NC}"
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
        # Comando debería funcionar (exit code 0 o 1 está bien para algunos builtins)
        if [[ $exit_code -eq 0 || $exit_code -eq 1 ]]; then
            test_passed=1
        fi
    else
        # Comando NO debería funcionar (exit code != 0)
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

echo -e "${GREEN}=== ECHO BUILTIN TESTS ===${NC}"
run_test "Echo simple" "echo hello world" 1
run_test "Echo con -n" "echo -n hello" 1
run_test "Echo con comillas" "echo \"hello world\"" 1
run_test "Echo con variables" "echo \$HOME" 1
run_test "Echo sin argumentos" "echo" 1
run_test "Echo con argumentos inválidos" "echo -x hello" 1

echo -e "${GREEN}=== PWD BUILTIN TESTS ===${NC}"
run_test "PWD simple" "pwd" 1
run_test "PWD con argumentos (no debería)" "pwd arg1 arg2" 1
run_test "PWD con flags inválidos" "pwd -x" 1

echo -e "${GREEN}=== ENV BUILTIN TESTS ===${NC}"
run_test "ENV simple" "env" 1
run_test "ENV con argumentos (no debería)" "env arg1" 1

echo -e "${GREEN}=== CD BUILTIN TESTS ===${NC}"
run_test "CD a directorio existente" "cd /" 1
run_test "CD a HOME" "cd" 1
run_test "CD a directorio inexistente" "cd /directorio/inexistente" 0
run_test "CD con demasiados argumentos" "cd / /tmp" 0
run_test "CD con .." "cd .." 1

echo -e "${GREEN}=== EXPORT BUILTIN TESTS ===${NC}"
run_test "Export variable válida" "export TEST_VAR=hello" 1
run_test "Export sin argumentos" "export" 1
run_test "Export variable inválida (número al inicio)" "export 123VAR=test" 0
run_test "Export variable inválida (caracteres especiales)" "export VAR-TEST=hello" 0
run_test "Export variable válida con underscore" "export TEST_VAR_2=world" 1

echo -e "${GREEN}=== UNSET BUILTIN TESTS ===${NC}"
run_test "Unset variable existente" "unset PATH" 1
run_test "Unset variable inexistente" "unset VARIABLE_INEXISTENTE" 1
run_test "Unset sin argumentos" "unset" 0
run_test "Unset variable inválida" "unset 123VAR" 0

echo -e "${GREEN}=== EXIT BUILTIN TESTS ===${NC}"
run_test "Exit sin argumentos" "exit" 1
run_test "Exit con número válido" "exit 42" 1
run_test "Exit con número inválido" "exit abc" 0
run_test "Exit con demasiados argumentos" "exit 1 2" 0

# Limpiar
rm -rf "$TMP_DIR"

# Mostrar resumen
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  BUILTINS TEST SUMMARY${NC}"
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
