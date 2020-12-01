# Unidad Efectos UNO

Unidad o pedal de efectos realizado con un Arduino UNO, controlado con una interfaz gráfica portable realizada con Python.

# Detalles
La unidad de efectos se enchufa al instrumento (guitarra, piano, etc.), modifica su sonido según lo indicado por el/la ususario/a en la interfaz gráfica para PC y luego envía el resultado a un amplificado, también conectado al "pedal".

La unidad de efectos consiste de un Arduino UNO. Los efectos se realizan vía software con C/C++. La señal de entrada del instrumento y la de salida modificada al amplificador deben pasar por un circuito de acondicionamiento para eliminar el ruido. La interfaz gráfica portable, realizada en Python para PC, es la que le indica al pedal qué efecto debe realizar en determinado momento. La comunicación entre ambos es vía USB.

Para más detalles mirar la [presentación]().
