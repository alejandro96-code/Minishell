# 🧪 Suite de Tests de Memory Leaks para Minishell

Esta suite de tests está diseñada para detectar **todos los posibles memory leaks** en tu implementación de minishell usando **valgrind** y casos de prueba exhaustivos.

## 📋 Contenido de la Suite

### Scripts Principales

1. **`run_all_leak_tests.sh`** - Script maestro con menú interactivo
2. **`memory_leak_tests.sh`** - Tests básicos y generales (~5 min)
3. **`detailed_leak_tests.sh`** - Tests detallados de áreas críticas (~10 min)
4. **`stress_leak_tests.sh`** - Tests de stress y edge cases (~20 min)

### Áreas Cubiertas por los Tests

#### 🔧 Tests Básicos
- Comandos simples y builtins
- Variables de entorno
- Parsing básico de comandos
- Pipes simples
- Redirecciones básicas

#### 🔍 Tests Detallados
- **Gestión de Environment**: `copy_env_array()`, `add_env_value()`, `remove_env_entry()`
- **Parsing y Expansión**: `expand_variable()`, `process_quotes_and_vars()`, `remove_quotes()`
- **Pipes y Procesos**: `split_and_validate_commands()`, `execute_child_command()`
- **Redirecciones**: `heredoc()`, `write_expanded_line()`
- **Error Handling**: Limpieza de memoria en errores
- **Cleanup**: `cleanup_and_exit()`, liberación al salir

#### ⚡ Tests de Stress
- Muchas variables de entorno (100+ exports)
- Comandos muy largos (4096+ caracteres)
- Pipes anidados profundos (20+ niveles)
- Redirecciones múltiples simultáneas
- Edge cases críticos (strings máximos, variables recursivas)
- Fragmentación de memoria
- Condiciones de carrera
- Recovery de errores

## 🚀 Uso Rápido

### Instalación de Prerequisitos

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install valgrind

# Fedora/RHEL
sudo dnf install valgrind

# macOS (via Homebrew)
brew install valgrind
```

### Ejecución Simple

```bash
# Dar permisos de ejecución
chmod +x tests/*.sh

# Ejecutar script maestro (recomendado)
cd tests
./run_all_leak_tests.sh

# O ejecutar tests individuales
./memory_leak_tests.sh        # Solo tests básicos
./detailed_leak_tests.sh      # Solo tests detallados
./stress_leak_tests.sh        # Solo tests de stress
```

## 📊 Interpretación de Resultados

### ✅ Sin Memory Leaks
```
🧪 Test: builtin echo simple
✅ Sin leaks detectados
```

### ❌ Memory Leak Detectado
```
🧪 Test: export múltiple
❌ LEAK DETECTADO
Detalles del leak:
==12345== 48 bytes in 3 blocks are definitely lost in loss record 15 of 25
==12345==    at 0x483B7F3: malloc (vg_replace_malloc.c:307)
==12345==    at 0x109ABC: add_env_value (export_utils2.c:45)
```

### ⏱️ Timeout (Posible Problema)
```
🧪 Test: pipes anidados profundos
⏱️ TIMEOUT (puede indicar leak o bucle infinito)
```

## 🔧 Casos Específicos Que Detectan los Tests

### Memory Leaks Comunes en Minishell

1. **Environment Variables**
   ```c
   // PROBLEMA: No liberar env array al modificar
   char **new_env = add_env_value("VAR=val", env);
   // SOLUCIÓN: Liberar el env anterior
   ```

2. **Parsing de Comandos**
   ```c
   // PROBLEMA: No liberar argv después de split
   char **args = split_command_args(input);
   // SOLUCIÓN: Usar ft_free_split(args)
   ```

3. **Pipes y Procesos**
   ```c
   // PROBLEMA: No liberar command arrays en pipes
   char **commands = split_by_pipes(input);
   // SOLUCIÓN: Liberar cada comando y el array
   ```

4. **Expansión de Variables**
   ```c
   // PROBLEMA: No liberar buffers temporales
   char *expanded = expand_variable(var, env);
   // SOLUCIÓN: free(expanded) después de usar
   ```

5. **Redirecciones y Heredoc**
   ```c
   // PROBLEMA: No liberar líneas de heredoc
   char *line = get_next_line(fd);
   // SOLUCIÓN: free(line) en cada iteración
   ```

## 🛠️ Debugging Memory Leaks

### Usar Valgrind Manualmente
```bash
# Análisis básico
valgrind --leak-check=full ./minishell

# Análisis detallado con orígenes
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell

# Guardar en archivo
valgrind --leak-check=full --log-file=valgrind.log ./minishell
```

### Usar AddressSanitizer
```bash
# Compilar con AddressSanitizer
make CFLAGS="-fsanitize=address -g -O1"

# Ejecutar (detecta más errores que valgrind)
./minishell
```

### Usar GDB para Debugging
```bash
# Compilar con símbolos de debug
make CFLAGS="-g -O0"

# Ejecutar con gdb
gdb ./minishell
(gdb) run
(gdb) bt  # backtrace cuando hay error
```

## 📈 Optimización de Memoria

### Técnicas Recomendadas

1. **Cleanup Functions**
   ```c
   void cleanup_command(t_command *cmd) {
       if (!cmd) return;
       ft_free_split(cmd->argv);
       free(cmd);
   }
   ```

2. **Error Handling**
   ```c
   char **result = malloc(sizeof(char*) * count);
   if (!result) {
       cleanup_partial_allocation(partial_data);
       return NULL;
   }
   ```

3. **RAII Pattern (Resource Acquisition Is Initialization)**
   ```c
   void process_with_cleanup(char *input) {
       char **args = NULL;
       char *expanded = NULL;
       
       args = split_command_args(input);
       if (!args) goto cleanup;
       
       expanded = expand_variables(args[0], env);
       if (!expanded) goto cleanup;
       
       // ... usar args y expanded ...
       
   cleanup:
       ft_free_split(args);
       free(expanded);
   }
   ```

## 🎯 Checklist de Memory Safety

- [ ] Cada `malloc()` tiene su `free()` correspondiente
- [ ] Cada `ft_strdup()` se libera después de usar
- [ ] Arrays de strings se liberan con `ft_free_split()`
- [ ] Variables de entorno se liberan al modificar
- [ ] Buffers temporales de parsing se liberan
- [ ] Descriptores de archivo se cierran
- [ ] Procesos hijos se esperan con `wait()`
- [ ] Señales se manejan sin leaks
- [ ] Cleanup completo en `exit`

## 🚨 Solución de Problemas Comunes

### "valgrind: command not found"
```bash
sudo apt-get install valgrind
```

### "minishell not found"
```bash
cd .. && make && cd tests
```

### Tests muy lentos
```bash
# Ejecutar solo tests básicos
./memory_leak_tests.sh
```

### Muchos false positives de readline
Los tests incluyen supresiones automáticas para readline. Si sigues viendo leaks de readline, son normales y no cuentan como errores de tu código.

## 📁 Archivos Generados

- `leak_report_YYYYMMDD_HHMMSS.txt` - Reporte completo de la ejecución
- `*.tmp` - Archivos temporales (se limpian automáticamente)
- `readline.supp` - Archivo de supresiones de valgrind

## 🤝 Contribuir

Para añadir nuevos tests:

1. Agregar función `run_test()` en el script apropiado
2. Seguir el patrón de naming existente
3. Incluir descripción del caso que detecta
4. Añadir timeout apropiado para tests largos

---

**💡 Consejo**: Ejecuta estos tests regularmente durante el desarrollo, no solo al final. Es más fácil arreglar leaks cuando el código está fresco en tu mente.

**🎯 Meta**: El objetivo es llegar a **0 memory leaks** en todos los tests. ¡Tu minishell debería ser tan robusto como bash!