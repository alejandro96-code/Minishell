#!/bin/bash

# Pruebas para comandos builtin de minishell
MINISHELL=${1:-"../Mini_2.0/minishell"}
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}🔧 TESTING BUILTINS${NC}"

# Función mejorada para ejecutar test
run_test() {
    local cmd="$1"
    local expected="$2"
    local test_name="$3"
    local no_newline="$4"
    
    echo -n "Testing: $test_name... "
    
    # Crear archivos temporales
    local tmp_output=$(mktemp)
    
    # Ejecutar comando en minishell
    echo -e "$cmd\nexit" | timeout 5 $MINISHELL > "$tmp_output" 2>/dev/null
    
    # Limpiar salida eliminando líneas del prompt y exit
    if [ "$no_newline" = "true" ]; then
        # Para echo -n, extraer solo el contenido entre el comando y el siguiente prompt
        result=$(cat "$tmp_output" | sed 's/\x1b\[[0-9;]*m//g' | grep -A1 "$cmd" | tail -1 | sed 's/.*: //' | tr -d '\n')
    else
        result=$(grep -v "exit" "$tmp_output" | sed '/dgasco-g@Minishell/d' | sed '/^$/d' | tail -1)
    fi
    
    if [[ "$result" == *"$expected"* ]] || [[ -z "$expected" && -z "$result" ]]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Command: $cmd"
        echo "  Expected: '$expected'"
        echo "  Got: '$result'"
        ((FAILED++))
    fi
    
    rm -f "$tmp_output"
}

# Función especial para comandos con pipes
run_pipe_test() {
    local cmd="$1"
    local expected="$2"
    local test_name="$3"
    
    echo -n "Testing: $test_name... "
    
    # Crear archivos temporales
    local tmp_output=$(mktemp)
    
    # Ejecutar comando en minishell
    echo -e "$cmd\nexit" | timeout 5 $MINISHELL > "$tmp_output" 2>/dev/null
    
    # Limpiar salida
    result=$(grep -v "exit" "$tmp_output" | sed '/dgasco-g@Minishell/d' | grep "$expected")
    
    if [[ -n "$result" ]]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Command: $cmd"
        echo "  Expected to contain: '$expected'"
        echo "  Full output:"
        cat "$tmp_output" | head -5
        ((FAILED++))
    fi
    
    rm -f "$tmp_output"
}

# Tests para echo
run_test "echo hello world" "hello world" "echo básico"
run_test "echo" "" "echo vacío"

# Tests para pwd
current_dir=$(pwd)
# Ajustar para obtener solo el directorio actual
current_dirname=$(basename "$current_dir")
run_test "pwd" "$current_dirname" "pwd"

# Tests para cd - simplificado
echo -n "Testing: cd command... "
echo -e "cd ..\npwd\nexit" | timeout 5 $MINISHELL >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    ((FAILED++))
fi

# Tests para env
run_pipe_test "env | grep HOME" "HOME=" "env muestra HOME"

# Tests para export/unset - mejorados
echo -n "Testing: export variable... "
tmp_file=$(mktemp)
echo -e "export TEST_VAR=hello\necho \$TEST_VAR\nexit" | timeout 5 $MINISHELL > "$tmp_file" 2>/dev/null
result=$(grep -v "exit" "$tmp_file" | sed '/dgasco-g@Minishell/d' | grep "hello")
if [[ -n "$result" ]]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    echo "  Expected: 'hello'"
    echo "  Got output:"
    cat "$tmp_file" | head -5
    ((FAILED++))
fi
rm -f "$tmp_file"

echo -n "Testing: unset variable... "
tmp_file=$(mktemp)
echo -e "export TEST_VAR=hello\nunset TEST_VAR\necho \$TEST_VAR\nexit" | timeout 5 $MINISHELL > "$tmp_file" 2>/dev/null
result=$(grep -v "exit" "$tmp_file" | sed '/dgasco-g@Minishell/d' | tail -1)
if [[ -z "$result" ]] || [[ "$result" == *"\$TEST_VAR"* ]]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    echo "  Expected: empty or \$TEST_VAR"
    echo "  Got: '$result'"
    ((FAILED++))
fi
rm -f "$tmp_file"

# Tests para exit
echo -n "Testing: exit code... "
echo "exit 42" | timeout 3 $MINISHELL >/dev/null 2>&1
if [ $? -eq 42 ]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    ((FAILED++))
fi

echo -e "\n${YELLOW}BUILTINS RESULTS: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}"
[ $FAILED -eq 0 ] && exit 0 || exit 1
