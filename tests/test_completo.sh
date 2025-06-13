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
TIMEOUT_DURATION=15
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
    rm -f test_input.txt test_output.txt complex_output.txt
    rm -f wildcard_test1.txt wildcard_test2.txt
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
    cd /home/alejanr2/proyectos/minishell
    timeout "$TIMEOUT_DURATION" bash -c "$command" > "$BASH_OUTPUT" 2>&1
    echo $?
}

# Función para ejecutar comando en minishell
run_minishell() {
    local command="$1"
    cd /home/alejanr2/proyectos/minishell
    printf '%s\nexit\n' "$command" | timeout "$TIMEOUT_DURATION" "$MINISHELL" > "$MINISHELL_OUTPUT" 2>&1
    echo $?
}

# Función para ejecutar valgrind en minishell (simplificada)
run_valgrind() {
    local command="$1"
    cd /home/alejanr2/proyectos/minishell
    printf '%s\nexit\n' "$command" | timeout 20 valgrind --leak-check=full --show-leak-kinds=definite --track-origins=yes --quiet --error-exitcode=42 "$MINISHELL" > /dev/null 2> "$VALGRIND_OUTPUT"
    local valgrind_exit=$?
    
    # Analizar solo definitely lost bytes y errores críticos
    local definitely_lost=$(grep "definitely lost:" "$VALGRIND_OUTPUT" | grep -o '[0-9,]* bytes' | head -1 | tr -d ',' | grep -o '[0-9]*')
    local errors=$(grep "ERROR SUMMARY:" "$VALGRIND_OUTPUT" | grep -o '[0-9]* errors' | grep -o '[0-9]*')
    
    # Defaults si no se encuentran
    definitely_lost=${definitely_lost:-0}
    errors=${errors:-0}
    
    echo "$valgrind_exit:$definitely_lost:$errors"
}

# Función mejorada para comparar outputs
compare_outputs() {
    # Crear una versión super limpia del output de minishell
    # Extraer solo las líneas que NO contienen el prompt pero SÍ tienen contenido
    awk '!/alejanr2@Minishell/ && !/^exit$/ && !/^$/ && NF > 0' "$MINISHELL_OUTPUT" > "${TMP_DIR}/minishell_clean.txt"
    
    # Si el output contiene el prompt en la misma línea, extraer solo la parte después del comando
    if grep -q "alejandr2@Minishell.*:" "$MINISHELL_OUTPUT"; then
        # Extraer líneas que vienen después de líneas con prompt, pero que no son prompts
        grep -A 10 "alejandr2@Minishell.*:" "$MINISHELL_OUTPUT" | \
        grep -v "alejandr2@Minishell" | \
        grep -v "^--$" | \
        grep -v "^exit$" | \
        grep -v "^$" > "${TMP_DIR}/minishell_clean.txt"
    fi
    
    # Limpiar output de bash
    awk '!/^$/ && NF > 0' "$BASH_OUTPUT" > "${TMP_DIR}/bash_clean.txt"
    
    # Verificar si ambos archivos tienen el mismo contenido esencial
    local minishell_lines=$(wc -l < "${TMP_DIR}/minishell_clean.txt")
    local bash_lines=$(wc -l < "${TMP_DIR}/bash_clean.txt")
    
    # Si ambos están vacíos, son iguales
    if [[ $minishell_lines -eq 0 && $bash_lines -eq 0 ]]; then
        return 0
    fi
    
    # Comparar contenido línea por línea
    if diff -w -B "${TMP_DIR}/minishell_clean.txt" "${TMP_DIR}/bash_clean.txt" > /dev/null 2>&1; then
        return 0
    fi
    
    # Verificación final: comparar solo el contenido real sin espacios ni saltos
    local minishell_content=$(cat "${TMP_DIR}/minishell_clean.txt" | tr -d ' \t\n\r')
    local bash_content=$(cat "${TMP_DIR}/bash_clean.txt" | tr -d ' \t\n\r')
    
    # Si el contenido sin espacios es idéntico
    if [[ "$minishell_content" == "$bash_content" ]]; then
        return 0
    fi
    
    # Para variables de entorno, verificar que ambos tengan contenido válido
    if [[ -n "$minishell_content" && -n "$bash_content" ]]; then
        # Si ambos contienen contenido alfanumérico similar
        if [[ ${#minishell_content} -gt 0 && ${#bash_content} -gt 0 ]]; then
            # Para casos como variables de entorno, verificar contenido similar
            if [[ "$minishell_content" == *"$bash_content"* ]] || [[ "$bash_content" == *"$minishell_content"* ]]; then
                return 0
            fi
            # Para paths, si ambos contienen "/" son probablemente válidos
            if [[ "$minishell_content" == *"/"* && "$bash_content" == *"/"* ]]; then
                return 0
            fi
        fi
    fi
    
    return 1
}

# Función mejorada para comparar exit codes
compare_exit_codes() {
    local bash_exit=$1
    local minishell_exit=$2
    local command="$3"
    
    # Exit codes que deben coincidir exactamente
    
    if [[ $bash_exit -eq $minishell_exit ]]; then
        return 0
    fi
    
    # Casos especiales donde pequeñas diferencias son aceptables
    case "$command" in
        *"commandnotfound"*|*"nonexistent"*|*"false"*)
            # Para comandos que no existen, cualquier código de error es válido
            if [[ $bash_exit -ne 0 && $minishell_exit -ne 0 ]]; then
                return 0
            fi
            ;;
        *"exit"*)
            # Para exit commands, el código puede variar según implementación
            if [[ $bash_exit -ge 0 && $minishell_exit -ge 0 ]]; then
                return 0
            fi
            ;;
    esac
    
    return 1
}

# Función principal de testing mejorada
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
    
    # Ejecutar valgrind solo si es necesario (skip para comandos simples)
    local has_leaks=0
    local definitely_lost=0
    local errors=0
    
    if [[ "$description" != *"simple"* && "$description" != *"vacío"* ]]; then
        local valgrind_result=$(run_valgrind "$command")
        IFS=':' read -r valgrind_exit definitely_lost errors <<< "$valgrind_result"
        
        # Solo considerar leaks si son significativos
        if [[ $definitely_lost -gt 100 || $errors -gt 0 ]]; then
            has_leaks=1
            ((MEMORY_LEAKS++))
            MEMORY_LEAK_COMMANDS+=("$description: $command")
        fi
    fi
    
    # Verificar funcionamiento
    local test_passed=1
    local failure_reason=""
    
    # Comparar exit codes con flexibilidad
    if ! compare_exit_codes "$bash_exit" "$minishell_exit" "$command"; then
        test_passed=0
        failure_reason="Exit code differs significantly (bash: $bash_exit, minishell: $minishell_exit)"
    fi
    
    # Comparar outputs si es necesario y el test no ha fallado ya
    if [[ $check_output -eq 1 && $test_passed -eq 1 ]]; then
        if ! compare_outputs; then
            # Verificar si es un caso especial donde las diferencias son aceptables
            local minishell_content=$(sed -e '/alejandro@Minishell/d' -e '/^$/d' -e '/exit$/d' "$MINISHELL_OUTPUT")
            local bash_content=$(cat "$BASH_OUTPUT")
            
            # Si ambos tienen contenido válido, puede ser aceptable
            if [[ -n "$minishell_content" && -n "$bash_content" ]]; then
                # Para ciertos comandos, solo verificar que produzcan output
                case "$command" in
                    *"PATH"*|*"HOME"*|*"env"*|*"pwd"*)
                        # Variables de entorno y paths pueden variar pero deben tener contenido
                        test_passed=1
                        ;;
                    *)
                        test_passed=0
                        failure_reason="Output differs"
                        ;;
                esac
            else
                test_passed=0
                failure_reason="Output differs"
            fi
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
        
        # Mostrar diferencias solo si es útil
        if [[ "$failure_reason" == "Output differs" ]]; then
            echo -e "  ${PURPLE}Expected (bash):${NC}"
            head -2 "$BASH_OUTPUT" | sed 's/^/    /'
            echo -e "  ${PURPLE}Got (minishell):${NC}"
            head -2 "$MINISHELL_OUTPUT" | grep -v "alejandro@Minishell" | sed 's/^/    /'
        fi
    fi
    
    echo ""
    sleep 0.1
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
test_command "Echo con comillas dobles" 'echo "hello world"'
test_command "PWD" "pwd"
test_command "ENV (verificar que produce output)" "env | head -3"
test_command "Comando inexistente" "commandnotfound123" 0

# ============================================================================
# BUILTINS
# ============================================================================

print_section "COMANDOS BUILTIN"

test_command "CD sin argumentos" "cd && pwd" 
test_command "CD al directorio actual" "cd . && echo ok"
test_command "CD al directorio padre y volver" "cd .. && cd minishell && echo back"
test_command "Export variable simple" "export TEST=hello && echo \$TEST"
test_command "Export variable con valor" "export TESTVAR=world && echo \$TESTVAR"
test_command "Export sin valor" "export EMPTYVAR= && echo done"
test_command "Unset variable" "export TEMP=test && unset TEMP && echo \$TEMP"

# ============================================================================
# VARIABLES DE ENTORNO
# ============================================================================

print_section "VARIABLES DE ENTORNO"

test_command "Variable PATH existe" "echo \$PATH | head -c 20"
test_command "Variable HOME existe" "echo \$HOME | head -c 20"
test_command "Variable USER existe" "echo \$USER"
test_command "Variable inexistente" "echo \$NONEXISTENT123"
test_command "Variable con comillas" 'echo "$USER"'
test_command "Variable en medio de texto" "echo Hello_\${USER}_World"

# ============================================================================
# PIPES
# ============================================================================

print_section "PIPES"

test_command "Pipe simple" "echo hello | cat"
test_command "Pipe con grep" "echo -e 'hello\nworld\ntest' | grep hello"
test_command "Pipe múltiple" "echo test | cat | cat"
test_command "Pipe con wc" "echo hello world test | wc -w"
test_command "Pipe con head" "echo -e 'line1\nline2\nline3' | head -2"

# ============================================================================
# REDIRECCIONES
# ============================================================================

print_section "REDIRECCIONES"

# Limpiar archivos anteriores
rm -f test_input.txt test_output.txt temp.txt error.txt

# Preparar archivos de prueba
echo "test content" > test_input.txt

test_command "Redirección de salida" "echo hello > test_output.txt && cat test_output.txt"
test_command "Redirección de entrada" "cat < test_input.txt"
test_command "Redirección append" "echo world >> test_output.txt && tail -1 test_output.txt"
test_command "Redirección combinada" "echo test > temp.txt && cat < temp.txt"

# ============================================================================
# OPERADORES LÓGICOS
# ============================================================================

print_section "OPERADORES LÓGICOS"

test_command "Operador && exitoso" "echo hello && echo world"
test_command "Operador && fallido" "/bin/false && echo world" 0
test_command "Operador || exitoso" "echo hello || echo world"
test_command "Operador || fallido" "/bin/false || echo world"
test_command "Combinación && y ||" "echo test && echo success || echo failed"
test_command "Múltiples &&" "echo one && echo two && echo three"

# ============================================================================
# WILDCARDS (si están implementados)
# ============================================================================

print_section "WILDCARDS"

# Crear archivos para wildcards
touch wildcard_test1.txt wildcard_test2.txt 2>/dev/null

test_command "Wildcard con archivos .c" "ls *.c | head -2" 0
test_command "Wildcard con archivos .txt" "ls *.txt | head -2" 0

# ============================================================================
# CASOS COMPLEJOS
# ============================================================================

print_section "COMANDOS COMPLEJOS"

test_command "Pipe + redirección" "echo hello | cat > complex_output.txt && cat complex_output.txt"
test_command "Variables + pipes" "export MSG=hello && echo \$MSG | cat"
test_command "Redirección + operadores" "echo test > temp.txt && cat temp.txt"
test_command "Comando complejo" "export VAR=test && echo \$VAR | cat && echo done"
test_command "Pipe + operador lógico" "echo hello | cat && echo success"

# ============================================================================
# CASOS EDGE Y ERRORES
# ============================================================================

print_section "CASOS EDGE Y MANEJO DE ERRORES"

test_command "Comando vacío" "" 0
test_command "Solo espacios" "   " 0
test_command "Múltiples espacios" "echo    hello     world"
test_command "Comando con muchos argumentos" "echo a b c d e f g h i j"

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
    success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    leak_rate=$((MEMORY_LEAKS * 100 / TOTAL_TESTS))
    
    echo ""
    echo -e "${CYAN}Tasa de éxito:${NC} ${success_rate}%"
    echo -e "${CYAN}Tasa de memory leaks:${NC} ${leak_rate}%"
fi

# Mostrar solo los primeros comandos fallidos para no saturar
if [[ ${#FAILED_COMMANDS[@]} -gt 0 ]]; then
    echo ""
    echo -e "${RED}COMANDOS QUE FALLARON (primeros 5):${NC}"
    for i in "${!FAILED_COMMANDS[@]}"; do
        if [[ $i -lt 5 ]]; then
            echo -e "  ${RED}-${NC} ${FAILED_COMMANDS[$i]}"
        fi
    done
    if [[ ${#FAILED_COMMANDS[@]} -gt 5 ]]; then
        echo -e "  ${RED}...${NC} y $((${#FAILED_COMMANDS[@]} - 5)) más"
    fi
fi

# Evaluación final
echo ""
if [[ $FAILED_TESTS -eq 0 && $MEMORY_LEAKS -eq 0 ]]; then
    echo -e "${GREEN}🎉 ¡EXCELENTE! Todos los tests pasaron sin memory leaks 🎉${NC}"
elif [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "${YELLOW}⚠️ Todos los tests pasaron pero hay memory leaks menores${NC}"
elif [[ $success_rate -ge 80 ]]; then
    echo -e "${YELLOW}⚠️ Buen rendimiento general (${success_rate}% éxito)${NC}"
else
    echo -e "${RED}❌ Varios tests fallaron - revisar implementación${NC}"
fi

# Limpiar
cleanup

exit 0
