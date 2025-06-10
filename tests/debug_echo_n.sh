#!/bin/bash

MINISHELL="../Mini_2.0/minishell"

echo "=== DEBUGGING ECHO -n ==="

echo "1. Testing echo -n directly:"
echo -e "echo -n hello\nexit" | $MINISHELL | xxd

echo -e "\n2. Testing regular echo:"
echo -e "echo hello\nexit" | $MINISHELL | xxd

echo -e "\n3. Bash comparison:"
echo -n hello | xxd

echo -e "\n4. Raw output with cat -A:"
echo -e "echo -n hello\nexit" | $MINISHELL | cat -A
