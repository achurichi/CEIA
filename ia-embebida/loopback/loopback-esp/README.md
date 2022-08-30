## Preguntas

1. ¿Existe otra estrategia para acceder al puerto sería que permita obtener un loopback a nivel de byte (estrategia sin `printf`/`scanf`)?
2. ¿Cuáles son las ventajas y desventajas de la alternativa sin `printf`/`scanf`?
3. ¿Es posible implementar esta solución sin utilizar pooling, es decir mediante interrupciones[2]?
   3.1) En sus palabras ¿Cual es la ventaja de implementarlo con interrupciones ?
4. Luego de la lectura de [3][4] ¿Que otras mejoras consideraría evaluar?

[1] https://embeddedartistry.com/blog/2019/11/06/an-embedded-friendly-printf-implementation/

[2] https://www.itemis.com/en/yakindu/state-machine/documentation/user-guide/polling-vs-interrupts

[3] https://embeddedartistry.com/blog/2020/10/26/optimizing-power-consumption-in-battery-powered-devices/

[4] https://www.embedded.com/pushing-performance-limitations-in-microcontrollers/

## Respuestas

**1, 2**. El framework ESP-IDF provee las funciones `uart_write_bytes` y `uart_read_bytes` que pueden utilizarse para leer o escribir un arreglo de bytes. También se podría usar la librería printf [1] que implementa esta función con mejor performance. Pero esta librería necesita una funcion base que pueda escribir de a 1 byte, para lo cual se utilizaría `uart_write_bytes`, y ya que esta función permite enviar varios bytes carece de sentido utilizar la librería printf.

Algunas pruebas realizadas arrojaron los siguentes resultados:

- Tiempo de escritura de la función `printf` (por defecto, no la librería): 210ms.
- Tiempo de escritura de la función `uart_write_bytes`: 17ms.
- Tiempo de lectura por byte de la función `scanf`: 58ms.
- Tiempo de lectura por byte de la función `uart_read_bytes`: 11ms.

En cuanto a performance es recomendable utilizar `uart_write_bytes` y `uart_read_bytes` ya que los tiempos de escritura/lectura son mucho menores.

`printf` y `scanf` podrían utilizarse en los casos en los que se quiera realizar un código simple y facil de interpretar, con prósito de debuggear, o que se desee realizar código portable a otras plataformas, ya que estas son funciones estandarizadas.

**3**\. Si, es posible implementar interrupciones para la lectura de la UART ya que lo soporta.

La ventaja de utilizar interrupciones y tener código asíncrono es que este es no-bloqueante, es decir la ejecución del programa principal no ser verá interrumpida mientras no se dispare la interrupción.

En este caso se implementaron la interrupciones utilizando `xTaskCreate` y `xQueueReceive` para crear una tarea para procesar los datos que llegan por la UART.
En versiones anteriores de ESP-IDF se podía utilizar `uart_isr_register` para ejecutar una funcion handler cuando se reciban datos, desde la version 4.4 de ESP-IDF esta función quedó deprecada y los ejemplos que provee espressif hacen uso de `xTaskCreate` y `xQueueReceive`.

Ejemplo de espressif:

https://github.com/espressif/esp-idf/blob/master/examples/peripherals/uart/uart_events/main/uart_events_example_main.c

**4**\. Algunas mejoras que se podrían evaluar:

- Configurar los GPIO en low power mode para evitar corrientes de fuga.
- Usar los distintos sleep modes del micro para evitar consumir energia cuando no se esté ejecutando ninguna tarea.
- Desactivar periféricos que no se usen.
- Ver la posibilidad de utilizar DMA para escribir de la UART directamente a la memoria para que luego el tiempo de lectura del micro sea menor.
