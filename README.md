# Arduino MIDI Synth with LCD Display

This project allows you to create a MIDI-controlled synthesizer using an Arduino, an ultrasonic sensor, and an LCD display. The sensor detects distance, and based on that, MIDI notes are generated and sent to a DAW (like Reaper) via Hairless MIDI and loopMIDI. You can recreate this setup using the following guide, along with the Tinkercad model and Arduino code.

The system sends MIDI chords based on the distance detected by the sensor and displays the distance on an LCD screen. It's perfect for creating ambient or meditation-style music.

## Features
- Ultrasonic sensor controlling MIDI notes.
- LCD display showing the distance in real-time.
- Smooth MIDI note transitions.
- Use with Reaper, Hairless MIDI, and loopMIDI.
## Materials Needed

### Hardware:
- Arduino Uno (or compatible board)
- Ultrasonic sensor (HC-SR04)
- LCD Display (16x2) with potentiometer for contrast control
- Breadboard and jumper wires
- 10kΩ potentiometer (for LCD contrast)
- USB cable (for Arduino)

### Software:
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Hairless MIDI](http://projectgus.github.io/hairless-midiserial/)
- [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html)
- [Reaper DAW](https://www.reaper.fm/)
## Hardware Setup

You can view the Tinkercad model for the wiring setup below
![Arduino Setup](https://github.com/user-attachments/assets/e3406aed-52cb-44b2-b432-21ccadbea267)

)

### Wiring Diagram
- **Ultrasonic Sensor (HC-SR04):**
  - VCC → 5V
  - GND → GND
  - Trig → Pin 9
  - Echo → Pin 10
- **LCD (16x2):**
  - VSS → GND
  - VDD → 5V
  - V0 → Potentiometer (for contrast control)
  - RS → Pin 12
  - RW → GND
  - E → Pin 11
  - D4 → Pin 5
  - D5 → Pin 4
  - D6 → Pin 3
  - D7 → Pin 2
  - A (Backlight +) → 5V
  - K (Backlight -) → GND

### Example Image of Setup:
![Setup image](https://github.com/user-attachments/assets/250ddc1c-b7fe-4f9a-b3db-02fb7cd63029)

## Arduino Code

The following code handles the ultrasonic sensor, MIDI output, and LCD display:
You can find the full Arduino code in the `arduino_midi_synth.ino` file [here](arduino_midi_synth.ino).
## loopMIDI Setup

1. Download and install [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html).
2. Open **loopMIDI** and create a new virtual MIDI port by clicking the **+** button. Name the port something like "Arduino MIDI".
3. Leave loopMIDI running while you use Hairless MIDI and Reaper.

## Hairless MIDI Setup

1. Download and install [Hairless MIDI](http://projectgus.github.io/hairless-midiserial/).
2. Open Hairless MIDI and configure it as follows:
   - **Serial port**: Select the COM port for your Arduino (e.g., COM3).
   - **MIDI Out**: Select the virtual MIDI port created in loopMIDI (e.g., "Arduino MIDI").
   - **Baud rate**: Set to **115200** (same as in the Arduino code).
3. Start the **Serial→MIDI Bridge** to begin sending MIDI data from the Arduino to Reaper.

## Reaper Setup

1. Open Reaper and go to **Options > Preferences > MIDI Devices**.
2. Under **MIDI Inputs**, enable the **loopMIDI** port created earlier (e.g., "Arduino MIDI").
3. Insert a new track in Reaper (**Ctrl+T**), and click the **FX** button to add a virtual instrument.
   - You can use **ReaSynth** or any other VST instrument.
4. Arm the track for recording and enable input monitoring by clicking the **Record Arm** button and the **Monitor** button.
5. Set the **Input** of the track to **MIDI > loopMIDI > All Channels**.
6. You should now hear MIDI notes generated from your Arduino as you move your hand in front of the sensor.

## Testing and Troubleshooting

### Testing
1. Once everything is connected, move your hand in front of the ultrasonic sensor to trigger MIDI notes.
2. The LCD should display the distance, and you should hear MIDI notes in Reaper.
3. Adjust the sensor's distance and note range as needed.

### Troubleshooting
- If you're not hearing sound in Reaper:
  - Ensure **loopMIDI** and **Hairless MIDI** are running and configured correctly.
  - Make sure the correct **COM port** is selected in Hairless MIDI.
  - Ensure that in the Arduino IDE, the Serial Monitor is off as Hairless MIDI requires it to be disabled for it to work properly. 
  - Check that the **MIDI input** is enabled in Reaper and the correct virtual instrument is added.


