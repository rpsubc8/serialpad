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
