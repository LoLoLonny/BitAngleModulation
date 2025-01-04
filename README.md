# BitAngleModulation
A Arduino hobby project that implements bit angle modulation for a multiplex LED matrix.

This sketch implements binary code modulation (also called bit angle modulation) for a custom soldered RGB multiplex matrix.
Multiplexing is a method of signal transmission used to combine several pieces of data. In the case of an LED matrix, the anodes and cathodes of the LEDs are soldered to form the rows and columns of a matrix. This reduces the number of control pins required.
This project uses “common anode” RGB LEDs. These each have an anode for the common power supply and three cathodes for the individual colors.

<img src="https://github.com/user-attachments/assets/43a1a686-dfb6-40ef-9111-932a34af7d74" alt="drawing" width="400"/>

The multiplex matrix is controlled by bit shift registers. A shift register receives data serially and forwards it in parallel.A shift register can receive and forward one byte.This means that eight rows of LEDs can be controlled with one pin. Four shift registers are used in this project. Three for the rows (8xR,8xG,8xB) and one to control the six columns of the matrix.

The Sketch starts with a short demo that prints out words and then reads a raw audio signal from the ADCH and interprets it with a FFT implementation. The implementation of the FFT is not mine. I found it online [](http://www.instructables.com/id/Arduino-Frequency-Detection/).
