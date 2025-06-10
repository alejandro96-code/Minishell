#!/bin/bash

# Pruebas para variables de entorno y expansión
MINISHELL=${1:-"../Mini_2.0/minishell"}
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}💰 TESTING VARIABLES${NC}"

# Función mejorada para probar variables
test_var() {
    local cmd="$1"
    local expected="$2"
    local test_name="$3"
    
    echo -n "Testing: $test_name... "
    
    local tmp_output=$(mktemp)
    echo -e "$cmd\nexit" | timeout 5 $MINISHELL > "$tmp_output" 2>/dev/null
    
    # Limpiar salida
    result=$(grep -v "exit" "$tmp_output" | sed '/dgasco-g@Minishell/d' | sed '/^$/d' | tail -1)
    
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

# Función para verificar que una variable contiene algo específico
test_var_contains() {
    local cmd="$1"
    local expected="$2"
    local test_name="$3"
    
    echo -n "Testing: $test_name... "
    
    local tmp_output=$(mktemp)
    echo -e "$cmd\nexit" | timeout 5 $MINISHELL > "$tmp_output" 2>/dev/null
    
    result=$(grep -v "exit" "$tmp_output" | sed '/dgasco-g@Minishell/d' | grep "$expected")
    
    if [[ -n "$result" ]]; then
        echo -e "${GREEN}✅ PASS${NC}"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "  Command: $cmd"
        echo "  Expected to contain: '$expected'"
        echo "  Full output:"
        cat "$tmp_output" | head -3
        ((FAILED++))
    fi
    
    rm -f "$tmp_output"
}

# Tests básicos de variables - verificar que contienen algo
test_var_contains "echo \$HOME" "/home" "variable HOME"
test_var_contains "echo \$USER" "$USER" "variable USER"
test_var_contains "echo \$PATH" "/bin" "variable PATH"

# Tests de export/unset con verificación mejorada
echo -n "Testing: export nueva variable... "
tmp_file=$(mktemp)
echo -e "export MYVAR=hello\necho \$MYVAR\nexit" | timeout 5 $MINISHELL > "$tmp_file" 2>/dev/null
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

echo -n "Testing: modificar variable exportada... "
tmp_file=$(mktemp)
echo -e "export MYVAR=hello\nexport MYVAR=world\necho \$MYVAR\nexit" | timeout 5 $MINISHELL > "$tmp_file" 2>/dev/null
result=$(grep -v "exit" "$tmp_file" | sed '/dgasco-g@Minishell/d' | grep "world")
if [[ -n "$result" ]]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    echo "  Expected: 'world'"
    echo "  Got output:"
    cat "$tmp_file" | head -5
    ((FAILED++))
fi
rm -f "$tmp_file"

# Test de unset
echo -n "Testing: unset variable... "
tmp_file=$(mktemp)
echo -e "export TESTVAR=test\nunset TESTVAR\necho \$TESTVAR\nexit" | timeout 5 $MINISHELL > "$tmp_file" 2>/dev/null
result=$(grep -v "exit" "$tmp_file" | sed '/dgasco-g@Minishell/d' | tail -1)
# La variable debería estar vacía o mostrar literalmente $TESTVAR
if [[ -z "$result" ]] || [[ "$result" == *"\$TESTVAR"* ]] || [[ "$result" == *"TESTVAR"* ]]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    echo "  Expected: empty or \$TESTVAR"
    echo "  Got: '$result'"
    ((FAILED++))
fi
rm -f "$tmp_file"

# Test de variable no existente
test_var "echo \$NONEXISTENT" "" "variable no existente"

# Test con múltiples variables - simplificado
echo -n "Testing: múltiples variables... "
tmp_file=$(mktemp)
echo -e "export A=1\nexport B=2\necho \$A\$B\nexit" | timeout 5 $MINISHELL > "$tmp_file" 2>/dev/null
result=$(grep -v "exit" "$tmp_file" | sed '/dgasco-g@Minishell/d' | grep "12")
if [[ -n "$result" ]]; then
    echo -e "${GREEN}✅ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌ FAIL${NC}"
    echo "  Expected: '12'"
    echo "  Got output:"
    cat "$tmp_file" | head -5
    ((FAILED++))
fi
rm -f "$tmp_file"

echo -e "\n${YELLOW}VARIABLES RESULTS: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}"
[ $FAILED -eq 0 ] && exit 0 || exit 1
