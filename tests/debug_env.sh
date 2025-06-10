#!/bin/bash

MINISHELL="../Mini_2.0/minishell"

echo "=== DEBUGGING ENV COMMAND ==="

echo "1. Testing direct env command:"
echo -e "env\nexit" | $MINISHELL 2>&1 | head -10

echo -e "\n2. Testing env with grep HOME:"
echo -e "env | grep HOME\nexit" | $MINISHELL 2>&1

echo -e "\n3. Comparing with bash env | grep HOME:"
env | grep HOME

echo -e "\n4. Testing just the filter part of our test:"
result=$(echo -e "env | grep HOME\nexit" | $MINISHELL 2>/dev/null | grep -v "exit" | tail -n +2)
echo "Filtered result: '$result'"

echo -e "\n5. Testing if env command exists in minishell:"
echo -e "which env\nexit" | $MINISHELL 2>&1
