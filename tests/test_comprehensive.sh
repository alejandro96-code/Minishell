#!/bin/bash

# Test completo para minishell con comandos simples y complejos
# Creado por alejandro para probar todas las funcionalidades implementadas

MINISHELL="./minishell"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
MAGENTA="\033[35m"
CYAN="\033[36m"
NC="\033[0m" # No Color
BOLD="\033[1m"

PASSED=0
FAILED=0
TOTAL=0
FAILED_COMMANDS=()

# Función para imprimir encabezados de sección
print_section() {
    echo -e "\n${BOLD}${BLUE}======================================${NC}"
    echo -e "${BOLD}${BLUE}  $1${NC}"
    echo -e "${BOLD}${BLUE}======================================${NC}"
}

# Función para ejecutar y verificar comandos
test_command() {
    local description="$1"
    local command="$2"
    local expected_exit_code="${3:-0}"
    local check_output="${4:-}"
    
    ((TOTAL++))
    echo -n "Testing: $description... "
    
    # Crear archivo temporal para capturar salida
    local tmp_file=$(mktemp)
    local exit_code
    
    # Ejecutar comando con timeout
    printf '%s\nexit\n' "$command" | timeout 10 $MINISHELL > "$tmp_file" 2>&1
    exit_code=$?
    
    # Verificar código de salida
    if [ $exit_code -eq $expected_exit_code ]; then
        if [ -n "$check_output" ]; then
            # Verificar contenido específico en la salida
            if grep -q "$check_output" "$tmp_file"; then
                echo -e "${GREEN}✅ PASS${NC}"
                ((PASSED++))
            else
                echo -e "${RED}❌ FAIL${NC} - Output doesn't contain: $check_output"
                echo "  Got output:"
                head -3 "$tmp_file" | sed 's/^/    /'
                FAILED_COMMANDS+=("$description: $command")
                ((FAILED++))
            fi
        else
            echo -e "${GREEN}✅ PASS${NC}"
            ((PASSED++))
        fi
    else
        echo -e "${RED}❌ FAIL${NC} - Exit code: $exit_code (expected: $expected_exit_code)"
        FAILED_COMMANDS+=("$description: $command")
        ((FAILED++))
    fi
    
    rm -f "$tmp_file"
}

# Función para tests más complejos con validación personalizada
test_complex() {
    local description="$1"
    local command="$2"
    local validation_func="$3"
    
    ((TOTAL++))
    echo -n "Testing: $description... "
    
    local tmp_file=$(mktemp)
    printf '%s\nexit\n' "$command" | timeout 10 $MINISHELL > "$tmp_file" 2>&1
    
    if $validation_func "$tmp_file"; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Output:"
        head -3 "$tmp_file" | sed 's/^/    /'
        FAILED_COMMANDS+=("$description: $command")
        ((FAILED++))
    fi
    
    rm -f "$tmp_file"
}

# Validaciones personalizadas
validate_logical_and() {
    local file="$1"
    grep -v "alejandro@Minishell" "$file" | grep -q "hello" && grep -v "alejandro@Minishell" "$file" | grep -q "world"
}

validate_logical_or_success() {
    local file="$1"
    # Buscar solo en líneas que no sean prompts (que no contengan "alejandro@Minishell")
    grep -v "alejandro@Minishell" "$file" | grep -q "first" && ! grep -v "alejandro@Minishell" "$file" | grep -q "second"
}

validate_logical_or_failure() {
    local file="$1"
    ! grep -v "alejandro@Minishell" "$file" | grep -q "first" && grep -v "alejandro@Minishell" "$file" | grep -q "second"
}

validate_pipe_output() {
    local file="$1"
    # Debe mostrar solo las líneas que contienen la palabra buscada
    local count=$(grep -v "alejandro@Minishell" "$file" | grep -c "USER" 2>/dev/null || echo "0")
    [ "$count" -gt 0 ]
}

validate_parentheses() {
    local file="$1"
    # (true && echo hello) || echo world debe mostrar "hello" pero no "world"
    grep -v "alejandro@Minishell" "$file" | grep -q "hello" && ! grep -v "alejandro@Minishell" "$file" | grep -q "world"
}

validate_export_variable() {
    local file="$1"
    grep -v "alejandro@Minishell" "$file" | grep -q "testvalue"
}

validate_redirection() {
    [ -f "test_redirect.txt" ] && grep -q "redirect test" "test_redirect.txt"
}

validate_heredoc() {
    local file="$1"
    grep -v "alejandro@Minishell" "$file" | grep -q "line1" && grep -v "alejandro@Minishell" "$file" | grep -q "line2"
}

# ====================================
# TESTS BÁSICOS DE COMANDOS SIMPLES
# ====================================

print_section "COMANDOS BÁSICOS"

test_command "comando echo simple" "echo hello world" 0 "hello world"
test_command "echo con flag -n" "echo -n hello" 0
test_command "comando pwd" "pwd" 0
test_command "comando env (verificar que existe)" "env | head -1" 0

print_section "BUILTINS"

test_command "cd a directorio home" "cd && pwd" 0
test_command "cd a directorio inexistente" "cd /directorio/inexistente" 1
test_command "export nueva variable" "export TESTVAR=hello && echo \$TESTVAR" 0 "hello"
test_command "unset variable" "export TEMP=test && unset TEMP && echo \$TEMP" 0
test_command "exit con código" "exit 42" 42

# ====================================
# OPERADORES LÓGICOS SIMPLES
# ====================================

print_section "OPERADORES LÓGICOS BÁSICOS"

test_complex "operador && (éxito + éxito)" "echo hello && echo world" validate_logical_and
test_complex "operador || (éxito, no ejecuta segundo)" "echo first || echo second" validate_logical_or_success
test_complex "operador || (fallo, ejecuta segundo)" "false || echo second" validate_logical_or_failure

# ====================================
# OPERADORES LÓGICOS COMPLEJOS
# ====================================

print_section "OPERADORES LÓGICOS COMPLEJOS"

test_command "cadena de && exitosos" "echo a && echo b && echo c" 0 "c"
test_command "cadena de || con primer éxito" "true || echo no || echo tampoco" 0
test_command "mezcla && y ||" "echo hello && false || echo world" 0 "world"
test_complex "paréntesis con &&" "(echo hello && echo there) || echo world" validate_parentheses

# ====================================
# PIPES
# ====================================

print_section "PIPES"

test_command "pipe simple" "echo hello world | cat" 0 "hello world"
test_complex "pipe con grep" "env | grep USER" validate_pipe_output
test_command "pipe múltiple" "echo hello | cat | cat" 0 "hello"
test_command "pipe con wc" "echo -e 'line1\\nline2\\nline3' | wc -l" 0 "3"

# ====================================
# REDIRECCIONES
# ====================================

print_section "REDIRECCIONES"

test_command "redirección de salida >" "echo 'redirect test' > test_redirect.txt" 0
test_complex "verificar archivo creado" "cat test_redirect.txt" validate_redirection
test_command "redirección append >>" "echo 'append test' >> test_redirect.txt" 0
test_command "redirección de entrada <" "cat < test_redirect.txt" 0 "redirect test"

# Heredoc básico
test_complex "heredoc simple" "cat << EOF\nline1\nline2\nEOF" validate_heredoc

# ====================================
# VARIABLES Y EXPANSIÓN
# ====================================

print_section "VARIABLES Y EXPANSIÓN"

test_complex "export y uso de variable" "export MYTEST=testvalue && echo \$MYTEST" validate_export_variable
test_command "variable de entorno existente" "echo \$HOME" 0
test_command "variable inexistente" "echo \$NONEXISTENT" 0
test_command "múltiples variables" "export A=1 && export B=2 && echo \$A\$B" 0 "12"

# ====================================
# WILDCARDS
# ====================================

print_section "WILDCARDS"

# Crear archivos para probar wildcards
test_command "preparar archivos para wildcards" "touch test_file1.txt test_file2.txt" 0
test_command "wildcard simple *" "ls test_file*.txt" 0 "test_file1.txt"
test_command "wildcard con ?" "ls test_file?.txt" 0
test_command "limpiar archivos de prueba" "rm -f test_file*.txt test_redirect.txt" 0

# ====================================
# COMANDOS COMPLEJOS COMBINADOS
# ====================================

print_section "COMANDOS COMPLEJOS COMBINADOS"

test_command "pipe + operador lógico" "echo hello | cat && echo world" 0 "world"
test_command "redirección + operador lógico" "echo test > temp.txt && cat temp.txt" 0 "test"
test_command "paréntesis + pipes" "(echo hello && echo world) | grep hello" 0 "hello"
test_command "variables + operadores" "export X=hello && echo \$X && echo world" 0 "world"

# Comando súper complejo
test_command "comando muy complejo" "export TEST=value && (echo \$TEST | cat) && echo success || echo failed" 0 "success"

# ====================================
# CASOS EDGE
# ====================================

print_section "CASOS EDGE Y ERRORES"

test_command "comando inexistente" "comandoinexistente" 127
test_command "sintaxis pipe incorrecta" "echo hello |" 1
test_command "paréntesis sin cerrar" "(echo hello" 1
test_command "múltiples espacios" "echo    hello     world" 0 "hello"
test_command "comando vacío entre &&" "echo hello && && echo world" 1

# ====================================
# TESTS DE RENDIMIENTO BÁSICO
# ====================================

print_section "TESTS DE RENDIMIENTO"

test_command "muchos pipes" "echo hello | cat | cat | cat | cat | cat" 0 "hello"
test_command "muchos operadores &&" "true && true && true && true && echo final" 0 "final"
test_command "comando largo" "echo very long command with many words to test buffer handling" 0 "buffer"

# ====================================
# LIMPIEZA
# ====================================

print_section "LIMPIEZA"
rm -f temp.txt test_redirect.txt test_*.txt

# ====================================
# RESULTADOS FINALES
# ====================================

print_section "RESULTADOS FINALES"

echo -e "${BOLD}Tests ejecutados: $TOTAL${NC}"
echo -e "${BOLD}${GREEN}Pasados: $PASSED${NC}"
echo -e "${BOLD}${RED}Fallidos: $FAILED${NC}"

# Mostrar lista de comandos fallidos
if [ ${#FAILED_COMMANDS[@]} -gt 0 ]; then
    echo -e "\n${BOLD}${RED}COMANDOS QUE FALLARON:${NC}"
    for cmd in "${FAILED_COMMANDS[@]}"; do
        echo -e "${RED}  - $cmd${NC}"
    done
fi

if [ $FAILED -eq 0 ]; then
    echo -e "\n${BOLD}${GREEN}🎉 ¡TODOS LOS TESTS PASARON! 🎉${NC}"
    echo -e "${GREEN}El minishell está funcionando correctamente.${NC}"
    exit 0
else
    echo -e "\n${BOLD}${RED}❌ ALGUNOS TESTS FALLARON${NC}"
    echo -e "${RED}Revisa los errores mostrados arriba.${NC}"
    echo -e "${YELLOW}Porcentaje de éxito: $(( PASSED * 100 / TOTAL ))%${NC}"
    exit 1
fi
