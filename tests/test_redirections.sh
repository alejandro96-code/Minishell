#!/bin/bash

# Test de redirecciones para minishell
# Ejecutar desde el directorio raíz del proyecto: ./tests/test_redirections.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== TESTS DE REDIRECCIONES PARA MINISHELL ===${NC}"
echo

# Función para ejecutar test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_file="$3"
    
    echo -e "${YELLOW}Test: $test_name${NC}"
    echo "Comando: $command"
    
    # Ejecutar comando en minishell
    echo "$command" | ./minishell > /dev/null 2>&1
    
    # Verificar si se creó el archivo esperado
    if [ -n "$expected_file" ] && [ -f "$expected_file" ]; then
        echo -e "${GREEN}✓ PASS: Archivo $expected_file creado${NC}"
        echo "Contenido:"
        cat "$expected_file"
        echo
    elif [ -n "$expected_file" ]; then
        echo -e "${RED}✗ FAIL: Archivo $expected_file no se creó${NC}"
    else
        echo -e "${GREEN}✓ Comando ejecutado${NC}"
    fi
    echo "---"
}

# Función para limpiar archivos de test
cleanup() {
    rm -f test_output.txt test_input.txt test_append.txt test_heredoc.txt
    rm -f file1.txt file2.txt temp.txt
}

# Limpiar antes de empezar
cleanup

echo -e "${YELLOW}1. REDIRECCIÓN DE SALIDA (>)${NC}"

# Test 1: Redirección simple de salida
run_test "Redirección de echo a archivo" "echo 'Hola mundo' > test_output.txt" "test_output.txt"

# Test 2: Redirección de ls
run_test "Redirección de ls" "ls > file1.txt" "file1.txt"

# Test 3: Redirección con comando que no existe
run_test "Comando inexistente con redirección" "comando_que_no_existe > test_output.txt" "test_output.txt"

echo -e "${YELLOW}2. REDIRECCIÓN DE SALIDA CON APPEND (>>)${NC}"

# Test 4: Append a archivo existente
echo "Primera línea" > test_append.txt
run_test "Append a archivo existente" "echo 'Segunda línea' >> test_append.txt" "test_append.txt"

# Test 5: Append a archivo nuevo
run_test "Append a archivo nuevo" "echo 'Archivo nuevo' >> file2.txt" "file2.txt"

echo -e "${YELLOW}3. REDIRECCIÓN DE ENTRADA (<)${NC}"

# Test 6: Crear archivo de entrada y usarlo
echo -e "línea 1\nlínea 2\nlínea 3" > test_input.txt
run_test "Cat con redirección de entrada" "cat < test_input.txt > temp.txt" "temp.txt"

# Test 7: wc con redirección de entrada
run_test "wc con redirección de entrada" "wc -l < test_input.txt > temp.txt" "temp.txt"

echo -e "${YELLOW}4. HEREDOC (<<)${NC}"

# Test 8: Heredoc simple
run_test "Heredoc simple" "cat << EOF > test_heredoc.txt
Primera línea del heredoc
Segunda línea del heredoc
EOF" "test_heredoc.txt"

echo -e "${YELLOW}5. REDIRECCIONES MÚLTIPLES${NC}"

# Test 9: Redirección de entrada y salida
run_test "Entrada y salida" "cat < test_input.txt > temp.txt"

# Test 10: Comando con múltiples redirecciones
run_test "Múltiples redirecciones" "echo 'test' > temp.txt && cat temp.txt > file1.txt"

echo -e "${YELLOW}6. CASOS DE ERROR${NC}"

# Test 11: Redirección sin archivo
echo "echo 'test' >" | ./minishell > /dev/null 2>&1
echo "Test: Redirección sin archivo de destino"
echo -e "${GREEN}✓ Debería mostrar error de sintaxis${NC}"
echo "---"

# Test 12: Redirección a archivo sin permisos
touch no_permissions.txt
chmod 000 no_permissions.txt
echo "echo 'test' > no_permissions.txt" | ./minishell > /dev/null 2>&1
echo "Test: Redirección a archivo sin permisos"
echo -e "${GREEN}✓ Debería mostrar error de permisos${NC}"
chmod 644 no_permissions.txt
rm -f no_permissions.txt
echo "---"

echo -e "${YELLOW}7. REDIRECCIONES CON PIPES${NC}"

# Test 13: Pipe con redirección
run_test "Pipe con redirección de salida" "echo 'test pipe' | cat > temp.txt" "temp.txt"

# Test 14: Redirección con pipe
run_test "Redirección con pipe" "cat < test_input.txt | wc -l > temp.txt" "temp.txt"

echo -e "${YELLOW}8. REDIRECCIONES CON COMILLAS${NC}"

# Test 15: Redirección con archivo entre comillas
run_test "Archivo entre comillas" "echo 'test quotes' > 'file with spaces.txt'" "file with spaces.txt"

# Test 16: Redirección con contenido entre comillas
run_test "Contenido entre comillas" "echo 'texto con espacios' > temp.txt" "temp.txt"

echo -e "${YELLOW}9. ORDEN DE ARGUMENTOS${NC}"

# Test 17: Redirección antes del comando
run_test "Redirección antes del comando" "> temp.txt echo 'orden test'" "temp.txt"

# Test 18: Redirección en el medio
run_test "Redirección en el medio" "echo > temp.txt 'medio test'" "temp.txt"

echo -e "${YELLOW}10. CASOS ESPECIALES${NC}"

# Test 19: Redirección a /dev/null
run_test "Redirección a /dev/null" "echo 'no output' > /dev/null"

# Test 20: Redirección desde /dev/null
run_test "Entrada desde /dev/null" "cat < /dev/null > temp.txt" "temp.txt"

echo -e "${GREEN}=== TESTS COMPLETADOS ===${NC}"

# Mostrar archivos creados
echo -e "${YELLOW}Archivos creados durante las pruebas:${NC}"
ls -la *.txt 2>/dev/null || echo "No se crearon archivos .txt"

# Limpiar archivos de test
echo -e "\n${YELLOW}¿Limpiar archivos de test? (y/n)${NC}"
read -r response
if [[ "$response" =~ ^[Yy]$ ]]; then
    cleanup
    rm -f "file with spaces.txt"
    echo "Archivos limpiados"
fi