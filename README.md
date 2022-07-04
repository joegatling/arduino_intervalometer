# DIY Intervalometer
DIY Arduino intervalometer designed for use with Canon digital cameras. Unlike most intervalometers out there, this one has a clock in it and can be configured to start at a specific time.

# How To Use
<img src="/Media/SketchDeviceArrows.png" width="200px"><br/>
The only input on the device is the knob. Turn to highlight options, and click to select.

# Setup Screen
<img src="/Media/SketchScreenIdle.png" width="200px"><br/>
The setup screen allows is where the intervalometer is configured before the session begins.

| Icon | Meaning |
| ---- | ------- |
|<img src="/Media/SketchIconStart.png" width="40px">|The start settings allows either a delay (in hours, minutes, and seconds) or a start time. Once **GO** has been selected, the device will wait according to this setting before triggering the shutter for the first time. If this says "NOW", then the session will begin immediately after pressing **GO**.|
|<img src="/Media/SketchIconShutter.png" width="40px">|How long to hold the shutter for (in hours, minutes, and seconds). If this is "-", then the shutter will be briefly pressed. Focus and Shutter are triggered at the same time.|
|<img src="/Media/SketchIconInterval.png" width="40px">|How long to wait between exposures.|
|<img src="/Media/SketchIconFrames.png" width="40px">|How many exposures to take before ending the session. If this is "-", then the session will continue forever.|

**NOTE: The session is not active until the GO button has been selected.**

# Session Screen
<img src="/Media/SketchScreenRunning.png" width="200px"><br/>
When a session is active, the screen will display the current status.

<img src="/Media/SketchUnlock0.png" width="200px"><img src="/Media/SketchUnlock1.png" width="200px"><br/>
To end a session early, tap the knob, and then turn the wheel clockwise until the unlock bar fills entirely.

# Battery
The device can run directly from a USB power source, but can also accept a **LiPoly** battery. The device draws very little power as it enters a deep sleep between exposures.

**WARNING: Any LiPoly battery used must have a controller circuit, and when plugged in the red wire must be on the left.  Reversing the polarity will destroy the intervalometer.**

Compatible batteries include:
- [Polymer Lithium Ion Battery (LiPo) 3.7V 120mAh](https://core-electronics.com.au/lipo-polymer-lithium-ion-battery-120mah.html)
- [Lithium Ion Polymer Battery - 3.7v 500mAh](https://www.adafruit.com/product/1578)


# Timing
![Timing Diagram](/Media/TimingDiagram.png)
**Notes**
- Focus and Shutter are triggered at the same time. 

# Hardware
- 3.5mm output (for use with a 3.5mm -> Canon C3 cable)
- Micro USB connector (for charging and firmware updates)
- LCD screen (64x128)
- Clickable knob for input
- Adafruit Feather M0 controller



