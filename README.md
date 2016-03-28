- [Intro](#intro)
- [Plans / specification]($specification)
- [Hardware](#hardware)

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

I've designed robot's body using Google SketchUp.
All peaces were cut from PCV foarm ("forex").
Unfortunatelly it's bendy but I don't have access to 3D printer.
Also plexiglass is very popular with hobbyst robot constructors but it's harder to process.
