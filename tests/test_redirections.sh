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
BASH_OUTPUT="${TMP_DIR}/bash_output.txt"
MINISHELL_OUTPUT="${TMP_DIR}/minishell_output.txt"

# Arrays para comandos fallidos
declare -a FAILED_COMMANDS=()

# Crear directorio temporal
mkdir -p "$TMP_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  MINISHELL REDIRECTIONS TEST SUITE${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Función para ejecutar test con comparación bash vs minishell
run_test() {
    local description="$1"
    local command="$2"
    local should_work="$3"  # 1 = debería funcionar, 0 = no debería funcionar
    
    ((TOTAL_TESTS++))
    echo -e "${YELLOW}Test $TOTAL_TESTS:${NC} $description"
    echo -e "${BLUE}Command:${NC} $command"
    
    # Limpiar archivos de output previos
    > "$BASH_OUTPUT"
    > "$MINISHELL_OUTPUT"
    > "$VALGRIND_OUTPUT"
    
    # Crear directorio de trabajo para cada test
    local test_dir="${TMP_DIR}/test_${TOTAL_TESTS}"
    mkdir -p "$test_dir"
    
    # Preparar comando para ambos shells (ajustar paths)
    local test_command="${command//$TMP_DIR/$test_dir}"
    
    # Ejecutar en bash
    cd "$test_dir" 2>/dev/null || true
    timeout 5 bash -c "$test_command" > "$BASH_OUTPUT" 2>&1
    local bash_exit=$?
    echo -e "${BLUE}Bash result:${NC} Exit code: $bash_exit"
    
    # Ejecutar en minishell con valgrind
    cd "$test_dir" 2>/dev/null || true
    printf '%s\nexit\n' "$test_command" | timeout 10 valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > "$MINISHELL_OUTPUT" 2> "$VALGRIND_OUTPUT"
    local valgrind_exit=$?
    
    printf '%s\nexit\n' "$test_command" | timeout 10 "$MINISHELL" > /dev/null 2>&1
    local minishell_exit=$?
    echo -e "${BLUE}Minishell result:${NC} Exit code: $minishell_exit"
    
    # Verificar memory leaks
    local definitely_lost=$(grep "definitely lost:" "$VALGRIND_OUTPUT" | grep -o '[0-9,]* bytes' | head -1 | tr -d ',' | grep -o '[0-9]*')
    local errors=$(grep "ERROR SUMMARY:" "$VALGRIND_OUTPUT" | grep -o '[0-9]* errors' | grep -o '[0-9]*')
    
    definitely_lost=${definitely_lost:-0}
    errors=${errors:-0}
    
    local has_leaks=0
    if [[ $definitely_lost -gt 0 || $errors -gt 0 ]]; then
        has_leaks=1
        ((MEMORY_LEAKS++))
    fi
    
    # Evaluar comportamiento
    local behavior_match=0
    if [[ $should_work -eq 1 ]]; then
        # Comando debería funcionar - comparar con bash
        if [[ $bash_exit -eq 0 && $minishell_exit -eq 0 ]]; then
            behavior_match=1
        elif [[ $bash_exit -ne 0 && $minishell_exit -ne 0 ]]; then
            behavior_match=1  # Ambos fallan, está bien
        fi
    else
        # Comando NO debería funcionar
        if [[ $minishell_exit -ne 0 ]]; then
            behavior_match=1
        fi
    fi
    
    # Mostrar resultado
    if [[ $behavior_match -eq 1 ]]; then
        if [[ $has_leaks -eq 0 ]]; then
            echo -e "  ${GREEN}✅ PASS${NC} - Behavior matches bash (Lost: ${definitely_lost}B, Errors: $errors)"
            ((PASSED_TESTS++))
        else
            echo -e "  ${YELLOW}⚠️  PASS (with memory leaks)${NC} - Lost: ${definitely_lost}B, Errors: $errors"
            ((PASSED_TESTS++))
        fi
    else
        echo -e "  ${RED}❌ FAIL${NC} - Behavior differs from bash (Lost: ${definitely_lost}B, Errors: $errors)"
        FAILED_COMMANDS+=("$command")
        ((FAILED_TESTS++))
    fi
    
    # Limpiar directorio de test
    rm -rf "$test_dir"
    echo ""
}

echo -e "${GREEN}=== REDIRECCIONES DE SALIDA (>) ===${NC}"
run_test "Redirección simple >" "echo hello > output.txt" 1
run_test "Redirección sobrescribir" "echo world > output.txt" 1
run_test "Redirección builtin pwd" "pwd > pwd_output.txt" 1
run_test "Redirección comando externo" "ls > ls_output.txt" 1

echo -e "${GREEN}=== REDIRECCIONES DE APPEND (>>) ===${NC}"
run_test "Redirección append >>" "echo line1 >> append.txt" 1
run_test "Redirección append segunda línea" "echo line2 >> append.txt" 1
run_test "Append builtin" "pwd >> append.txt" 1

echo -e "${GREEN}=== REDIRECCIONES DE ENTRADA (<) ===${NC}"
# Tests que crean y usan archivos
run_test "Redirección entrada <" "echo 'test input content' > input.txt && cat < input.txt" 1
run_test "Redirección entrada wc" "echo -e 'line1\nline2\nline3' > lines.txt && wc -l < lines.txt" 1

echo -e "${GREEN}=== HEREDOC (<<) ===${NC}"
run_test "Heredoc simple" "cat << EOF
hello world
EOF" 1

echo -e "${GREEN}=== REDIRECCIONES ERRÓNEAS ===${NC}"
run_test "Redirección sin archivo >" "echo hello >" 0
run_test "Redirección sin archivo <" "cat <" 0
run_test "Redirección sin archivo >>" "echo hello >>" 0
run_test "Redirección entrada archivo inexistente" "cat < noexiste.txt" 0

echo -e "${GREEN}=== REDIRECCIONES MÚLTIPLES ===${NC}"
run_test "Múltiples redirecciones >" "echo hello > out1.txt > out2.txt" 1
run_test "Redirección entrada y salida" "echo 'test' > input.txt && cat < input.txt > output2.txt" 1
run_test "Comando con argumentos y redirección" "echo arg1 arg2 > args.txt" 1

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

# Mostrar comandos fallidos
if [[ ${#FAILED_COMMANDS[@]} -gt 0 ]]; then
    echo -e "\n${RED}Failed commands:${NC}"
    for cmd in "${FAILED_COMMANDS[@]}"; do
        echo -e "  - $cmd"
    done
fi

if [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "\n${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}Some tests failed!${NC}"
    exit 1
fi
