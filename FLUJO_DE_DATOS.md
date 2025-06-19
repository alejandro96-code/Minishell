# FLUJO DE DATOS MINISHELL - GUÍA PASO A PASO

## 🚀 INICIO: main() - Punto de entrada

```
1. INICIO AQUÍ → main()
   ├─ Copia variables de entorno (envp → env)
   ├─ Configura manejadores de señales
   └─ Entra al bucle principal
```

---

## 🔄 BUCLE PRINCIPAL: main_loop()

```
2. BUCLE INFINITO
   ├─ Genera prompt personalizado: "user@host:path$ "
   ├─ Lee entrada del usuario: readline()
   ├─ Si entrada vacía → vuelve al paso 2
   ├─ Si entrada = "exit" → termina programa
   └─ Si hay entrada → CONTINÚA AL PASO 3
```

---

## 🎯 DECISIÓN PRINCIPAL: process_input()

```
3. ANALIZA LA ENTRADA
   ├─ ¿Contiene "|" (pipes)?
   │   ├─ SÍ → VA AL PASO 4 (Pipeline)
   │   └─ NO → VA AL PASO 5 (Comando Simple)
   └─ [IMPORTANTE: Aquí se decide todo el flujo posterior]
```

---

## 🔗 RUTA A: PIPELINE (comandos con pipes)

```
4. PROCESAMIENTO DE PIPELINE
   ├─ Divide entrada por "|": "ls | grep .c" → ["ls", "grep .c"]
   ├─ Valida sintaxis (no puede terminar en "|")
   ├─ Para cada comando en el pipeline:
   │   ├─ Crea pipe entre comandos
   │   ├─ Fork → proceso hijo
   │   ├─ Conecta stdout del anterior con stdin del siguiente
   │   └─ Ejecuta comando
   ├─ Espera a que terminen todos los procesos
   └─ TERMINA → VUELVE AL PASO 2
```

**Ejemplo visual:**
```
"ls | grep .c | wc -l"
     ↓
[ls] → pipe1 → [grep .c] → pipe2 → [wc -l]
```

---

## 📝 RUTA B: COMANDO SIMPLE

```
5. PROCESAMIENTO DE COMANDO SIMPLE
   ├─ PASO 5.1: Parsing inicial
   ├─ PASO 5.2: Expansión
   ├─ PASO 5.3: Redirecciones
   └─ PASO 5.4: Ejecución
```

### 5.1 PARSING INICIAL
```
   ├─ Valida comillas: "echo 'hello" → ERROR
   ├─ Valida redirecciones: "echo > " → ERROR
   ├─ Divide en argumentos: "echo hello world" → ["echo", "hello", "world"]
   └─ Identifica si es builtin: "echo" → SÍ, "ls" → NO
```

### 5.2 EXPANSIÓN DE VARIABLES
```
   ├─ Busca variables: "echo $USER" → "echo alejandro"
   ├─ Procesa comillas:
   │   ├─ Simples: 'hello $USER' → "hello $USER" (sin expansión)
   │   └─ Dobles: "hello $USER" → "hello alejandro" (con expansión)
   ├─ Variables especiales: "$?" → código de salida anterior
   └─ Quita comillas finales: "hello" → hello
```

### 5.3 MANEJO DE REDIRECCIONES
```
   ├─ Busca redirecciones en argumentos:
   │   ├─ ">" → PASO 5.3a (Redirección de salida)
   │   ├─ ">>" → PASO 5.3b (Redirección de salida append)
   │   ├─ "<" → PASO 5.3c (Redirección de entrada)
   │   └─ "<<" → PASO 5.3d (Heredoc)
   ├─ Guarda descriptores originales (backup)
   ├─ Aplica redirecciones según el tipo
   └─ Quita redirecciones de los argumentos
```

#### 5.3a REDIRECCIÓN DE SALIDA ">"
```
"echo hello > file.txt"
     ↓
1. Detecta ">" en argumentos
2. Abre/crea "file.txt" (modo escritura)
3. ¿Se pudo abrir?
   ├─ NO → Error "Permission denied" → TERMINA
   └─ SÍ → CONTINÚA
4. Guarda stdout original
5. dup2(file_fd, STDOUT) → stdout apunta a file.txt
6. Quita ">" y "file.txt" de argumentos
7. args final: ["echo", "hello"]
```

#### 5.3b REDIRECCIÓN APPEND ">>"
```
"echo hello >> file.txt"
     ↓
1. Detecta ">>" en argumentos  
2. Abre "file.txt" (modo append, crea si no existe)
3. ¿Se pudo abrir?
   ├─ NO → Error "Permission denied" → TERMINA
   └─ SÍ → CONTINÚA
4. Guarda stdout original
5. dup2(file_fd, STDOUT) → stdout apunta a file.txt (final)
6. Quita ">>" y "file.txt" de argumentos
7. args final: ["echo", "hello"]
```

#### 5.3c REDIRECCIÓN DE ENTRADA "<"
```
"cat < input.txt"
     ↓
1. Detecta "<" en argumentos
2. Abre "input.txt" (modo lectura)
3. ¿Existe el archivo?
   ├─ NO → Error "No such file or directory" → TERMINA
   └─ SÍ → CONTINÚA
4. Guarda stdin original
5. dup2(file_fd, STDIN) → stdin lee desde input.txt
6. Quita "<" y "input.txt" de argumentos
7. args final: ["cat"]
```

#### 5.3d HEREDOC "<<"
```
"cat << EOF"
     ↓
1. Detecta "<<" en argumentos
2. Extrae delimitador: "EOF"
3. Crea pipe temporal: pipe(pipefd)
4. BUCLE de lectura:
   ├─ Muestra prompt: "> "
   ├─ Lee línea del usuario
   ├─ ¿Línea == "EOF"?
   │   ├─ SÍ → Termina bucle
   │   └─ NO → Escribe línea al pipe + expansión de variables
   └─ Repite
5. Cierra write_end del pipe
6. dup2(read_end, STDIN) → stdin lee del pipe
7. Quita "<<" y "EOF" de argumentos
8. args final: ["cat"]
```

**Ejemplo visual de heredoc:**
```
$ cat << EOF
> Hello $USER
> This is line 2
> EOF
     ↓
pipe: "Hello alejandro\nThis is line 2\n"
     ↓
cat lee del pipe → imprime contenido
```

#### 5.3e MÚLTIPLES REDIRECCIONES
```
"cat < input.txt > output.txt"
     ↓
1. Procesa "<" → stdin desde input.txt
2. Procesa ">" → stdout hacia output.txt  
3. args final: ["cat"]
4. cat lee de input.txt, escribe a output.txt
```

```
"echo hello > file1.txt > file2.txt"
     ↓
1. Procesa primer ">" → stdout hacia file1.txt
2. Procesa segundo ">" → stdout hacia file2.txt (sobrescribe anterior)
3. args final: ["echo", "hello"]
4. Resultado: solo se escribe en file2.txt
```

### 5.4 EJECUCIÓN
```
   ├─ ¿Es builtin?
   │   ├─ SÍ → VA AL PASO 6 (Builtin)
   │   └─ NO → VA AL PASO 7 (Externo)
   └─ Restaura descriptores originales
```

---

## 🏠 PASO 6: EJECUTAR BUILTIN

```
6. COMANDOS INTERNOS
   ├─ echo → imprime argumentos
   ├─ cd → cambia directorio
   ├─ pwd → imprime directorio actual
   ├─ env → muestra variables de entorno
   ├─ export → exporta variables
   ├─ unset → elimina variables
   ├─ exit → termina el programa
   └─ TERMINA → Restaura redirecciones → VUELVE AL PASO 2
```

---

## 🌍 PASO 7: EJECUTAR COMANDO EXTERNO

```
7. COMANDOS EXTERNOS
   ├─ Busca el comando en PATH: "ls" → "/bin/ls"
   ├─ ¿Encontró el comando?
   │   ├─ NO → Error "command not found" → VUELVE AL PASO 2
   │   └─ SÍ → CONTINÚA
   ├─ Fork: crea proceso hijo
   ├─ PROCESO HIJO:
   │   ├─ Resetea señales a comportamiento por defecto
   │   ├─ Ejecuta: execve("/bin/ls", args, env)
   │   └─ Si execve falla → exit(127)
   ├─ PROCESO PADRE:
   │   ├─ Ignora señales (SIGINT, SIGQUIT)
   │   ├─ Espera al hijo: waitpid()
   │   ├─ Restaura manejo de señales
   │   └─ Obtiene código de salida
   └─ TERMINA → VUELVE AL PASO 2
```

---

## ⚡ MANEJO DE SEÑALES (en paralelo)

```
SEÑALES - Funcionan en paralelo durante todo el proceso:

Ctrl+C (SIGINT):
├─ En readline → nueva línea, limpia entrada
├─ En ejecución → mata proceso hijo
└─ g_signal_received = SIGINT

Ctrl+\ (SIGQUIT):
├─ En readline → ignorado
├─ En ejecución → mata proceso hijo + "Quit (core dumped)"
└─ g_signal_received = SIGQUIT
```

---

## 🧹 LIMPIEZA Y MEMORIA

```
LIMPIEZA AUTOMÁTICA en cada paso:
├─ readline() → siempre hacer free()
├─ Variables expandidas → free() después de usar
├─ Argumentos procesados → free() al final
├─ Pipes → close() todos los file descriptors
└─ Al salir del programa → libera entorno completo
```

---

## 📋 EJEMPLOS PASO A PASO

### EJEMPLO 1: "echo hello"
```
INICIO → main_loop → readline("echo hello")
     ↓
PASO 3: ¿tiene pipes? NO → Comando simple
     ↓
PASO 5.1: Parsing → ["echo", "hello"]
PASO 5.2: Expansión → sin cambios
PASO 5.3: Redirecciones → ninguna
PASO 5.4: ¿builtin? SÍ
     ↓
PASO 6: builtin_echo() → write(1, "hello\n", 6)
     ↓
TERMINA → VUELVE AL PASO 2
```

### EJEMPLO 2: "ls | grep .c"
```
INICIO → main_loop → readline("ls | grep .c")
     ↓
PASO 3: ¿tiene pipes? SÍ → Pipeline
     ↓
PASO 4: División → ["ls", "grep .c"]
     ↓ 
Fork 1: ls → stdout conectado a pipe
Fork 2: grep .c → stdin desde pipe, stdout a terminal
     ↓
Esperar procesos → VUELVE AL PASO 2
```

### EJEMPLO 3: "echo $USER > file.txt"
```
INICIO → main_loop → readline("echo $USER > file.txt")
     ↓
PASO 3: ¿tiene pipes? NO → Comando simple
     ↓
PASO 5.1: Parsing → ["echo", "$USER", ">", "file.txt"]
PASO 5.2: Expansión → ["echo", "alejandro", ">", "file.txt"]
PASO 5.3: Redirecciones → 
    ├─ Detecta ">" → redirección de salida
    ├─ Abre "file.txt" para escritura
    ├─ dup2(file_fd, STDOUT) → stdout apunta a file.txt
    └─ args → ["echo", "alejandro"]
PASO 5.4: ¿builtin? SÍ
     ↓
PASO 6: builtin_echo() → write(1, "alejandro\n", 10) → va a file.txt
     ↓
TERMINA → Restaura stdout → VUELVE AL PASO 2
```

### EJEMPLO 4: "cat << EOF"
```
INICIO → main_loop → readline("cat << EOF")
     ↓
PASO 3: ¿tiene pipes? NO → Comando simple
     ↓
PASO 5.1: Parsing → ["cat", "<<", "EOF"]
PASO 5.2: Expansión → sin cambios
PASO 5.3: Redirecciones (Heredoc) →
    ├─ Detecta "<<" → heredoc con delimitador "EOF"
    ├─ Crea pipe temporal
    ├─ BUCLE: Muestra "> ", lee líneas hasta "EOF"
    ├─ Escribe líneas al pipe con expansión de variables
    ├─ dup2(pipe_read, STDIN) → stdin lee del pipe
    └─ args → ["cat"]
PASO 5.4: ¿builtin? NO
     ↓
PASO 7: Comando externo → /bin/cat lee del pipe → imprime contenido
     ↓
TERMINA → Restaura stdin → VUELVE AL PASO 2
```

### EJEMPLO 5: "ls | grep .c > results.txt"
```
INICIO → main_loop → readline("ls | grep .c > results.txt")
     ↓
PASO 3: ¿tiene pipes? SÍ → Pipeline
     ↓
PASO 4: División → ["ls", "grep .c > results.txt"]
     ↓
Fork 1: "ls" → stdout conectado a pipe
Fork 2: "grep .c > results.txt" →
    ├─ stdin desde pipe (de ls)
    ├─ Procesa redirección ">" → stdout a results.txt
    └─ args final: ["grep", ".c"]
     ↓
Resultado: ls → pipe → grep → results.txt
Esperar procesos → VUELVE AL PASO 2
```

---

## 🎯 RESUMEN VISUAL COMPLETO

```
      INICIO
         │
    ┌────▼────┐
    │ main()  │
    └────┬────┘
         │
    ┌────▼────┐
    │readline │
    └────┬────┘
         │
    ┌────▼────┐
    │ ¿pipes? │
    └─┬─────┬─┘
      │     │
     SÍ    NO
      │     │
   ┌──▼─┐ ┌─▼────────┐
   │PIPE│ │SIMPLE CMD│
   └──┬─┘ └─┬────────┘
      │     │
      │  ┌──▼──┐
      │  │parse│
      │  └──┬──┘
      │     │
      │  ┌──▼───┐
      │  │expand│
      │  └──┬───┘
      │     │
      │  ┌──▼────┐
      │  │redir  │◄─── >, >>, <, <<
      │  └──┬────┘
      │     │
      │  ┌──▼────┐
      │  │builtin?
      │  └─┬───┬─┘
      │   SÍ  NO
      │    │   │
      │ ┌──▼┐ ┌▼───┐
      │ │INT│ │EXT │
      │ └──┬┘ └┬───┘
      │    │   │
      └────┼───┘
           │
      ┌────▼────┐
      │ CLEANUP │◄─── Restaura fd
      └────┬────┘
           │
      ┌────▼────┐
      │ REPEAT  │
      └─────────┘
```

## 📋 CHEAT SHEET DE REDIRECCIONES

| Símbolo | Nombre | Efecto | Ejemplo |
|---------|--------|--------|---------|
| `>` | Redirección salida | stdout → archivo (sobrescribe) | `echo hi > file` |
| `>>` | Redirección append | stdout → archivo (añade al final) | `echo hi >> file` |
| `<` | Redirección entrada | archivo → stdin | `cat < file` |
| `<<` | Heredoc | entrada temporal → stdin | `cat << EOF` |

## 🚨 PUNTOS CRÍTICOS DE REDIRECCIONES

1. **Orden de procesamiento**: Se procesan de izquierda a derecha
2. **Backup de descriptores**: Siempre se guardan los originales antes de cambiar
3. **Restauración**: Al final se restauran los descriptores originales
4. **Manejo de errores**: Si falla una redirección, no se ejecuta el comando
5. **Heredoc**: Es el único que requiere interacción del usuario (bucle de lectura)
6. **Múltiples redirecciones**: La última de cada tipo prevalece

**¡SIMPLE!** Siempre empiezas en main(), decides si hay pipes o no, procesas según corresponda, ejecutas, limpias y repites. 🔄
