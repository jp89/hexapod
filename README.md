- [Intro](#intro)
- [Plans / specification](#specification)
- [Hardware](#hardware)
- [Software](#software)

Intro
-----

This repository holds project of a simple 6-legged walking robot.
I'm Robotics graduate but during studies never had time to do something
like this just for fun, as a hobby. By developing this project I'm aiming
to both have fun of creating something from scratch and improve my coding skills.

Plans / specification
-----

Here are some thoughts about what I want my robot to be like:

Hardware:
  - middle-sized: big enought to carry RaspberryPi, camera and li-pol pack but not too big
                  as I want to use medium servos to make it walk
  - wireless: communication using wifi, powered by battery (li-pol) pack, no strings attached

Software:
  - Linux-powered
  - 2-modes: controlled and autonomous
  - using inverse kinematics for controlling legs
  - using camera for detecting obstacles
  - incorporate FPV and Xbox gamepad controll

Hardware
-----

I've designed robot's body using Google SketchUp - here is [project file](https://raw.githubusercontent.com/jp89/hexapod/master/hexapod_body.skp).
All peaces were cut from PCV foarm ("forex"). I printed sections of all body parts, marked them on forex sheet and cut with sharp knife. Unfortunatelly this material is bendy but I don't have access to 3D printer. Also plexiglass is very popular with hobbyst robot constructors but it's harder to process. Hexapod's legs are powered by medium size-class servos PowerHD HD-1160A. Servos are controlled by Raspberry Pi 2 and two Adafruit Servo Hats.

Here are some pictures of robot's body (not yet completed):
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160119_190657410.jpg" width = 500>
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160121_195926524.jpg" width = 500>
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160122_203152024.jpg" width = 500>
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160125_200029162.jpg" width = 500>
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160201_190640366.jpg" width = 500>
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160201_190747610.jpg" width = 500>
<img src="https://raw.githubusercontent.com/jp89/hexapod/master/pics/IMG_20160201_190845341.jpg" width = 500>


Software
-----
[TODO add high level design in UML]

Adafruit provides an example of servo controlling application written in Python.
I'd like my whole project to be written in C++, so I decided to port servo driver.
Adafruit.cpp and adafruit.hpp files provide API very similar to the one from Python examples.
If you would like to use this ported library all you need to do is include those files in
your project and read adafruit.hpp comments.
