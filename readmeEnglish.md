# Serial PAD
It allows you to use a PC keyboard or mouse on a real PSX connected to the control port. Supports:
<ul>
  <li><b>PS/2 Mouse (USB)</b> - 100% functional, includes test screen</li>
  <li><b>PS/2 keyboard (usb)</b> - function code, missing test display</li>
  <li><b>Slow serial communication</b> - function code, missing test display</li>
</ul>
<center><img src="preview/capturaTV.jpg"></center>
This way you can enter data by keyboard or mouse on any model of PSX or PsOne by connecting a keyboard to a control port of the PSX, giving it greater autonomy, being able to act as a personal computer, for example.<br>
<center><img src="preview/ps1mouse.jpg"></center>
The mouse and keyboard that will be used are from PC, so this project, will not make use of PSX mice, that is, it is not compatible with the original PSX mouse, and it will only work with our program that translates button clicks.<br>
The results in real machine, are impressive, since it is very fast, both the mouse, as the keyboard. To upload it to a real machine, the most comfortable way is to use a cable <a href="http://hitmen.c02.at/html/psx_siocable.html">skywalker</a>, as I have it, and a usb to serial converter, as well as the modified version <b>send.exe</b> for Windows 64 bits.
<br><br>

<h2>Simulation</h2>
For those who don't want to modify a real PSX controller, tests can be done by emulation:
<ul>
 <li><a href="https://www.epsxe.com/">PSX emulator (ePSX)</a></li>
 <li><a href="https://www.autohotkey.com/">Auto Hot Key (Script keyboard and mouse)</a></li>
</ul>
Once the Auto Hot Key is installed, we will be able to launch the KeyboardMouse.ahk, which will translate the mouse movements and button presses into keyboard strokes, which the ePSX emulator will interpret as button presses of the controller.<br>
<center><img src="preview/ePSXconfigPAD.gif"></center>
We must set the triggers of the control with the keys:
<ul>
 <li><b>R1</b> - R</li>
 <li><b>R2</b> - T</li>
 <li><b>L1</b> - W</li>
 <li><b>L2</b> - E</li> 
</ul>
If we launch the KeyboardMouse.ahk script in a command window, we'll see how moving the mouse generates keystrokes:
<center><img src="preview/autohotkey.gif"></center>
We must be careful, and make sure we are in the ePSX emulator window, because every time we do something with the mouse, it will be pressing keys.<br>
<center><img src="preview/captureEpsxMouse.gif"></center>
In the emulator, if we launch the main.exe, we will get the menu with the buttons:
<ul>
 <li><b>Fast</b> - Fast mouse speed</li>
 <li><b>Normal</b> - Normal speed</li>
 <li><b>Slow</b> - Slow speed</li>
 <li><b>X</b> - Reverses X-axis movement</li>
 <li><b>Y</b> - Reverse the Y-axis movement</li>
 <li><b>Left</b> - Left-handed</li>
</ul>
At the top, we will see information about the coordinates and mouse buttons, as well as the status of the PSX controller.
If we pass over the buttons, they will be illuminated, and if we click it will be activated.<br>
We must take into account that the emulation is not going well, since the keyboard script and the emulator have some delays, which in the end results in some non-flowing movements of the mouse and sporadic failures. However it is very useful to see that at least it works.
<br><br>

<h2>Modify control</h2>
If we want a fluid response, we must modify the actual command.
<center><img src="preview/mandoCableBotones.jpg"></center>
The 4 buttons (side triggers) will be used, i.e. R1, R2, L1 and L2. The reason for this is that they are the easiest to modify without using a soldering iron, i.e. anyone can do it. These buttons are attached to the control plate with a wire for each of the buttons, and another for the ground. The way to detect a pressed button is very simple, just by joining it to the ground.
Not all controllers use the mass system, so we'll have to consider whether our controller is suitable for this project.
<center><img src="preview/mandoR1R2.jpg"></center>
We must look with the multitester at the mass, which corresponds to the real mass of the control.
Then we will use a transistor buffer system that will activate the buttons. These transistors will be controlled by an ARDUINO, so that the data of the PS/2 mouse will be read and the buttons of the control will be activated.
We must cut the cables (the easiest way) or solder the board, an extension of cables to the outside, and then connect to the transistor buffer.
<center><img src="preview/pinoutPadPSXarduino.gif"></center>
The resistance for the base of the transistor is 470 Ohms, and the transistor serves us 2n3904, which is the most basic and cheapest on the market. That doesn't stop us from doing the same with optocouplers or even solid relays.
<br><br>


<h2>Physical control in Emulator</h2>
Once the ARDUINO has been updated with the mouse code, and connected to the PS/2, we can test the ePSX emulator, before trying a real PSX, thanks to a PSX to usb converter.
<center><img src="preview/PSXusb.jpg"></center>
We have to configure the controller in the PSX emulator, so that we use the buttons on the controller, instead of the letters.
<center><img src="preview/ePSXconfigUsbPAD.gif"></center>
And if everything is correct, when running the main.exe, we can use the mouse.
<br><br>


<h2>Physical control in real console</h2>
The following materials are required
<ul>
 <li>PSX control buttons R1,R2,L1,L2</li>
 <li>USB to PS/2 adapter or PS/2 mouse</li>
 <li>PS/2 keyboard</li>
 <li>PS/2 Mouse</li>
 <li>Arduino Nano or UNO</li>
</ul>
Once everything is connected and the main.exe is started, when you move the mouse, you will be able to see it on the screen. We must guarantee that our mouse is compatible with a PS/2 communication. Not all modern mice accept a conversion from USB to PS/2. To be sure, it is as simple as, debugging in ARDUINO following the output trace to the serial console, decomposing the code.
<center><img src="preview/PS2toArduino.jpg"></center>
<br><br>


<h2>Mouse Protocol</h2>
A very simple protocol similar to how mice were used in the early COMMODORE and MSX has been used per game port, but with several speed improvements.<br>
Every 25 milliseconds 4 bits are sent, containing:
<ul>
 <li>2 bits X movement</li>
 <li>2 bits Y movement</li>
</ul>
Or:
<ul>
 <li>2 bits left button</li>
 <li>2 bits right button</li> 
</ul>

Movement bits can be:
<ul>
 <li><b>00</b> - No movement</li>
 <li><b>01</b> - Positive movement</li>
 <li><b>11</b> - Negative movement</li>
</ul>
For the buttons, if the option <b>10</b> is given, the button is active.
Remember, that the PSX in each vertical delay, the data are read from the PAD, so they are read between 16 and 20 milliseconds.<br>
The 25 millisecond delay code is set on the Arduino, but we can change it to 50 milliseconds. In the case of the keyboard emulator, it is set to 50 milliseconds, since at lower times, synchronism problems occur.
<br><br>
