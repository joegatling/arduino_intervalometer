# arduino_intervalometer
DIY Arduino intervalometer for a Cannon 5D camera

# Features (planned)

**General**
- Shutter button 
- Screen for adjusting settings, seeing current status, etc... 

**Interalometer**
- Initial delay before first exposure
- Delay between exposures
- Duration of exposure
- Start exposures:
  - Immediately
  - After a delay
  - At a specified date & time 
- End exposures after:
  - A certain number of exposures
  - A total amount of time has passed
  - At a specified date & time

# Timing
![Timing Diagram](/Media/TimingDiagram.png)
**Notes**
- Initial delay occurs after the start and date time. Set this to 0 in order to have the sequence start right away
- The first focus will overlap with the initial delay in order to ensure that first exposure occurs on time.

# Connections
- 3.5mm output (for use with a 3.5mm -> Canon C3 cable)
- Micro USB connector (for charging and firmware updates)



