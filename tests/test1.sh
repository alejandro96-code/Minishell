#!/bin/bash

# Ruta al ejecutable de tu minishell
MINISHELL="./minishell"

# Archivos temporales para capturar salidas
TMP_MINISHELL_OUTPUT=$(mktemp)
TMP_MINISHELL_CLEANED=$(mktemp)
TMP_BASH_OUTPUT=$(mktemp)

# Lista de comandos a probar
COMMANDS=(
    "echo hola mundo"
    "ls -l"
    "pwd"
    "export VAR=test"
    "echo \$VAR"
    "unset VAR"
    "echo \$VAR"
    "cd .."
    "pwd"
    "cat nonexistentfile"
)

# Colores para salida
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo "🔍 Comparando comandos entre minishell y bash..."

for CMD in "${COMMANDS[@]}"; do
    echo -e "\n🔹 Comando: $CMD"

    # Ejecutar en minishell
    (echo "$CMD"; echo "exit") | $MINISHELL > "$TMP_MINISHELL_OUTPUT" 2>&1
    EXIT_MINISHELL=$?

    # Limpiar salida de minishell:
    # - quitar líneas que parecen ser el prompt (usuario@host~...)
    # - quitar líneas que son solo "exit"
    grep -vE "^[^ ]+@[^ ]+~.*:.*$|^exit$" "$TMP_MINISHELL_OUTPUT" > "$TMP_MINISHELL_CLEANED"

    # Ejecutar en bash
    bash -c "$CMD" > "$TMP_BASH_OUTPUT" 2>&1
    EXIT_BASH=$?

    # Comparar salidas
    if diff -q "$TMP_MINISHELL_CLEANED" "$TMP_BASH_OUTPUT" > /dev/null && [ $EXIT_MINISHELL -eq $EXIT_BASH ]; then
        echo -e "${GREEN}✅ Salida y código de salida coinciden${NC}"
    else
        echo -e "${RED}❌ Diferencias detectadas${NC}"
        echo -e "${RED}-- Minishell --${NC}"
        cat "$TMP_MINISHELL_CLEANED"
        echo -e "${RED}-- Bash --${NC}"
        cat "$TMP_BASH_OUTPUT"
        echo -e "${RED}Código salida Minishell: $EXIT_MINISHELL, Bash: $EXIT_BASH${NC}"
    fi
done

# Limpiar archivos temporales
rm "$TMP_MINISHELL_OUTPUT" "$TMP_MINISHELL_CLEANED" "$TMP_BASH_OUTPUT"
