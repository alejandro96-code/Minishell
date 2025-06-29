#!/bin/bash

# Script maestro para ejecutar todos los tests de memory leaks de minishell
# Ejecuta todos los tipos de tests en orden y genera un reporte completo

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

MINISHELL="../minishell"
REPORT_FILE="leak_report_$(date +%Y%m%d_%H%M%S).txt"

# Función para mostrar el header
show_header() {
    echo -e "${CYAN}================================================================${NC}"
    echo -e "${CYAN}           SUITE COMPLETA DE TESTS DE MEMORY LEAKS${NC}"
    echo -e "${CYAN}                     MINISHELL PROJECT${NC}"
    echo -e "${CYAN}================================================================${NC}"
    echo -e "${YELLOW}Fecha: $(date)${NC}"
    echo -e "${YELLOW}Directorio: $(pwd)${NC}"
    echo -e "${YELLOW}Minishell: $MINISHELL${NC}"
    echo ""
}

# Función para verificar prerequisitos
check_prerequisites() {
    echo -e "${BLUE}🔍 Verificando prerequisitos...${NC}"
    
    local errors=0
    
    # Verificar valgrind
    if ! command -v valgrind &> /dev/null; then
        echo -e "${RED}❌ valgrind no está instalado${NC}"
        echo "   Instalar con: sudo apt-get install valgrind"
        errors=$((errors + 1))
    else
        echo -e "${GREEN}✅ valgrind encontrado: $(valgrind --version | head -1)${NC}"
    fi
    
    # Verificar minishell
    if [ ! -f "$MINISHELL" ]; then
        echo -e "${RED}❌ minishell no encontrado en $MINISHELL${NC}"
        echo "   Compilar con: make"
        errors=$((errors + 1))
    else
        echo -e "${GREEN}✅ minishell encontrado${NC}"
    fi
    
    # Verificar permisos de ejecución de scripts
    for script in "memory_leak_tests.sh" "detailed_leak_tests.sh" "stress_leak_tests.sh"; do
        if [ ! -x "$script" ]; then
            echo -e "${YELLOW}⚠️  Dando permisos de ejecución a $script${NC}"
            chmod +x "$script"
        fi
    done
    
    if [ $errors -gt 0 ]; then
        echo -e "${RED}❌ Se encontraron $errors errores. Corregir antes de continuar.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✅ Todos los prerequisitos están listos${NC}"
    echo ""
}

# Función para ejecutar un tipo de test
run_test_suite() {
    local suite_name="$1"
    local script_name="$2"
    local description="$3"
    
    echo -e "${PURPLE}🚀 Ejecutando: $suite_name${NC}"
    echo -e "${YELLOW}Descripción: $description${NC}"
    echo -e "${CYAN}Script: $script_name${NC}"
    echo "$(date): Iniciando $suite_name" >> "$REPORT_FILE"
    echo "----------------------------------------" >> "$REPORT_FILE"
    
    if [ -f "$script_name" ]; then
        echo ""
        ./"$script_name" 2>&1 | tee -a "$REPORT_FILE"
        local exit_code=$?
        
        echo "" >> "$REPORT_FILE"
        echo "Código de salida: $exit_code" >> "$REPORT_FILE"
        echo "Finalizado: $(date)" >> "$REPORT_FILE"
        echo "========================================" >> "$REPORT_FILE"
        echo "" >> "$REPORT_FILE"
        
        if [ $exit_code -eq 0 ]; then
            echo -e "${GREEN}✅ $suite_name completado exitosamente${NC}"
        else
            echo -e "${YELLOW}⚠️  $suite_name completado con warnings/errores${NC}"
        fi
    else
        echo -e "${RED}❌ Script $script_name no encontrado${NC}"
        echo "ERROR: Script no encontrado" >> "$REPORT_FILE"
    fi
    
    echo ""
}

# Función para mostrar opciones de menú
show_menu() {
    echo -e "${CYAN}Selecciona qué tests ejecutar:${NC}"
    echo "1) Tests básicos de memory leaks (rápido - ~5 min)"
    echo "2) Tests detallados de áreas críticas (medio - ~10 min)"
    echo "3) Tests de stress y edge cases (lento - ~20 min)"
    echo "4) Suite completa (todos los tests - ~35 min)"
    echo "5) Compilar minishell y ejecutar suite completa"
    echo "6) Solo verificar prerequisitos"
    echo "0) Salir"
    echo ""
    echo -n "Opción [1-6, 0 para salir]: "
}

# Función para mostrar resumen final
show_final_summary() {
    echo -e "${CYAN}================================================================${NC}"
    echo -e "${CYAN}                    RESUMEN FINAL${NC}"
    echo -e "${CYAN}================================================================${NC}"
    
    echo -e "${BLUE}📋 Reporte guardado en: ${YELLOW}$REPORT_FILE${NC}"
    
    if [ -f "$REPORT_FILE" ]; then
        local total_leaks=$(grep -c "LEAK DETECTADO\|MEMORY ERROR DETECTADO" "$REPORT_FILE" 2>/dev/null || echo "0")
        local total_timeouts=$(grep -c "TIMEOUT" "$REPORT_FILE" 2>/dev/null || echo "0")
        local total_ok=$(grep -c "Sin leaks detectados\|Sin errores de memoria" "$REPORT_FILE" 2>/dev/null || echo "0")
        
        echo -e "${GREEN}✅ Tests sin problemas: $total_ok${NC}"
        echo -e "${RED}❌ Memory leaks detectados: $total_leaks${NC}"
        echo -e "${YELLOW}⏱️  Timeouts: $total_timeouts${NC}"
        
        if [ $total_leaks -eq 0 ] && [ $total_timeouts -eq 0 ]; then
            echo -e "${GREEN}🎉 ¡EXCELENTE! No se detectaron memory leaks${NC}"
        else
            echo -e "${RED}⚠️  Se detectaron problemas que requieren atención${NC}"
        fi
    fi
    
    echo ""
    echo -e "${BLUE}💡 Próximos pasos:${NC}"
    echo "1. Revisar el reporte detallado en $REPORT_FILE"
    echo "2. Si hay leaks, usar gdb para debugging: gdb ./minishell"
    echo "3. Considerar usar AddressSanitizer: make CFLAGS='-fsanitize=address -g'"
    echo "4. Para análisis más profundo: valgrind --tool=massif ./minishell"
    echo ""
}

# Función principal
main() {
    show_header
    
    # Crear reporte inicial
    echo "REPORTE DE MEMORY LEAKS - MINISHELL" > "$REPORT_FILE"
    echo "Generado: $(date)" >> "$REPORT_FILE"
    echo "========================================" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    
    check_prerequisites
    
    while true; do
        show_menu
        read -r choice
        
        case $choice in
            1)
                run_test_suite "Tests Básicos" "memory_leak_tests.sh" "Tests generales de funcionalidad básica"
                ;;
            2)
                run_test_suite "Tests Detallados" "detailed_leak_tests.sh" "Análisis profundo de áreas críticas"
                ;;
            3)
                run_test_suite "Tests de Stress" "stress_leak_tests.sh" "Casos límite y stress testing"
                ;;
            4)
                echo -e "${PURPLE}🚀 Ejecutando suite completa de tests...${NC}"
                echo "Esto puede tomar hasta 35 minutos. ¿Continuar? [y/N]: "
                read -r confirm
                if [[ $confirm =~ ^[Yy]$ ]]; then
                    run_test_suite "Tests Básicos" "memory_leak_tests.sh" "Tests generales"
                    run_test_suite "Tests Detallados" "detailed_leak_tests.sh" "Análisis profundo"
                    run_test_suite "Tests de Stress" "stress_leak_tests.sh" "Casos límite"
                fi
                ;;
            5)
                echo -e "${BLUE}🔨 Compilando minishell...${NC}"
                cd .. && make clean && make
                if [ $? -eq 0 ]; then
                    cd tests
                    echo -e "${GREEN}✅ Compilación exitosa${NC}"
                    run_test_suite "Tests Básicos" "memory_leak_tests.sh" "Tests generales"
                    run_test_suite "Tests Detallados" "detailed_leak_tests.sh" "Análisis profundo"
                    run_test_suite "Tests de Stress" "stress_leak_tests.sh" "Casos límite"
                else
                    echo -e "${RED}❌ Error en compilación${NC}"
                    cd tests
                fi
                ;;
            6)
                check_prerequisites
                ;;
            0)
                echo -e "${BLUE}👋 Saliendo...${NC}"
                break
                ;;
            *)
                echo -e "${RED}❌ Opción inválida${NC}"
                ;;
        esac
        
        if [ "$choice" != "6" ] && [ "$choice" != "0" ]; then
            show_final_summary
            echo -e "${CYAN}Presiona Enter para volver al menú...${NC}"
            read -r
        fi
    done
}

# Ejecutar si es llamado directamente
if [ "${BASH_SOURCE[0]}" == "${0}" ]; then
    main "$@"
fi