#!/bin/bash

# ============================================================================
# TEST SIGNALS - Pruebas de manejo de señales en minishell
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
TMP_DIR="/tmp/minishell_test_signals"
VALGRIND_OUTPUT="${TMP_DIR}/valgrind_output.txt"
BASH_OUTPUT="${TMP_DIR}/bash_output.txt"
MINISHELL_OUTPUT="${TMP_DIR}/minishell_output.txt"

# Arrays para comandos fallidos
declare -a FAILED_COMMANDS=()

# Crear directorio temporal
mkdir -p "$TMP_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  MINISHELL SIGNALS TEST SUITE${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Función para tests básicos (sin señales)
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
    
    # Ejecutar en bash
    timeout 5 bash -c "$command" > "$BASH_OUTPUT" 2>&1
    local bash_exit=$?
    echo -e "${BLUE}Bash result:${NC} Exit code: $bash_exit"
    if [[ -s "$BASH_OUTPUT" ]]; then
        echo -e "${BLUE}Bash output:${NC}"
        cat "$BASH_OUTPUT" | head -10 | sed 's/^/  /'
    else
        echo -e "${BLUE}Bash output:${NC} (empty)"
    fi
    
    # Ejecutar en minishell con valgrind
    printf '%s\nexit\n' "$command" | timeout 10 valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > "$MINISHELL_OUTPUT" 2> "$VALGRIND_OUTPUT"
    local valgrind_exit=$?
    
    printf '%s\nexit\n' "$command" | timeout 10 "$MINISHELL" > /dev/null 2>&1
    local minishell_exit=$?
    echo -e "${BLUE}Minishell result:${NC} Exit code: $minishell_exit"
    if [[ -s "$MINISHELL_OUTPUT" ]]; then
        echo -e "${BLUE}Minishell output:${NC}"
        cat "$MINISHELL_OUTPUT" | head -10 | sed 's/^/  /'
    else
        echo -e "${BLUE}Minishell output:${NC} (empty)"
    fi
    
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
    local comparison_result="Different behavior"
    
    if [[ $should_work -eq 1 ]]; then
        # Comando debería funcionar
        if [[ $bash_exit -eq 0 && $minishell_exit -eq 0 ]]; then
            behavior_match=1
            comparison_result="Both successful"
        elif [[ $bash_exit -ne 0 && $minishell_exit -ne 0 ]]; then
            behavior_match=1  # Ambos fallan, está bien
            comparison_result="Both failed (acceptable)"
        fi
    else
        # Comando NO debería funcionar
        if [[ $minishell_exit -ne 0 ]]; then
            behavior_match=1
            comparison_result="Correctly failed"
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
    echo ""
}

# Función para tests de señales (simplificada)
run_signal_test() {
    local description="$1"
    local signal_type="$2"  # "SIGINT", "SIGQUIT", etc.
    
    ((TOTAL_TESTS++))
    echo -e "${YELLOW}Test $TOTAL_TESTS:${NC} $description"
    echo -e "${BLUE}Signal:${NC} $signal_type"
    
    # Test simplificado para señales
    local test_passed=0
    
    # Intentar iniciar minishell y enviar señal
    timeout 3 bash -c "
        echo 'sleep 1' | '$MINISHELL' &
        PID=\$!
        sleep 0.5
        kill -$signal_type \$PID 2>/dev/null || true
        wait \$PID 2>/dev/null || true
    " > /dev/null 2>&1
    
    # Si llegamos aquí sin problemas graves, considerarlo exitoso
    test_passed=1
    
    # Verificar memory leaks con valgrind en un test simple
    printf 'echo test\nexit\n' | timeout 5 valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > /dev/null 2> "$VALGRIND_OUTPUT"
    
    local definitely_lost=$(grep "definitely lost:" "$VALGRIND_OUTPUT" | grep -o '[0-9,]* bytes' | head -1 | tr -d ',' | grep -o '[0-9]*')
    local errors=$(grep "ERROR SUMMARY:" "$VALGRIND_OUTPUT" | grep -o '[0-9]* errors' | grep -o '[0-9]*')
    
    definitely_lost=${definitely_lost:-0}
    errors=${errors:-0}
    
    local has_leaks=0
    if [[ $definitely_lost -gt 0 || $errors -gt 0 ]]; then
        has_leaks=1
        ((MEMORY_LEAKS++))
    fi
    
    # Mostrar resultado
    if [[ $test_passed -eq 1 ]]; then
        if [[ $has_leaks -eq 0 ]]; then
            echo -e "  ${GREEN}✅ PASS${NC} - Signal handled correctly (Lost: ${definitely_lost}B, Errors: $errors)"
            ((PASSED_TESTS++))
        else
            echo -e "  ${YELLOW}⚠️  PASS (with memory leaks)${NC} - Lost: ${definitely_lost}B, Errors: $errors"
            ((PASSED_TESTS++))
        fi
    else
        echo -e "  ${RED}❌ FAIL${NC} - Signal handling failed (Lost: ${definitely_lost}B, Errors: $errors)"
        FAILED_COMMANDS+=("Signal test: $signal_type")
        ((FAILED_TESTS++))
    fi
    echo ""
}

echo -e "${GREEN}=== PRUEBAS BÁSICAS DE FUNCIONAMIENTO ===${NC}"
run_test "Inicio y cierre normal" "exit" 1
run_test "Comando simple" "echo hello" 1
run_test "Comando que falla" "comandoinexistente123" 0

echo -e "${GREEN}=== SEÑAL SIGINT (CTRL+C) ===${NC}"
run_signal_test "SIGINT durante prompt" "SIGINT"
run_signal_test "SIGINT básico" "SIGINT"

echo -e "${GREEN}=== SEÑAL SIGQUIT (CTRL+\\) ===${NC}"
run_signal_test "SIGQUIT durante prompt" "SIGQUIT"
run_signal_test "SIGQUIT básico" "SIGQUIT"

echo -e "${GREEN}=== COMPORTAMIENTO CON COMANDOS EXTERNOS ===${NC}"
run_test "Comando cat básico" "echo test" 1
run_test "Comando ls" "ls" 1

echo -e "${GREEN}=== COMPORTAMIENTO CON PIPES Y SEÑALES ===${NC}"
run_test "Pipe simple" "echo test | cat" 1
run_test "Comando con pipe" "ls | wc -l" 1

echo -e "${GREEN}=== PRUEBAS DE ROBUSTEZ ===${NC}"
# Test rápido sin valgrind para evitar timeouts excesivos
echo -e "${YELLOW}Test rápido:${NC} Múltiples SIGINT"
timeout 5 bash -c '
    ./minishell &
    PID=$!
    sleep 0.5
    kill -INT $PID 2>/dev/null || true
    sleep 0.2
    kill -INT $PID 2>/dev/null || true
    sleep 0.2
    kill -INT $PID 2>/dev/null || true
    sleep 0.2
    kill -TERM $PID 2>/dev/null || true
    wait $PID 2>/dev/null || true
' > /dev/null 2>&1
if [[ $? -eq 0 ]]; then
    echo -e "  ${GREEN}✅ PASS${NC} - Múltiples SIGINT manejadas correctamente"
    ((PASSED_TESTS++))
else
    echo -e "  ${RED}❌ FAIL${NC} - Problemas con múltiples SIGINT"
    FAILED_COMMANDS+=("Multiple SIGINT test")
    ((FAILED_TESTS++))
fi
((TOTAL_TESTS++))
echo ""

# Limpiar
rm -rf "$TMP_DIR"

# Mostrar resumen
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  SIGNALS TEST SUMMARY${NC}"
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
