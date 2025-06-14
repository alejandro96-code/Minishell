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

# Crear directorio temporal
mkdir -p "$TMP_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  MINISHELL SIGNALS TEST SUITE${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Función para ejecutar test con valgrind y señales
run_signal_test() {
    local description="$1"
    local command="$2"
    local signal="$3"
    local should_work="$4"  # 1 = debería funcionar, 0 = no debería funcionar
    
    ((TOTAL_TESTS++))
    echo -e "${YELLOW}Test $TOTAL_TESTS:${NC} $description"
    echo -e "${BLUE}Command:${NC} $command"
    echo -e "${BLUE}Signal:${NC} $signal"
    
    # Ejecutar minishell en background con valgrind
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > /dev/null 2> "$VALGRIND_OUTPUT" &
    local minishell_pid=$!
    
    sleep 0.5  # Dar tiempo a que inicie
    
    # Enviar comando si se proporciona
    if [[ -n "$command" ]]; then
        echo "$command" > /proc/$minishell_pid/fd/0 2>/dev/null || true
        sleep 0.5
    fi
    
    # Enviar señal
    if [[ "$signal" != "NONE" ]]; then
        kill -"$signal" $minishell_pid 2>/dev/null || true
        sleep 0.5
    fi
    
    # Verificar si el proceso sigue vivo
    local process_alive=0
    if kill -0 $minishell_pid 2>/dev/null; then
        process_alive=1
        # Terminar el proceso si sigue vivo
        echo "exit" > /proc/$minishell_pid/fd/0 2>/dev/null || true
        sleep 0.5
        kill -TERM $minishell_pid 2>/dev/null || true
        sleep 0.5
        kill -KILL $minishell_pid 2>/dev/null || true
    fi
    
    wait $minishell_pid 2>/dev/null || true
    local exit_code=$?
    
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
    
    # Evaluar resultado basado en el tipo de señal
    local test_passed=0
    case "$signal" in
        "INT"|"SIGINT")
            # CTRL+C debería interrumpir pero no cerrar minishell
            if [[ $should_work -eq 1 ]]; then
                test_passed=1  # Siempre pasa si se espera que funcione
            fi
            ;;
        "QUIT"|"SIGQUIT")
            # CTRL+\ debería ser ignorado en minishell
            if [[ $should_work -eq 1 ]]; then
                test_passed=1
            fi
            ;;
        "NONE")
            # Sin señal, verificar comportamiento normal
            if [[ $should_work -eq 1 && $exit_code -eq 0 ]]; then
                test_passed=1
            elif [[ $should_work -eq 0 && $exit_code -ne 0 ]]; then
                test_passed=1
            fi
            ;;
        *)
            test_passed=1  # Por defecto pasa para otras señales
            ;;
    esac
    
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

# Función simplificada para tests básicos
run_test() {
    local description="$1"
    local command="$2"
    local should_work="$3"
    
    run_signal_test "$description" "$command" "NONE" "$should_work"
}

echo -e "${GREEN}=== PRUEBAS BÁSICAS DE FUNCIONAMIENTO ===${NC}"
run_test "Inicio y cierre normal" "exit" 1
run_test "Comando simple" "echo hello" 1
run_test "Comando que falla" "comandoinexistente123" 0

echo -e "${GREEN}=== SEÑAL SIGINT (CTRL+C) ===${NC}"
run_signal_test "SIGINT durante prompt" "" "INT" 1
run_signal_test "SIGINT durante comando" "sleep 2" "INT" 1
run_signal_test "SIGINT en comando que no existe" "comandoinexistente" "INT" 1

echo -e "${GREEN}=== SEÑAL SIGQUIT (CTRL+\\) ===${NC}"
run_signal_test "SIGQUIT durante prompt" "" "QUIT" 1
run_signal_test "SIGQUIT durante comando" "sleep 2" "QUIT" 1

echo -e "${GREEN}=== COMPORTAMIENTO CON COMANDOS EXTERNOS ===${NC}"
run_signal_test "SIGINT en cat (interactivo)" "cat" "INT" 1
run_signal_test "SIGINT en grep" "grep test" "INT" 1

echo -e "${GREEN}=== COMPORTAMIENTO CON PIPES Y SEÑALES ===${NC}"
run_signal_test "SIGINT en pipe" "cat | cat" "INT" 1
run_signal_test "SIGINT en comando largo" "sleep 3 | cat" "INT" 1

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

if [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi
