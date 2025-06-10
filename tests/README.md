# 🧪 Suite de Pruebas para Minishell

Esta carpeta contiene una suite completa de pruebas automatizadas para validar el funcionamiento de tu minishell.

## 📁 Estructura de Archivos

- `test_runner.sh` - Script principal que ejecuta todas las pruebas
- `test_builtins.sh` - Pruebas para comandos builtin (echo, pwd, cd, env, export, unset, exit)
- `test_pipes.sh` - Pruebas para pipelines simples y múltiples
- `test_redirections.sh` - Pruebas para redirecciones de entrada/salida
- `test_variables.sh` - Pruebas para variables de entorno y expansión
- `test_quotes.sh` - Pruebas para manejo de comillas simples y dobles
- `test_edge_cases.sh` - Pruebas para casos especiales y límite

## 🚀 Uso

### Ejecutar todas las pruebas:
```bash
cd test_suite
./test_runner.sh
```

### Ejecutar un test específico:
```bash
./test_runner.sh test_builtins.sh
# o directamente:
./test_builtins.sh
```

### Ejecutar con ruta personalizada al minishell:
```bash
./test_builtins.sh /ruta/a/tu/minishell
```

## 📊 Interpretación de Resultados

- ✅ **PASS**: La prueba pasó correctamente
- ❌ **FAIL**: La prueba falló, se muestra la diferencia esperada vs obtenida
- 🟡 **TIMEOUT**: La prueba se colgó y fue terminada automáticamente

## 🔧 Personalización

Puedes modificar los scripts para:
- Agregar nuevos casos de prueba
- Cambiar el timeout de las pruebas
- Personalizar la salida y colores
- Agregar comparaciones más específicas

## 📝 Notas

- Las pruebas comparan tu minishell con bash cuando es posible
- Los timeouts previenen que las pruebas se cuelguen
- Se crean archivos temporales que se limpian automáticamente
- Los resultados muestran tanto éxitos como fallos detallados
- EJECUTA LOS TESTS DESDE ESTA CARPETA !!!!!!!!!!!!!!!!!!!!!
