# Sample Producer-Consumer App (ESP32-C6)

This application is a simple example of a single producer (the button triggering an event), single consumer (finite state machine) with no real need for an event dispatcher) and also 
a board support package (in this case, handling the two LEDs). 

Logic analyzer output shown below - clearly shows the signal representations of the LEDs and button presses after manually pressing the button. As per the title, channel 0 
represents the RED Led, channel 1 is the GREEN Led and channel 2 is the button (which triggers the interrupt on the falling edge). This can be examined in conjunction with 
the serial output (see serial_out.txt).

<img width="1168" height="412" alt="Ch 0 is RED - Ch 1 is GREEN - Ch 2 is BUTTON" src="https://github.com/user-attachments/assets/d3a675da-2198-47eb-aa35-150bf49b1675" />

Note: The value of the logic analyzer (or 4 channel oscilloscope) isn't just the clear representation of the signals over time but also the delays. In an earlier version of 
the code, I had forgotten to remove a delay from the loop (showing an ugly delay between the interrupt firing and the LEDs turning on or off). I spent too much time investigating 
switch debounce with the oscilloscope.

Another note: Buy a better scope!

