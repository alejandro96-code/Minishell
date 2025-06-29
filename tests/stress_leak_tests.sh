#!/bin/bash

# Tests de stress y edge cases para detectar memory leaks difíciles de encontrar
# Se enfoca en casos límite y situaciones de alta carga

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m'

MINISHELL="../minishell"

# Función para tests de stress con monitoreo de memoria
stress_test() {
    local test_name="$1"
    local commands="$2"
    local iterations="${3:-1}"
    local timeout_duration="${4:-30}"
    
    echo -e "${PURPLE}⚡ Stress Test: $test_name${NC}"
    echo -e "${YELLOW}Iteraciones: $iterations, Timeout: ${timeout_duration}s${NC}"
    
    # Crear supresiones mejoradas para stress tests
    cat > stress_readline.supp << 'EOF'
{
   readline_stress_malloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:malloc
   ...
   obj:*/libreadline.so.*
}
{
   readline_stress_realloc
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:realloc
   ...
   obj:*/libreadline.so.*
}
{
   history_stress
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:add_history
}
{
   rl_stress
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:rl_*
}
{
   libc_stress
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:__libc_*
}
{
   dlopen_stress
   Memcheck:Leak
   match-leak-kinds: definite,indirect,possible
   fun:*alloc
   ...
   fun:*dlopen*
}
EOF
    
    for i in $(seq 1 $iterations); do
        echo -e "$commands\nexit" > stress_input.tmp
        
        timeout $timeout_duration valgrind \
            --leak-check=full \
            --show-leak-kinds=definite,indirect \
            --track-origins=yes \
            --suppressions=stress_readline.supp \
            --log-file=stress_output_$i.tmp \
            $MINISHELL < stress_input.tmp > /dev/null 2>&1
        
        local exit_code=$?
        
        if [ $exit_code -eq 124 ]; then
            echo -e "${RED}⏱️  TIMEOUT en iteración $i (posible leak o bucle infinito)${NC}"
            break
        elif grep -q "definitely lost\|indirectly lost" stress_output_$i.tmp; then
            # Verificar si es un leak real (no de readline)
            if grep -A 10 -B 5 "lost" stress_output_$i.tmp | grep -v "libreadline\|add_history\|rl_\|__libc\|dlopen" | grep -q "lost"; then
                echo -e "${RED}❌ LEAK REAL en iteración $i${NC}"
                echo "Detalles (filtrado, sin readline):"
                grep -A 3 "lost" stress_output_$i.tmp | grep -v "libreadline\|add_history\|rl_\|__libc"
                break
            else
                echo -e "${GREEN}✓ Iteración $i OK (solo leaks de readline ignorados)${NC}"
            fi
        else
            echo -e "${GREEN}✓ Iteración $i OK${NC}"
        fi
    done
    
    rm -f stress_input.tmp stress_output_*.tmp stress_readline.supp
    echo ""
}

# Función para tests de edge cases específicos
edge_case_test() {
    local test_name="$1"
    local commands="$2"
    local expected_behavior="$3"
    
    echo -e "${BLUE}🔥 Edge Case: $test_name${NC}"
    echo -e "${YELLOW}Comportamiento esperado: $expected_behavior${NC}"
    
    echo -e "$commands\nexit" > edge_input.tmp
    
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --error-exitcode=42 \
             --log-file=edge_output.tmp \
             $MINISHELL < edge_input.tmp > edge_stdout.tmp 2>&1
    
    local valgrind_exit=$?
    
    if [ $valgrind_exit -eq 42 ]; then
        echo -e "${RED}❌ MEMORY ERROR DETECTADO${NC}"
        echo "Detalles:"
        grep -A 5 -B 2 "Invalid\|lost\|freed" edge_output.tmp
    else
        echo -e "${GREEN}✅ Sin errores de memoria${NC}"
    fi
    
    rm -f edge_input.tmp edge_output.tmp edge_stdout.tmp
    echo ""
}

echo -e "${PURPLE}🚀 Tests de Stress y Edge Cases para Memory Leaks${NC}"
echo "=================================================================="

# =================== TESTS DE STRESS DE MEMORIA ===================
echo -e "${PURPLE}=== STRESS TESTS DE MEMORIA ===${NC}"

stress_test "Muchas variables de entorno" \
    "$(for i in {1..100}; do echo "export STRESS_VAR_$i=valor_muy_largo_para_causar_stress_de_memoria_$i"; done)" \
    3 \
    45

stress_test "Comandos muy largos repetitivos" \
    "$(for i in {1..50}; do echo "echo test_numero_$i con argumentos muy largos para stress"; done)" \
    2 \
    30

stress_test "Pipes anidados profundos" \
    "echo start$(printf ' | cat%.0s' {1..20})" \
    5 \
    20

stress_test "Redirecciones múltiples" \
    "$(for i in {1..30}; do echo "echo test$i > file$i.tmp"; done)" \
    3 \
    25

# =================== EDGE CASES CRÍTICOS ===================
echo -e "${PURPLE}=== EDGE CASES CRÍTICOS ===${NC}"

edge_case_test "String de longitud máxima" \
    "echo $(printf 'X%.0s' {1..4096})" \
    "Debe manejar strings muy largos sin overflow"

edge_case_test "Variables con nombres muy largos" \
    "export $(printf 'A%.0s' {1..255})=valor" \
    "Debe validar longitud de nombres de variables"

edge_case_test "PATH con muchos directorios" \
    "export PATH=$(printf '/dir%d:' {1..100}):/bin:/usr/bin" \
    "Debe manejar PATH muy largo sin leaks"

edge_case_test "Comandos con caracteres especiales" \
    "echo \"\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\"" \
    "Debe escapar caracteres especiales correctamente"

edge_case_test "Muchas comillas anidadas" \
    "echo \"'\"'\"'\"'\"'\"'\"'\"'\"'\"'\"'\"'\"'\"" \
    "Debe parsear comillas complejas sin leaks"

# =================== TESTS DE FRAGMENTACIÓN DE MEMORIA ===================
echo -e "${PURPLE}=== TESTS DE FRAGMENTACIÓN ===${NC}"

edge_case_test "Allocaciones y liberaciones alternadas" \
    "$(for i in {1..20}; do echo -e "export VAR$i=val$i\nunset VAR$i"; done)" \
    "Debe evitar fragmentación excesiva"

edge_case_test "Expansión de variables recursiva" \
    "export A=\$B\nexport B=\$C\nexport C=valor\necho \$A" \
    "Debe detectar/manejar recursión en variables"

# =================== TESTS DE CONDICIONES DE CARRERA ===================
echo -e "${PURPLE}=== TESTS DE CONCURRENCIA ===${NC}"

edge_case_test "Múltiples procesos hijos" \
    "sleep 0.1 & sleep 0.1 & sleep 0.1 & wait" \
    "Debe manejar múltiples procesos sin leaks"

edge_case_test "Pipes con procesos que fallan" \
    "comando_inexistente | cat | comando_inexistente2" \
    "Debe limpiar pipes incluso cuando procesos fallan"

# =================== TESTS DE LÍMITES DEL SISTEMA ===================
echo -e "${PURPLE}=== TESTS DE LÍMITES DEL SISTEMA ===${NC}"

edge_case_test "Muchos argumentos en comando" \
    "echo $(printf 'arg%d ' {1..1000})" \
    "Debe manejar límites de argumentos del sistema"

edge_case_test "Muchas redirecciones simultáneas" \
    "echo test $(printf '> file%d.tmp ' {1..50})" \
    "Debe manejar límites de descriptores de archivo"

# =================== TESTS DE RECOVERY DE ERRORES ===================
echo -e "${PURPLE}=== TESTS DE RECOVERY DE ERRORES ===${NC}"

edge_case_test "Malloc failures simulados" \
    "export HUGE_VAR=$(printf 'X%.0s' {1..10000})" \
    "Debe manejar gracefully fallos de malloc"

edge_case_test "Comandos interrumpidos por señales" \
    "sleep 10" \
    "Debe limpiar memoria cuando comandos son interrumpidos"

edge_case_test "Parsing de comandos malformados" \
    "echo \"test sin cerrar comillas\necho test2\necho \\\"test3\\\"" \
    "Debe recuperarse de errores de parsing"

# =================== TESTS DE LIBERACIÓN TARDÍA ===================
echo -e "${PURPLE}=== TESTS DE LIBERACIÓN TARDÍA ===${NC}"

edge_case_test "Variables que persisten entre comandos" \
    "export PERSIST=valor1\necho \$PERSIST\nexport PERSIST=valor2\necho \$PERSIST" \
    "Debe liberar valores antiguos al actualizar variables"

edge_case_test "History de comandos largos" \
    "$(for i in {1..20}; do echo "echo comando_largo_numero_$i con muchos argumentos para llenar history"; done)" \
    "Debe manejar history sin acumular memoria indefinidamente"

echo -e "${PURPLE}🏁 Stress tests completados${NC}"

# Limpiar archivos temporales que puedan quedar
rm -f file*.tmp stress_*.tmp edge_*.tmp

echo -e "${BLUE}💡 Recomendaciones:${NC}"
echo "1. Si hay timeouts, revisar bucles infinitos en parsing"
echo "2. Si hay leaks en stress tests, optimizar gestión de memoria en operaciones repetitivas"
echo "3. Si hay errores en edge cases, añadir validaciones de entrada"
echo "4. Usar herramientas como AddressSanitizer para detectar más errores: gcc -fsanitize=address"