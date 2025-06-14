#!/bin/bash

# ============================================================================
# ALL TESTS RUN - Ejecuta todos los tests de minishell
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

# Variables de control
TOTAL_TEST_SUITES=0
PASSED_TEST_SUITES=0
FAILED_TEST_SUITES=0

# Directorio de tests
TEST_DIR="$(dirname "$0")"
MINISHELL_DIR="$(dirname "$TEST_DIR")"

echo -e "${PURPLE}============================================${NC}"
echo -e "${WHITE}  MINISHELL COMPREHENSIVE TEST RUNNER${NC}"
echo -e "${PURPLE}============================================${NC}"
echo ""

# Verificar que minishell existe
if [[ ! -f "$MINISHELL_DIR/minishell" ]]; then
    echo -e "${RED}❌ Error: minishell no encontrado en $MINISHELL_DIR${NC}"
    echo -e "${YELLOW}Ejecuta 'make' en el directorio del proyecto primero${NC}"
    exit 1
fi

# Verificar que valgrind está instalado
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}❌ Error: valgrind no está instalado${NC}"
    echo -e "${YELLOW}Instala valgrind para ejecutar los tests de memory leaks${NC}"
    exit 1
fi

echo -e "${CYAN}Directorio del proyecto: $MINISHELL_DIR${NC}"
echo -e "${CYAN}Ejecutando desde: $TEST_DIR${NC}"
echo ""

# Cambiar al directorio del proyecto
cd "$MINISHELL_DIR" || exit 1

# Función para ejecutar un test suite
run_test_suite() {
    local test_name="$1"
    local test_script="$2"
    
    ((TOTAL_TEST_SUITES++))
    
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${WHITE}Ejecutando: $test_name${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    
    # Ejecutar el test
    if bash "$test_script"; then
        echo -e "${GREEN}✅ $test_name COMPLETADO EXITOSAMENTE${NC}"
        ((PASSED_TEST_SUITES++))
    else
        echo -e "${RED}❌ $test_name FALLÓ${NC}"
        ((FAILED_TEST_SUITES++))
    fi
    
    echo ""
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
}

# Lista de tests a ejecutar en orden
declare -a TEST_SUITES=(
    "BUILTINS TESTS:tests/test_builtins.sh"
    "PIPES TESTS:tests/test_pipes.sh"
    "REDIRECTIONS TESTS:tests/test_redirections.sh"
    "SIGNALS TESTS:tests/test_signals.sh"
)

echo -e "${YELLOW}Ejecutando ${#TEST_SUITES[@]} test suites...${NC}"
echo ""

# Ejecutar cada test suite
for test_suite in "${TEST_SUITES[@]}"; do
    IFS=':' read -r test_name test_script <<< "$test_suite"
    
    # Verificar que el script existe
    if [[ ! -f "$test_script" ]]; then
        echo -e "${RED}❌ Error: Test script no encontrado: $test_script${NC}"
        ((TOTAL_TEST_SUITES++))
        ((FAILED_TEST_SUITES++))
        continue
    fi
    
    # Hacer el script ejecutable
    chmod +x "$test_script"
    
    # Ejecutar el test suite
    run_test_suite "$test_name" "$test_script"
    
    # Pausa breve entre tests
    sleep 1
done

# Mostrar resumen final
echo -e "${PURPLE}============================================${NC}"
echo -e "${WHITE}  RESUMEN FINAL DE TODOS LOS TESTS${NC}"
echo -e "${PURPLE}============================================${NC}"
echo ""

echo -e "${CYAN}Test Suites ejecutados: $TOTAL_TEST_SUITES${NC}"
echo -e "${GREEN}Test Suites exitosos: $PASSED_TEST_SUITES${NC}"
echo -e "${RED}Test Suites fallidos: $FAILED_TEST_SUITES${NC}"
echo ""

# Calcular porcentaje de éxito
if [[ $TOTAL_TEST_SUITES -gt 0 ]]; then
    local success_rate=$((PASSED_TEST_SUITES * 100 / TOTAL_TEST_SUITES))
    echo -e "${CYAN}Tasa de éxito: ${success_rate}%${NC}"
fi

echo ""

# Resultado final
if [[ $FAILED_TEST_SUITES -eq 0 ]]; then
    echo -e "${GREEN}🎉 ¡TODOS LOS TEST SUITES PASARON EXITOSAMENTE! 🎉${NC}"
    echo -e "${GREEN}Tu minishell está funcionando correctamente.${NC}"
    exit 0
else
    echo -e "${RED}💥 ALGUNOS TEST SUITES FALLARON 💥${NC}"
    echo -e "${YELLOW}Revisa los resultados arriba para más detalles.${NC}"
    echo ""
    echo -e "${CYAN}Notas:${NC}"
    echo -e "${CYAN}- Los fallos no necesariamente indican errores críticos${NC}"
    echo -e "${CYAN}- Algunos tests pueden fallar por diferencias en implementación${NC}"
    echo -e "${CYAN}- Los memory leaks se reportan pero no causan fallo automático${NC}"
    exit 1
fi
