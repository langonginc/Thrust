# Horizontal Thrust Measuring Stage

A stage to measure horizontral thrust generated by rocket engine.

This is a project created by students in The Aviation Club of the [Second High School Attached to Beijing Normal University](https://www.shsbnu.net/index.html).

[推力台说明手册(中文版) | Chinese version](https://github.com/langonginc/Thrust/blob/main/README.pdf)

![SolidWorks 3D Model](https://github.com/langonginc/Thrust/assets/59787082/9b430c01-5098-4a07-90ee-a4aa83d04bf6)

[Video from bilibili](https://www.bilibili.com/video/BV1fC411E7s8/?share_source=copy_web&vd_source=09f47ca71ce9a13b053114acdf5cffeb) 

# Highlights

We use Arduino Mega 2560 and HX711 to record and store the magnitude of the thrust into the program, and graph the associated function (force @ time).

- **0-20kg** measuring range _(maximum value not to exceed 30kg)_
- 30-50mm engine diameter fit
- 10Hz sample rate ([#1](https://github.com/langonginc/ForceTestStand/issues/1) We really need your help to increase it)
- Real-time display
- Serial port writing
- ~~SD card data writing~~

# Theory

Presure -> Sensors -> HX711 -> Arduino.

Pressure sensors generate a voltage by sensing pressure. Sensitive voltage value is 1mv, which means if I apply a force of `1kg`, it produces a voltage of `1mV` (for 0-10kg sensor).

HX711 will firstly amplify the voltage generated by the pressure sensor, since the voltage is too small. Then it will convert the analog signals (amplified voltage values) to digital signals (ad values).

After that, Arduino will get the digital signals (ad values). We then scale the digital signal to fit the pressure.

## Why We Use HX711

Based on Avia Semiconductor’s patented technology, HX711 is a precision 24-bit analog-to-digital converter (ADC) designed for weigh scales and industrial control applications to interface directly with a bridge sensor.

The input multiplexer selects either Channel A or B differential input to the low-noise programmable gain amplifier (PGA). Channel A can be programmed with a gain of 128 or 64, corresponding to a full-scale differential input voltage of ±20mV or ±40mV respectively, when a 5V supply is connected to AVDD analog power supply pin. Channel B has a fixed gain of 32. On-chip power supply regulator eliminates the need for an external supply regulator to provide analog power for the ADC and the sensor. Clock input is flexible. It can be from an external clock source, a crystal, or the on-chip oscillator that does not require any external component. On-chip power-on-reset circuitry simplifies digital interface initialization.

There is no programming needed for the internal registers. All controls to the HX711 are through the pins.

> We are using the channel A (input voltage of ±20mV).

## How HX711 works

The HX711 communicates with the microcontroller in serial communication mode consisting of pins SCK and DOUT. When DOUT goes from high to low, SCK inputs 25 pulses at a time to read the 24-bit A/D conversion data into the microcontroller and selects the input channel and gain for the next conversion on the 25th clock pulse.

# Getting Started

## Building Framework

Based on the Solidworks 3D model, the framework can be built with the following items.

| Item | Details | Quantity | Image |
| :----- | :----- | :----- | :----: |
| European Standard Aluminum Profiles 3030 | Length 400mm | 2 | <img width="450" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/51ba9b28-3dab-4097-b537-fb6b0269112a"> |
| European Standard Aluminum Profiles 3030 | Length 360mm | 1 | Same as above |
| European Standard Aluminum Profiles 3030 | Length 300mm | 5 | Same as above |
| European Standard Aluminum Profiles 3030 | Length 200mm | 1 | Same as above |
| European Standard Slider Nuts 3030 | M4 | About 20 | <img width="437" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/2dc183d2-ea36-473e-b237-4c39f9f1a9ae"> |
| European Standard Angle Fittings 3030 | | 16 | <img width="445" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/754c2e6c-5634-4a28-841b-e68dff2a29d0"> |
| Steel shaft ø8 | 300mm | 2 | <img width="333" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/930f8e58-5a72-4f09-b700-16e0b5d5788d"> |
| Shaft fixing bracket ø8 | | 4 | <img width="325" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/08051fd7-fbbe-48eb-8530-7e6e2b6a1ab8"> |
| Carriage slider ø8 | | 4 | <img width="321" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/1502ca1a-5872-4ee1-b9d6-d798a93ddcc2"> |
| Tray base plate _(Laser cut by myself)_ | From `plate.dwg` | 1 | <img width="656" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/01aaf1f0-54f0-4139-a129-9fc0d24b6aae"> |

> SolidWorks 2022, AutoCAD 2024 (DWG 2018)

## Wiring the Arduino Board

Wire the Arduino board and components using DuPont wires according to the following table.

| Item | Image | Reference link |
| :----- | :----- | :----- |
| HX711 chip + Pressure sensor | <img width="423" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/d9462333-b126-4f56-bf2f-f4b3c8cb072a"> | [Link](https://detail.tmall.com/item.htm?id=570644601916) |
| TM1650 4-digit digital tube display module | <img width="421" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/7d78ea1e-d939-419a-a8d0-1551830b52e4"> | |

**Arduino - HX711**

| Arduino Pin | HX711 Pin |
| :----: | :----: |
| 5V | VCC |
| GND | GND |
| 2 | SCK |
| 3 | DOUT |

**HX711 - Pressure sensors**

| HX711 Pin | Sensors Wire |
| :----: | :----: |
| E+ | Red |
| E- | Black |
| A- | White |
| A+ | Green |

**Arduino - Micro SD Card Adapter**

_We just removed this component as its low writing speed._

| Arduino Pin | HX711 Pin |
| :----: | :----: |
| 5V | VCC |
| GND | GND |
| 50 | MISO |
| 51 | MOSI |
| 52 | SCK |
| 53 | CS |

> The communication between the microcontroller and the SD card uses SPI, which takes place on digital pins 11, 12, and 13 (on most Arduino boards) or 50, 51, and 52 (Arduino Mega). Additionally, another pin must be used to select the SD card. This can be the hardware SS pin - pin 10 (on most Arduino boards) or pin 53 (on the Mega) - or another pin specified in the call to SD.begin(). Note that even if you don't use the hardware SS pin, it must be left as an output or the SD library won't work.
>
> --Arduino SD libarary

## Uploading Code to Arduino Board

Download `main.ino` and open it in Arduino IDE ([Link](https://www.arduino.cc/en/software)).

Plug in your Arduino. Then, click the "upload" button.

## Getting Data

Click `Tools - Serial Monitor` to check the results.

<img width="336" alt="image" src="https://github.com/langonginc/Thrust/assets/59787082/83e0c7d0-eb4c-4a05-b3ef-3f64745ccfef">

## Analysing Data

Copy the results getting from the serial monitor, which are array of integers (e.g. `101, 123.4, 111, 114.5, 121, 514.5, ...`) to the third line of `graph.py`.

```python
originalData = [ Paste Data To Here ]     # Notice: delete the last comma ',' !!!
```

Run the code, you will get a graph that is the associated function (force @ time).
