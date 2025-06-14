#!/bin/bash

# ============================================================================
# TEST REDIRECTIONS - Pruebas de redirecciones en minishell
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
TMP_DIR="/tmp/minishell_test_redirections"
VALGRIND_OUTPUT="${TMP_DIR}/valgrind_output.txt"

# Crear directorio temporal
mkdir -p "$TMP_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  MINISHELL REDIRECTIONS TEST SUITE${NC}"
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
    
    # Limpiar archivos de test previos
    rm -f "$TMP_DIR"/*.txt
    
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

echo -e "${GREEN}=== REDIRECCIONES DE SALIDA (>) ===${NC}"
run_test "Redirección simple >" "echo hello > $TMP_DIR/output.txt" 1
run_test "Redirección sobrescribir" "echo world > $TMP_DIR/output.txt" 1
run_test "Redirección builtin pwd" "pwd > $TMP_DIR/pwd_output.txt" 1
run_test "Redirección comando externo" "ls > $TMP_DIR/ls_output.txt" 1

echo -e "${GREEN}=== REDIRECCIONES DE APPEND (>>) ===${NC}"
run_test "Redirección append >>" "echo line1 >> $TMP_DIR/append.txt" 1
run_test "Redirección append segunda línea" "echo line2 >> $TMP_DIR/append.txt" 1
run_test "Append builtin" "pwd >> $TMP_DIR/append.txt" 1

echo -e "${GREEN}=== REDIRECCIONES DE ENTRADA (<) ===${NC}"
# Crear archivo de test
echo "test input content" > "$TMP_DIR/input.txt"
run_test "Redirección entrada <" "cat < $TMP_DIR/input.txt" 1
run_test "Redirección entrada grep" "grep test < $TMP_DIR/input.txt" 1
run_test "Redirección entrada wc" "wc -l < $TMP_DIR/input.txt" 1

echo -e "${GREEN}=== HEREDOC (<<) ===${NC}"
run_test "Heredoc simple" "cat << EOF
hello world
EOF" 1
run_test "Heredoc con variables" "cat << END
\$HOME
END" 1

echo -e "${GREEN}=== REDIRECCIONES ERRÓNEAS (NO DEBERÍAN FUNCIONAR) ===${NC}"
run_test "Redirección sin archivo >" "echo hello >" 0
run_test "Redirección sin archivo <" "cat <" 0
run_test "Redirección sin archivo >>" "echo hello >>" 0
run_test "Redirección a archivo sin permisos" "echo hello > /root/nopermissions.txt" 0
run_test "Redirección entrada archivo inexistente" "cat < $TMP_DIR/noexiste.txt" 0
run_test "Heredoc sin delimitador" "cat <<" 0

echo -e "${GREEN}=== REDIRECCIONES MÚLTIPLES ===${NC}"
run_test "Múltiples redirecciones >" "echo hello > $TMP_DIR/out1.txt > $TMP_DIR/out2.txt" 1
run_test "Redirección entrada y salida" "cat < $TMP_DIR/input.txt > $TMP_DIR/output2.txt" 1
run_test "Comando con argumentos y redirección" "echo arg1 arg2 > $TMP_DIR/args.txt" 1

# Limpiar
rm -rf "$TMP_DIR"

# Mostrar resumen
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  REDIRECTIONS TEST SUMMARY${NC}"
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
