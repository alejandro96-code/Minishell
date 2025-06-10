#!/bin/bash

# Script principal para ejecutar todas las pruebas de minishell
# Uso: ./test_runner.sh [test_name]

MINISHELL_PATH="../Mini_2.0/minishell"
TEST_DIR="."
RESULTS_DIR="results"

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Crear directorio de resultados
mkdir -p "$RESULTS_DIR"

echo -e "${BLUE}🧪 SUITE DE PRUEBAS PARA MINISHELL${NC}"
echo "======================================"

# Lista de scripts de prueba
TESTS=(
    "test_builtins.sh"
    "test_pipes.sh" 
    "test_redirections.sh"
    "test_variables.sh"
    "test_quotes.sh"
    "test_edge_cases.sh"
)

# Si se especifica un test, solo ejecutar ese
if [ $# -eq 1 ]; then
    TESTS=("$1")
fi

TOTAL_PASSED=0
TOTAL_FAILED=0

for test in "${TESTS[@]}"; do
    if [ -f "$TEST_DIR/$test" ]; then
        echo -e "\n${YELLOW}🔍 Ejecutando $test...${NC}"
        chmod +x "$TEST_DIR/$test"
        ./"$test" "$MINISHELL_PATH"
        
        # Capturar resultados del último test
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ $test completado${NC}"
        else
            echo -e "${RED}❌ $test falló${NC}"
        fi
    else
        echo -e "${RED}⚠️  $test no encontrado${NC}"
    fi
done

echo -e "\n${BLUE}======================================"
echo -e "🏁 RESUMEN FINAL${NC}"
echo "======================================"
