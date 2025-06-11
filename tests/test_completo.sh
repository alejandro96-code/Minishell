#!/bin/bash

# ============================================================================
# MINISHELL COMPREHENSIVE TEST SUITE
# Compara resultados con bash y verifica memory leaks con valgrind
# ============================================================================

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Contadores
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
MEMORY_LEAKS=0

# Arrays para comandos fallidos
declare -a FAILED_COMMANDS
declare -a MEMORY_LEAK_COMMANDS

# Configuración
MINISHELL="./minishell"
TIMEOUT_DURATION=10
TMP_DIR="/tmp/minishell_test"
BASH_OUTPUT="${TMP_DIR}/bash_output.txt"
MINISHELL_OUTPUT="${TMP_DIR}/minishell_output.txt"
VALGRIND_OUTPUT="${TMP_DIR}/valgrind_output.txt"

# Crear directorio temporal
mkdir -p "$TMP_DIR"

# Función para limpiar archivos temporales
cleanup() {
    rm -rf "$TMP_DIR"
    rm -f test_file*.txt temp.txt output.txt error.txt
    rm -f /tmp/test_redirect.txt /tmp/minishell_test.txt
}

# Función para mostrar encabezados de sección
print_section() {
    echo ""
    echo -e "${CYAN}======================================${NC}"
    echo -e "${WHITE}  $1${NC}"
    echo -e "${CYAN}======================================${NC}"
    echo ""
}

# Función para ejecutar comando en bash
run_bash() {
    local command="$1"
    timeout "$TIMEOUT_DURATION" bash -c "$command" > "$BASH_OUTPUT" 2>&1
    echo $?
}

# Función para ejecutar comando en minishell
run_minishell() {
    local command="$1"
    printf '%s\nexit\n' "$command" | timeout "$TIMEOUT_DURATION" "$MINISHELL" > "$MINISHELL_OUTPUT" 2>&1
    echo $?
}

# Función para ejecutar valgrind en minishell
run_valgrind() {
    local command="$1"
    printf '%s\nexit\n' "$command" | timeout 20 valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > /dev/null 2> "$VALGRIND_OUTPUT"
    local valgrind_exit=$?
    
    # Analizar output de valgrind
    local definitely_lost=$(grep "definitely lost:" "$VALGRIND_OUTPUT" | grep -o '[0-9,]* bytes' | head -1 | tr -d ',' | grep -o '[0-9]*')
    local possibly_lost=$(grep "possibly lost:" "$VALGRIND_OUTPUT" | grep -o '[0-9,]* bytes' | head -1 | tr -d ',' | grep -o '[0-9]*')
    local errors=$(grep "ERROR SUMMARY:" "$VALGRIND_OUTPUT" | grep -o '[0-9]* errors' | grep -o '[0-9]*')
    
    # Defaults si no se encuentran
    definitely_lost=${definitely_lost:-0}
    possibly_lost=${possibly_lost:-0}
    errors=${errors:-0}
    
    echo "$valgrind_exit:$definitely_lost:$possibly_lost:$errors"
}

# Función para comparar outputs (ignorando prompts)
compare_outputs() {
    # Filtrar prompts y líneas vacías del output de minishell
    grep -v "alejandro@Minishell" "$MINISHELL_OUTPUT" | grep -v "^$" > "${TMP_DIR}/minishell_clean.txt"
    grep -v "^$" "$BASH_OUTPUT" > "${TMP_DIR}/bash_clean.txt"
    
    # Comparar contenido relevante
    if diff -q "${TMP_DIR}/minishell_clean.txt" "${TMP_DIR}/bash_clean.txt" > /dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# Función principal de testing
test_command() {
    local description="$1"
    local command="$2"
    local check_output="${3:-1}"  # 1=comparar output, 0=solo exit code
    
    ((TOTAL_TESTS++))
    echo -e "${BLUE}Test $TOTAL_TESTS:${NC} $description"
    echo -e "${YELLOW}Command:${NC} $command"
    
    # Ejecutar en bash
    local bash_exit=$(run_bash "$command")
    
    # Ejecutar en minishell
    local minishell_exit=$(run_minishell "$command")
    
    # Ejecutar valgrind
    local valgrind_result=$(run_valgrind "$command")
    IFS=':' read -r valgrind_exit definitely_lost possibly_lost errors <<< "$valgrind_result"
    
    # Verificar memory leaks
    local has_leaks=0
    if [[ $definitely_lost -gt 0 || $errors -gt 0 ]]; then
        has_leaks=1
        ((MEMORY_LEAKS++))
        MEMORY_LEAK_COMMANDS+=("$description: $command")
    fi
    
    # Verificar funcionamiento
    local test_passed=1
    local failure_reason=""
    
    # Comparar exit codes (con tolerancia para algunos casos)
    if [[ $bash_exit -ne $minishell_exit ]]; then
        # Algunos comandos pueden tener diferentes exit codes pero ser funcionalmente correctos
        if [[ ! ($bash_exit -eq 127 && $minishell_exit -eq 127) ]]; then
            test_passed=0
            failure_reason="Exit code differs (bash: $bash_exit, minishell: $minishell_exit)"
        fi
    fi
    
    # Comparar outputs si es necesario
    if [[ $check_output -eq 1 && $test_passed -eq 1 ]]; then
        if ! compare_outputs; then
            test_passed=0
            failure_reason="Output differs"
        fi
    fi
    
    # Mostrar resultados
    if [[ $test_passed -eq 1 ]]; then
        if [[ $has_leaks -eq 0 ]]; then
            echo -e "  ${GREEN}✅ PASS${NC} - No memory leaks"
            ((PASSED_TESTS++))
        else
            echo -e "  ${YELLOW}⚠️  PASS (with memory leaks)${NC} - Lost: ${definitely_lost}B, Errors: $errors"
            ((PASSED_TESTS++))
        fi
    else
        if [[ $has_leaks -eq 0 ]]; then
            echo -e "  ${RED}❌ FAIL${NC} - $failure_reason (No memory leaks)"
        else
            echo -e "  ${RED}❌ FAIL${NC} - $failure_reason (Memory leaks: ${definitely_lost}B lost, $errors errors)"
        fi
        ((FAILED_TESTS++))
        FAILED_COMMANDS+=("$description: $command - $failure_reason")
        
        # Mostrar diferencias si es útil
        if [[ "$failure_reason" == "Output differs" ]]; then
            echo -e "  ${PURPLE}Expected (bash):${NC}"
            head -3 "$BASH_OUTPUT" | sed 's/^/    /'
            echo -e "  ${PURPLE}Got (minishell):${NC}"
            head -3 "${TMP_DIR}/minishell_clean.txt" | sed 's/^/    /'
        fi
    fi
    
    echo ""
    sleep 0.1  # Pequeña pausa para legibilidad
}

# ============================================================================
# INICIO DE TESTS
# ============================================================================

echo -e "${WHITE}========================================${NC}"
echo -e "${WHITE}     MINISHELL COMPREHENSIVE TESTS     ${NC}"
echo -e "${WHITE}========================================${NC}"
echo ""
echo -e "${CYAN}Comparando resultados con bash y verificando memory leaks...${NC}"
echo ""

# Verificar que minishell existe
if [[ ! -f "$MINISHELL" ]]; then
    echo -e "${RED}Error: $MINISHELL no encontrado${NC}"
    echo "Ejecuta 'make' para compilar el proyecto"
    exit 1
fi

# ============================================================================
# COMANDOS BÁSICOS
# ============================================================================

print_section "COMANDOS BÁSICOS"

test_command "Echo simple" "echo hello world"
test_command "Echo con salto de línea" "echo -n hello"
test_command "Echo vacío" "echo"
test_command "Echo con comillas simples" "echo 'hello world'"
test_command "Echo con comillas dobles" "echo \"hello world\""
test_command "PWD" "pwd"
test_command "ENV (primeras líneas)" "env | head -5"
test_command "Comando inexistente" "commandnotfound" 0

# ============================================================================
# BUILTINS
# ============================================================================

print_section "COMANDOS BUILTIN"

test_command "CD sin argumentos" "cd" 0
test_command "CD al directorio actual" "cd ." 0
test_command "CD al directorio padre" "cd .." 0
test_command "Export variable simple" "export TEST=hello" 0
test_command "Export y echo variable" "export TESTVAR=world && echo \$TESTVAR"
test_command "Export sin valor" "export EMPTYVAR=" 0
test_command "Unset variable" "export TEMP=test && unset TEMP && echo \$TEMP"
test_command "Exit con código 0" "exit 0" 0
test_command "Exit con código personalizado" "exit 42" 0

# ============================================================================
# VARIABLES DE ENTORNO
# ============================================================================

print_section "VARIABLES DE ENTORNO"

test_command "Variable PATH" "echo \$PATH | head -c 50"
test_command "Variable HOME" "echo \$HOME"
test_command "Variable inexistente" "echo \$NONEXISTENT"
test_command "Variable con comillas" "echo \"\$USER\""
test_command "Múltiples variables" "echo \$USER \$HOME | head -c 50"
test_command "Variable en medio de texto" "echo Hello_\${USER}_World"

# ============================================================================
# PIPES
# ============================================================================

print_section "PIPES"

test_command "Pipe simple" "echo hello | cat"
test_command "Pipe con grep" "echo -e 'hello\\nworld\\ntest' | grep hello"
test_command "Pipe múltiple" "echo test | cat | cat"
test_command "Pipe con wc" "echo hello world | wc -w"
test_command "Pipe con head" "env | head -3"
test_command "Pipe complejo" "echo -e 'one\\ntwo\\nthree' | grep -v two | wc -l"

# ============================================================================
# REDIRECCIONES
# ============================================================================

print_section "REDIRECCIONES"

# Preparar archivos de prueba
echo "test content" > test_input.txt

test_command "Redirección de salida" "echo hello > test_output.txt && cat test_output.txt"
test_command "Redirección de entrada" "cat < test_input.txt"
test_command "Redirección append" "echo world >> test_output.txt && cat test_output.txt"
test_command "Redirección de error" "ls nonexistent 2> error.txt" 0
test_command "Redirección combinada" "echo test > temp.txt && cat < temp.txt"

# ============================================================================
# OPERADORES LÓGICOS
# ============================================================================

print_section "OPERADORES LÓGICOS"

test_command "Operador && exitoso" "echo hello && echo world"
test_command "Operador && fallido" "false && echo world" 0
test_command "Operador || exitoso" "echo hello || echo world"
test_command "Operador || fallido" "false || echo world"
test_command "Combinación && y ||" "echo test && echo success || echo failed"
test_command "Múltiples &&" "echo one && echo two && echo three"

# ============================================================================
# WILDCARDS
# ============================================================================

print_section "WILDCARDS"

# Crear archivos para wildcards
touch wildcard_test1.txt wildcard_test2.txt

test_command "Wildcard asterisco" "ls wildcard_*.txt"
test_command "Wildcard interrogación" "ls wildcard_test?.txt"
test_command "Wildcard en directorio" "ls *.c | head -3"

# ============================================================================
# CASOS COMPLEJOS
# ============================================================================

print_section "COMANDOS COMPLEJOS"

test_command "Pipe + redirección" "echo hello | cat > complex_output.txt && cat complex_output.txt"
test_command "Variables + pipes" "export MSG=hello && echo \$MSG | cat"
test_command "Redirección + operadores" "echo test > temp.txt && cat temp.txt || echo failed"
test_command "Comando complejo" "export VAR=test && echo \$VAR | cat && echo done"
test_command "Pipe + operador lógico" "echo hello | cat && echo success"

# ============================================================================
# CASOS EDGE Y ERRORES
# ============================================================================

print_section "CASOS EDGE Y MANEJO DE ERRORES"

test_command "Comando vacío" "" 0
test_command "Solo espacios" "   " 0
test_command "Múltiples espacios" "echo    hello     world"
test_command "Pipe sin comando" "echo hello |" 0
test_command "Redirección sin archivo" "echo hello >" 0
test_command "Operador && sin comando" "echo hello &&" 0
test_command "Paréntesis sin cerrar" "(echo hello" 0
test_command "Comillas sin cerrar" "echo 'hello" 0

# ============================================================================
# HEREDOC (si está implementado)
# ============================================================================

print_section "HEREDOC"

test_command "Heredoc simple" "cat << EOF
line1
line2
EOF"

# ============================================================================
# TESTS DE RENDIMIENTO
# ============================================================================

print_section "TESTS DE RENDIMIENTO"

test_command "Comando largo" "echo $(printf 'a%.0s' {1..100})" 0
test_command "Muchas variables" "export A=1 && export B=2 && export C=3 && echo \$A\$B\$C"
test_command "Pipe largo" "echo hello | cat | cat | cat | cat"

# ============================================================================
# RESULTADOS FINALES
# ============================================================================

print_section "RESULTADOS FINALES"

echo -e "${WHITE}Tests ejecutados:${NC} $TOTAL_TESTS"
echo -e "${GREEN}Tests pasados:${NC} $PASSED_TESTS"
echo -e "${RED}Tests fallidos:${NC} $FAILED_TESTS"
echo -e "${YELLOW}Tests con memory leaks:${NC} $MEMORY_LEAKS"

# Calcular porcentajes
if [[ $TOTAL_TESTS -gt 0 ]]; then
    local success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    local leak_rate=$((MEMORY_LEAKS * 100 / TOTAL_TESTS))
    
    echo ""
    echo -e "${CYAN}Tasa de éxito:${NC} ${success_rate}%"
    echo -e "${CYAN}Tasa de memory leaks:${NC} ${leak_rate}%"
fi

# Mostrar comandos fallidos
if [[ ${#FAILED_COMMANDS[@]} -gt 0 ]]; then
    echo ""
    echo -e "${RED}COMANDOS QUE FALLARON:${NC}"
    for cmd in "${FAILED_COMMANDS[@]}"; do
        echo -e "  ${RED}-${NC} $cmd"
    done
fi

# Mostrar comandos con memory leaks
if [[ ${#MEMORY_LEAK_COMMANDS[@]} -gt 0 ]]; then
    echo ""
    echo -e "${YELLOW}COMANDOS CON MEMORY LEAKS:${NC}"
    for cmd in "${MEMORY_LEAK_COMMANDS[@]}"; do
        echo -e "  ${YELLOW}-${NC} $cmd"
    done
fi

# Evaluación final
echo ""
if [[ $FAILED_TESTS -eq 0 && $MEMORY_LEAKS -eq 0 ]]; then
    echo -e "${GREEN}🎉 ¡EXCELENTE! Todos los tests pasaron sin memory leaks 🎉${NC}"
elif [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "${YELLOW}⚠️ Todos los tests pasaron pero hay memory leaks${NC}"
elif [[ $MEMORY_LEAKS -eq 0 ]]; then
    echo -e "${YELLOW}⚠️ Sin memory leaks pero algunos tests fallaron${NC}"
else
    echo -e "${RED}❌ Hay tests fallidos y memory leaks que necesitan atención${NC}"
fi

# Limpiar
cleanup

exit 0
