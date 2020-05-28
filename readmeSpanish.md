# Serial PAD
Permite usar un teclado o ratón de PC en una PSX real conectada al puerto de mandos. Soporta:
<ul>
 <li><a href='#simulation'>Simulación</a></li>
 <li><a href='#padmodification'>Modificar mando</a></li>
 <li><a href="#padrealemulator">Mando físico en Emulador</a></li>
 <li><a href="#realpadrealconsole">Mando físico en consola real</a></li>
 <li><a href="#protocolo">Protocolo Ratón</a></li>
 <li><a href="#arduino">ARDUINO</a></li> 
 <li><a href="#uploadpsexe">Upload PsEXE</a></li>  
 <li><a href="#msdos">Uso de MSDOS</a></li>  
 <li><a href="#history">Historial</a></li>  
 <li><a href="#conclusion">Conclusion</a></li>  
</ul>
Las funcionalidades a destacar son:
<ul>
 <li><b>Ratón PS/2 (usb)</b> - 100% funcional, incluye pantalla de test</li>
 <li><b>Teclado PS/2 (usb)</b> - código funcional, falta pantalla de test</li> 
 <li><b>Comunicación lenta serie</b> - código funcional, falta pantalla de test</ul>
</ul>
<center><img src="preview/capturaTV.jpg"></center>
De esta forma se puede introducir datos por teclado o ratón en cualquier modelo de PSX o PsOne conectando un teclado a un puerto de mandos de la PSX, dándole una mayor autonomía, pudiendo actuar de ordenador personal, por ejemplo.<br>
<center><img src="preview/ps1mouse.jpg"></center>
El ratón y teclado que van a usarse son de PC, así que este proyecto, no va a hacer uso de ratones de PSX, es decir, que no es compatible con el ratón de PSX original, y sólo funcionará con nuestro programa que traduce pulsaciones de botones.<br>
Los resultados en máquina real, son impresionantes, dado que es muy rápido, tanto el ratón, como el teclado. Para subirlo a máquina real, lo más comodo es usar un cable <a href="http://hitmen.c02.at/html/psx_siocable.html">skywalker</a>, tal y como yo lo tengo, y un conversor de usb a serie, así como la versión modificada <b>send.exe</b> para Windows 64 bits.
<br><br>

<a name="simulation"><h2>Simulación</h2></a>
Para todo aquel que no quiera modificar un mando real de PSX, se puede realizar pruebas mediante emulación:
<ul>
 <li><a href="https://www.epsxe.com/">Emulador de PSX (ePSX)</a></li>
 <li><a href="https://www.autohotkey.com/">Auto Hot Key (Script teclado y ratón)</a></li>
</ul>
Una vez instalado el Auto Hot Key, podremos lanzar el KeyboardMouse.ahk, que traducirá los movimiento y pulsación de botones del ratón en pulsaciones de teclado, que el emulador ePSX interpretará como pulsación de botones del mando.<br>
<center><img src="preview/ePSXconfigPAD.gif"></center>
Debemos de configurar los gatillos del mando con las teclas:
<ul>
 <li><b>R1</b> - R</li>
 <li><b>R2</b> - T</li>
 <li><b>L1</b> - W</li>
 <li><b>L2</b> - E</li> 
</ul>
Si lanzamos el script KeyboardMouse.ahk en una ventana de comandos, veremos como al mover al ratón, se generan pulsaciones de teclas:
<center><img src="preview/autohotkey.gif"></center>
Debemos de tener cuidado, y asegurarnos de estar en la ventana del emulador ePSX, ya que cada vez que hagamos algo con el ratón, estará pulsando teclas.<br>
<center><img src="preview/captureEpsxMouse.gif"></center>
En el emulador, si lanzamos el main.exe, nos saldrá el menú con los botones:
<ul>
 <li><b>Fast</b> - Velocidad rápida del ratón</li>
 <li><b>Normal</b> - Velocidad normal</li>
 <li><b>Slow</b> - Velocidad lenta</li>
 <li><b>X</b> - Invierte el movimiento eje X</li>
 <li><b>Y</b> - Invierte el movimiento eje Y</li>
 <li><b>Left</b> - Zurdos</li>
</ul>
En la parte superior, nos aparecerá información de las coordenadas y botones del ratón, así como el estado del mando de la PSX.
Si pasamos por encima de los botones, se iluminarán, y si se hace click se activará.<br>
Debemos tener en cuenta, que la emulación no va bien, dado que el script de teclado y el emulador tiene unas demoras, que al final se traduce en unos movimientos no fluidos del ratón y fallos esporádicos. Sin embargo nos es muy útil para ver que al menos funciona.
<br><br>

<a name="padmodification"><h2>Modificar mando</h2></a>
Si queremos una respuesta fluida, debemos modificar el mando real.
<center><img src="preview/mandoCableBotones.jpg"></center>
Se van a usar los 4 botones (gatillos laterales), es decir, R1, R2, L1 y L2. El motivo se debe a que son los más fáciles de modificar sin falta de usar soldador, es decir, que cualquiera puede hacerlo. Estos botones estan unidos a la placa del mando con un hilo para cada uno de los botones, y otro para la masa. La forma de detectar un botón pulsada, es muy sencilla, tna sólo con unirlo con la masa.<br>
No todos los mandos usan el sistema de masa, así que tendremos que tener en cuenta si nuestro mando sirve para este proyecto.
<center><img src="preview/mandoR1R2.jpg"></center>
Debemos mirar con el multitester la masa, que se corresponde con la masa real del mando.<br>
Luego usaremos un sistema de buffer de transistores que activaran los botones. Dichos transistores seran controlados por un ARDUINO, de manera, que se leeran los datos del ratón PS/2 y se activarán los botones del mando.
Debemos cortar los cables (lo más facil) o bien soldar  la placa, una extensión de cables al exterior, para posteriormente conectar al buffer de transistores.
<center><img src="preview/pinoutPadPSXarduino.gif"></center>
La resistencia para la base del transistor es de 470 Ohmios, y el transistor nos sirven el 2n3904, que es el más básico y barato del mercado. Eso no quita, que podamos hacer lo mismo con optoacopladores o incluso relés sólidos.
<br><br>


<a name="padrealemulator"><h2>Mando físico en Emulador</h2></a>
Una vez actualizado el ARDUINO con el código del ratón, y conectado el ratón al PS/2, podemos hacer pruebas sobre el emulador ePSX, antes de probar con una PSX real, gracias a un conversor de mandos PSX a usb.
<center><img src="preview/PSXusb.jpg"></center>
Debemos de configurar el mando en el emulador de PSX, de manera que usemos los botones del mando, en lugar de las letras.
<center><img src="preview/ePSXconfigUsbPAD.gif"></center>
Y si todo es correcto, al ejecutar el main.exe, podremos usar el ratón.
<br><br>


<a name="realpadrealconsole"><h2>Mando físico en consola real</h2></a>
Se requiere los siguiente materiales
<ul>
 <li>Mando PSX botones R1,R2,L1,L2</li>
 <li>Adaptador usb a PS/2 o ratón PS/2</li>
 <li>Teclado PS/2</li>
 <li>Ratón PS/2</li>
 <li>Arduino Nano o UNO</li>
</ul>
Una vez todo conectado y arrancado el main.exe, al realizar movimientos del ratón, se podrá ver en la pantalla. Debemos garantizar que nuestro ratón sea compatible con una comunicación PS/2. No todos los ratones modernos aceptan una conversión de USB a PS/2. Para estar seguros, es tan sencillo como, depurar en ARDUINO siguiendo la traza de salida a la consola serie, descomentando el código.
<center><img src="preview/PS2toArduino.jpg"></center>
<br><br>


<a name="protocolo"><h2>Protocolo Ratón</h2></a>
Se ha usado un protocolo muy simple similar a como se usaban los ratones en los primeros COMMODORE y MSX por puerto de juegos, pero con varias mejoras de velocidad.<br>
Se envian cada 25 milisegundos 4 bits, que contienen:
<ul>
 <li>2 bits movimiento X</li>
 <li>2 bits movimiento Y</li>
</ul>
O bien:
<ul>
 <li>2 bits botón izquierdo</li>
 <li>2 bits botón derecho</li> 
</ul>

Los bits del movimiento pueden ser:
<ul>
 <li><b>00</b> - No hay movimiento</li>
 <li><b>01</b> - Movimiento positivo</li>
 <li><b>11</b> - Movimiento negativo</li>
</ul>
Para los botones, si se da la opción <b>10</b> es que el botón está activo.
Recordemos, que la PSX en cada retrazo vertical, se leen los datos del PAD, así que se leen entre 16 y 20 milisegundos.<br>
El código de demora de 25 milisegundos, está puesto en el Arduino, pero podemos cambiarlo a 50 milisegundos. Para el caso del emulador de teclado, está puesto a 50 milisegundos, dado que a tiempos inferiores, ocurren problemas de sincronismo.
<br><br>


<a href="arduino"><h2>ARDUINO</h2></a>
Se puede usar casi cualquier placa de ARDUINO, pero en concreto he realizado pruebas con el NANO, que dispone de 2 pines de interrupción, útiles para el PS/2.<br>
La disposición de pines es la siguiente:
<ul>
 <li>2 - CLK conector PS/2</li>
 <li>4 - pin datos conector PS/2</li>
 <li>5 - base transistor L1</li>
 <li>6 - base transistor L2</li>
 <li>7 - base transistor R1</li>
 <li>8 - base transistor R2</li> 
</ul>
Se ha usado una libreria muy básica de lectura del ratón <b>PS2Mouse.h</b>, que se encarga de leer el movimiento del ratón, así como el estado de los botones.<br>
Los pines activarán la base del transistor, que permitirá activar los botones del mando.
<br><br>
 


<a href="uploadpsexe"><h2>Upload PsEXE</h2></a>
Estoy realizando pruebas para usar este hack del mando como si fuera un canal de comunicaciones, estilo modem <b>(lento)</b> serie. Estoy usando un hack del bus de comunicaciones SPI, con un fake slave spi con arduino, de manera que no es necesario modificar el mando.
Y lo más importante, se permite realizar un upload de un ps-exe directamente  a memoria o a la memory card.
<center><img src="preview/capturaepsx.gif"></center>
Recordemos, que la PSX puede disponer de hasta 8 mandos PSX por medio de multitaps:
<ul>
 <li><b>8 x 14 botones x 50 </b>= 5600 bits/s</li>
 <li><b>8 x 14 botones x 60 </b>= 6720 bits/s</li>
 <li><b>8 x 16 botones (modo analogico) x 50 </b>= 6400 bits/s</li>
 <li><b>8 x 16 botones (modo analogico) x 60 </b>= 7680 bits/s</li>
 <li><b>8 x 48 bits (modo analógico) x 50 </b>= 19200 bits/s</li>
 <li><b>8 x 48 bits (modo analógico) x 60 </b>= 23040 bits/s</li>
</ul>
<center><img src="preview/linkPadSerial.jpg"></center>
Llevarlo a la práctica físicamente es bastante engorroso, pero es fácil de simular.
<br><br>

<h2>Simulacion</h2>
Para pruebas rápidas estoy usando un emulador de psx (epsx) y un script (autohotkey) que envia las pulsaciones de teclas para el emulador.
Se usan 8 botones del mando, junto con 1 más (L1) para el pulso (sincronia) del dato.

<center><table>
 <tr>
  <td>Button</td><td>Key</td><td>Bit</td>
 </tr>
 <tr>
  <td>L1</td><td>W</td><td>(sync)</td>
 </tr>
 <tr>  
  <td>L2</td><td>E</td><td>Q7</td>
 </tr>   
 <tr>  
  <td>R1</td><td>R</td><td>Q6</td>
 </tr>   
 <tr>     
  <td>R2</td><td>T</td><td>Q5</td>
 </tr>  
 <tr>  
  <td>Triangulo</td><td>D</td><td></td>
 </tr>  
 <tr>  
  <td>Cuadrado</td><td>S</td><td>Q3</td>
 <tr> 
 </tr>  
  <td>Circulo</td><td>X</td><td>Q2</td>
 <tr> 
 </tr>  
  <td>X</td><td>Z</td><td>Q1</td>
 <tr>      
  <td>Start</td><td>V</td><td>Q0</td>
 </tr>  
</table></center>
<br><br>

<h2>Otros frentes de desarrollo</h2>
Se realiza un ataque en varios frentes, desde el m&aacute;s sencillo, al m&aacute;s complejo:
<ul>
 <li><b>Control de teclado remoto y real</b> (puerto PC PS2) con arduino activando 9 botones del Gamepad, por medio
   de un protocolo muy simple de flanco ascendente y transistores, pero lento en velocidad. Se realizará un programa que envia los datos en msdos x86 100% compatible DOSBOX para el remoto via emulaci&oacute;n serie, así como la posibilidad de leer directamente de arduino el teclado con la libreria PS2keyboard.
  <center><img src="preview/mandoAnalizar.jpg"></center>
 </li>
 <li><b>Envio de PS-exe</b> a memoria de PSOne o a tarjeta de memoria de PSOne con arduino activando 9 botones del Gamepad,
  , por medio de un protocolo muy simple de flanco ascendente, pero lento en velocidad. Se realizará un programa que envia datos en msdos x86 100% compatible DOSBOX para el remoto via emulaci&oacute;n serie
 </li>
 <li><b>Carga de PS-exe</b> desde memorias de PSX (128 KB), con posibilidad de trozear el exe en varias tarjetas. Se realizará un programa en msdos x86 100% compatible DOSBOX para trocear el exe en múltiples .MCR de 128 KB. Dichos .MCR se puede guardar en tarjeta real desde PS2 mediante el <a href="https://gamesx.com/mccap/">sakura</a> de puerto paralelo (vmware con windows 95 y 98 con soporte puerto paralelo) o el <a href="https://github.com/ShendoXT/memcarduino">memcarduino</a>.
  <center><img src="preview/memecardAnalizar.jpg"></center>
 </li>
 <li>
  <b>Recepción de datos por medio de sonido</b> de la PSX, los 2 canales de audio. Se realizará un programa en psx que genera tonos con pulsos que pueden ser decodificados como datos binarios. La psx recibirá comandos remotos y enviará los datos como sonido.
 </li>
 <li>
  <b>Simulación de mando y memoria</b> psx por medio de arduino. He capturado los comandos del mando de la psx, así como los de la memoria, al usar un protocolo SPI a 250 Khz. Estoy realizando un programa en arduino que al recibir los comandos, envie las respuestas a un programa en la psone que interprete para cargar datos.  
  <center><img src="preview/mandoAnalizadorDigital.jpg"></center>
 </li>
</ul>
<br></br>

<a href="msdos"><h2>Uso de MSDOS</h2></a>
El motivo de usar MSDOS, es para lograr máxima compatibilidad con DOSBOX, de forma que se puede usar todo el kit de desarrollo psyq de 16bits, ya que así se puede usar en casi cualquier máquina actual que no sea PC. También se puede usar el msdosplayer para Windows 7 64 bits en adelante.
<br><br>

<a name="history"><h2>Historial</h2></a>
<ul>
 <li>2020/05/25 - 100% ratón PS/2</li>
 <li><b>En pruebas</b> - Simulación de PAD con teclado y emulador ePSX.</li> 
 <li>Código comentado para uso de teclado, y envio de ficheros (recibe 100 bytes).</li>
</ul>
<br><br>

<a name="conclusion"><h2>Conclusion</h2></a>
Se usa esta opción, porque mientras que la psx si dispone de puerto serie, la psone blanca, no. Se tiene por tanto que realizar modificaciones. Además se logra una alternativa al SIOCONS.
