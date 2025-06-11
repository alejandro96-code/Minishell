#!/bin/bash

# Test simple con valgrind para detectar memory leaks en minishell
# Creado para verificar que no hay fugas de memoria

MINISHELL="./minishell"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
NC="\033[0m" # No Color
BOLD="\033[1m"

PASSED=0
FAILED=0
TOTAL=0
FAILED_COMMANDS=()  # Array para almacenar comandos fallidos

# Función para imprimir encabezados
print_header() {
    echo -e "\n${BOLD}${BLUE}======================================${NC}"
    echo -e "${BOLD}${BLUE}  $1${NC}"
    echo -e "${BOLD}${BLUE}======================================${NC}"
}

# Función para probar comandos con valgrind
test_with_valgrind() {
    local description="$1"
    local command="$2"
    
    ((TOTAL++))
    echo -e "\n${YELLOW}Testing: $description${NC}"
    echo -e "${BLUE}Command: $command${NC}"
    
    # Crear archivo temporal para el comando
    local cmd_file=$(mktemp)
    echo -e "$command\nexit" > "$cmd_file"
    
    # Ejecutar con valgrind
    echo -e "${YELLOW}Running valgrind...${NC}"
    local valgrind_output=$(mktemp)
    
    timeout 30 valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=42 \
        $MINISHELL < "$cmd_file" > /dev/null 2> "$valgrind_output"
    
    local exit_code=$?
    
    # Analizar resultados de valgrind
    local definitely_lost=$(grep "definitely lost:" "$valgrind_output" | tail -1 | awk '{print $4}' | sed 's/,//g' | tr -d '\n')
    local possibly_lost=$(grep "possibly lost:" "$valgrind_output" | tail -1 | awk '{print $4}' | sed 's/,//g' | tr -d '\n')
    local still_reachable=$(grep "still reachable:" "$valgrind_output" | tail -1 | awk '{print $4}' | sed 's/,//g' | tr -d '\n')
    
    # Verificar si hay errores graves
    local errors=$(grep "ERROR SUMMARY:" "$valgrind_output" | tail -1 | awk '{print $4}' | tr -d '\n')
    
    # Establecer valores por defecto si están vacíos o no son números
    [[ ! "$definitely_lost" =~ ^[0-9]+$ ]] && definitely_lost=0
    [[ ! "$possibly_lost" =~ ^[0-9]+$ ]] && possibly_lost=0
    [[ ! "$still_reachable" =~ ^[0-9]+$ ]] && still_reachable=0
    [[ ! "$errors" =~ ^[0-9]+$ ]] && errors=0
    
    echo -e "${BLUE}Valgrind Results:${NC}"
    echo "  Exit code: $exit_code"
    echo "  Errors: ${errors:-0}"
    echo "  Definitely lost: ${definitely_lost:-0} bytes"
    echo "  Possibly lost: ${possibly_lost:-0} bytes"
    echo "  Still reachable: ${still_reachable:-0} bytes"
    
    # Determinar si el test pasó
    if [ "$exit_code" -eq 0 ] && [ "${errors}" -eq 0 ] && [ "${definitely_lost}" -eq 0 ]; then
        echo -e "${GREEN}✅ PASS - No memory leaks detected${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL - Memory issues detected${NC}"
        FAILED_COMMANDS+=("$command")  # Agregar comando a la lista de fallos
        if [ "${definitely_lost}" -gt 0 ]; then
            echo -e "${RED}  → Definitely lost: $definitely_lost bytes${NC}"
        fi
        if [ "${errors}" -gt 0 ]; then
            echo -e "${RED}  → Valgrind errors: $errors${NC}"
        fi
        if [ "$exit_code" -ne 0 ] && [ "$exit_code" -ne 42 ]; then
            echo -e "${RED}  → Unexpected exit code: $exit_code${NC}"
        fi
        
        # Mostrar detalles del error si es necesario
        echo -e "${YELLOW}Valgrind details:${NC}"
        head -20 "$valgrind_output" | sed 's/^/  /'
        
        ((FAILED++))
    fi
    
    # Limpiar archivos temporales
    rm -f "$cmd_file" "$valgrind_output"
}

# Verificar que valgrind está instalado
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}Error: valgrind no está instalado${NC}"
    echo "Por favor instala valgrind: sudo apt-get install valgrind"
    exit 1
fi

# Verificar que el minishell existe
if [ ! -f "$MINISHELL" ]; then
    echo -e "${RED}Error: $MINISHELL no encontrado${NC}"
    echo "Por favor compila el minishell primero: make"
    exit 1
fi

print_header "MEMORY LEAK TESTS WITH VALGRIND"

# ====================================
# TESTS BÁSICOS
# ====================================

print_header "COMANDOS BÁSICOS"

test_with_valgrind "Echo simple" "echo hello world"
test_with_valgrind "PWD" "pwd"
test_with_valgrind "ENV" "env | head -3"

# ====================================
# BUILTINS
# ====================================

print_header "BUILTINS"

test_with_valgrind "Export variable" "export TEST=value"
test_with_valgrind "Export y echo variable" "export TESTVAR=hello && echo \$TESTVAR"
test_with_valgrind "CD al home" "cd"
test_with_valgrind "Unset variable" "export TEMP=test && unset TEMP"

# ====================================
# PIPES
# ====================================

print_header "PIPES"

test_with_valgrind "Pipe simple" "echo hello | cat"
test_with_valgrind "Pipe con grep" "env | grep USER"
test_with_valgrind "Pipe múltiple" "echo test | cat | cat"

# ====================================
# OPERADORES LÓGICOS
# ====================================

print_header "OPERADORES LÓGICOS"

test_with_valgrind "Operador &&" "echo hello && echo world"
test_with_valgrind "Operador ||" "echo hello || echo world"
test_with_valgrind "Combinación && y ||" "echo test && echo success || echo failed"

# ====================================
# REDIRECCIONES
# ====================================

print_header "REDIRECCIONES"

test_with_valgrind "Redirección salida" "echo test > /tmp/minishell_test.txt"
test_with_valgrind "Redirección append" "echo append >> /tmp/minishell_test.txt"
test_with_valgrind "Redirección entrada" "cat < /tmp/minishell_test.txt"

# ====================================
# WILDCARDS
# ====================================

print_header "WILDCARDS"

test_with_valgrind "Wildcard simple" "ls *.c"
test_with_valgrind "Wildcard en directorio" "ls utils/*.c"

# ====================================
# COMANDOS COMPLEJOS
# ====================================

print_header "COMANDOS COMPLEJOS"

test_with_valgrind "Comando complejo con variables" "export A=hello && export B=world && echo \$A \$B"
test_with_valgrind "Pipe y operadores" "echo test | cat && echo success"
test_with_valgrind "Múltiples exports" "export A=1 && export B=2 && export C=3"

# ====================================
# CASES EDGE
# ====================================

print_header "CASOS EDGE"

test_with_valgrind "Comando vacío" ""
test_with_valgrind "Solo espacios" "   "
test_with_valgrind "Múltiples espacios" "echo    hello    world"

# ====================================
# LIMPIEZA
# ====================================

# Limpiar archivos de prueba
rm -f /tmp/minishell_test.txt

# ====================================
# RESULTADOS FINALES
# ====================================

print_header "RESULTADOS FINALES"

echo -e "${BOLD}Tests ejecutados: $TOTAL${NC}"
echo -e "${BOLD}${GREEN}Sin memory leaks: $PASSED${NC}"
echo -e "${BOLD}${RED}Con memory leaks: $FAILED${NC}"

# Mostrar lista de comandos que fallaron
if [ $FAILED -gt 0 ]; then
    print_header "COMANDOS QUE FALLARON"
    echo -e "${RED}Los siguientes comandos tienen memory leaks:${NC}\n"
    for i in "${!FAILED_COMMANDS[@]}"; do
        echo -e "${RED}$(($i + 1)). ${FAILED_COMMANDS[$i]}${NC}"
    done
fi

if [ $FAILED -eq 0 ]; then
    echo -e "\n${BOLD}${GREEN}🎉 ¡EXCELENTE! NO HAY MEMORY LEAKS 🎉${NC}"
    echo -e "${GREEN}El minishell maneja la memoria correctamente.${NC}"
    exit 0
else
    echo -e "\n${BOLD}${RED}❌ SE DETECTARON MEMORY LEAKS${NC}"
    echo -e "${RED}Revisa los errores mostrados arriba.${NC}"
    echo -e "${YELLOW}Porcentaje sin leaks: $(( PASSED * 100 / TOTAL ))%${NC}"
    exit 1
fi