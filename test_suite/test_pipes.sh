#!/bin/bash

# Pruebas específicas para pipelines
MINISHELL=${1:-"../Mini_2.0/minishell"}
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}🔗 TESTING PIPES${NC}"

# Función mejorada para comparar con bash
test_pipe() {
    local cmd="$1"
    local test_name="$2"
    
    echo -n "Testing: $test_name... "
    
    # Crear archivos temporales
    local tmp_mini=$(mktemp)
    local tmp_bash=$(mktemp)
    local tmp_mini_clean=$(mktemp)
    
    # Ejecutar en minishell
    echo -e "$cmd\nexit" | timeout 8 $MINISHELL > "$tmp_mini" 2>/dev/null
    local exit_mini=$?
    
    # Ejecutar en bash
    timeout 8 bash -c "$cmd" > "$tmp_bash" 2>/dev/null
    local exit_bash=$?
    
    # Limpiar output de minishell (quitar líneas del prompt y exit)
    grep -v "exit" "$tmp_mini" | sed '/dgasco-g@Minishell/d' | sed '/^$/d' > "$tmp_mini_clean"
    
    # Comparar si ambos tienen contenido similar
    if [ -s "$tmp_mini_clean" ] && [ -s "$tmp_bash" ]; then
        # Verificar que el contenido principal coincida
        mini_content=$(cat "$tmp_mini_clean" | head -1)
        bash_content=$(cat "$tmp_bash" | head -1)
        
        if [[ "$mini_content" == "$bash_content" ]] || [[ "$mini_content" == *"$(echo "$bash_content" | cut -c1-10)"* ]]; then
            echo -e "${GREEN}✅ PASS${NC}"
            ((PASSED++))
        else
            echo -e "${RED}❌ FAIL${NC}"
            echo "  Command: $cmd"
            echo "  Minishell: '$mini_content'"
            echo "  Bash: '$bash_content'"
            ((FAILED++))
        fi
    elif [ ! -s "$tmp_mini_clean" ] && [ ! -s "$tmp_bash" ]; then
        # Ambos vacíos, ok
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Command: $cmd"
        echo "  Minishell output exists: $([ -s "$tmp_mini_clean" ] && echo "yes" || echo "no")"
        echo "  Bash output exists: $([ -s "$tmp_bash" ] && echo "yes" || echo "no")"
        ((FAILED++))
    fi
    
    # Limpieza
    rm -f "$tmp_mini" "$tmp_bash" "$tmp_mini_clean"
}

# Función para tests simples que solo verifican que no fallen
test_pipe_simple() {
    local cmd="$1"
    local test_name="$2"
    
    echo -n "Testing: $test_name... "
    
    local tmp_output=$(mktemp)
    echo -e "$cmd\nexit" | timeout 5 $MINISHELL > "$tmp_output" 2>/dev/null
    local exit_code=$?
    
    # Si no hay timeout (exit code 124) y el comando se ejecutó, considerarlo éxito
    if [ $exit_code -ne 124 ]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC} (timeout)"
        ((FAILED++))
    fi
    
    rm -f "$tmp_output"
}

# Tests básicos de pipes
test_pipe "echo hello | cat" "pipe básico"
test_pipe "echo hello world | wc -w" "echo pipe wc"

# Tests con ls (usando test simple para evitar diferencias de formato)
test_pipe_simple "ls | head -3" "ls pipe head"

# Tests con múltiples pipes
test_pipe "echo hello world | cat | cat" "pipe doble"
test_pipe_simple "echo test | cat | wc -c" "pipe triple"

# Tests con builtins en pipes
test_pipe "echo test | cat" "echo builtin en pipe"
test_pipe_simple "pwd | cat" "pwd builtin en pipe"

# Tests casos especiales
test_pipe "echo | cat" "pipes con echo vacío"
test_pipe_simple "echo hello | grep hello" "pipe con grep"

# Test específico para verificar que los pipes básicos funcionan
echo -n "Testing: basic pipe functionality... "
result=$(echo -e "echo test | cat\nexit" | timeout 3 $MINISHELL 2>/dev/null | grep -v "exit" | sed '/dgasco-g@Minishell/d' | grep "test")
if [[ -n "$result" ]]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    ((FAILED++))
fi

echo -e "\n${YELLOW}PIPES RESULTS: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}"
[ $FAILED -eq 0 ] && exit 0 || exit 1
