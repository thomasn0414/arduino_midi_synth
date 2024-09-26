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
#include <LiquidCrystal.h>

// Initialize the LCD with the pin numbers for RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int trigPin = 9;
const int echoPin = 10;

long duration;
int distance;
int lastDistance = -1; // To store the last distance value
int smoothedDistance = 0; // Smoothed distance value
int note = 60; // Middle C (MIDI note number 60)
int previousNote = -1; // Keep track of the previous note played

// Variables for smoothing the distance readings
const int numReadings = 20; // Increase number of readings for more smoothing
int readings[numReadings]; // Array to store the distance readings
int readIndex = 0; // Current index in the readings array
int total = 0; // Running total of the readings

const int changeThreshold = 3; // Minimum change in distance to trigger note change

void setup() {
  // Set up LCD and print an initial message
  lcd.begin(16, 2);
  lcd.print("Distance: ");

  // Set up the sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize Serial for communication with Hairless MIDI
  Serial.begin(115200);

  // Initialize all readings to 0 (for smoothing)
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger the ultrasonic pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echoPin's pulse
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  distance = duration * 0.034 / 2;

  // Add the current reading to the total (for smoothing)
  total = total - readings[readIndex]; // Subtract the oldest reading
  readings[readIndex] = distance; // Add the new reading
  total = total + readings[readIndex]; // Add the new reading to the total
  readIndex = (readIndex + 1) % numReadings; // Move to the next position

  // Calculate the average distance
  smoothedDistance = total / numReadings;

  // Only update the LCD if the distance has changed significantly
  if (abs(smoothedDistance - lastDistance) >= changeThreshold) {
    lcd.setCursor(0, 1); // Move to the second row
    lcd.print("                "); // Clear previous value
    lcd.setCursor(0, 1); // Move cursor to the start of the second row
    lcd.print(smoothedDistance); // Print the smoothed distance value
    lcd.print(" cm");
    lastDistance = smoothedDistance; // Store the new distance
  }

  // Only update MIDI notes if the change is larger than the threshold
  if (abs(smoothedDistance - previousNote) >= changeThreshold) {
    if (smoothedDistance >= 5 && smoothedDistance <= 100) {
      // Map distance to a root note for the chord (e.g., C2 to C5)
      note = map(smoothedDistance, 5, 100, 48, 72); // Mapping distance to MIDI notes

      // Only send the chord if the root note is different from the previous note
      if (note != previousNote) {
        // Play the chord (root, major third, and perfect fifth)
        playChord(note);

        // Send MIDI note-off for the previous chord
        if (previousNote != -1) {
          stopChord(previousNote);
        }

        previousNote = note; // Update the previous note
      }
    } else {
      // If the distance is out of range, send Note Off for the last played chord
      if (previousNote != -1) {
        stopChord(previousNote);
        previousNote = -1;
      }
    }
  }

  delay(100); // Delay for smoother output
}

// Function to play a major chord
void playChord(int rootNote) {
  sendMIDI(0x90, rootNote, 127);     // Root note
  sendMIDI(0x90, rootNote + 4, 127); // Major third
  sendMIDI(0x90, rootNote + 7, 127); // Perfect fifth
}

// Function to stop a major chord
void stopChord(int rootNote) {
  sendMIDI(0x80, rootNote, 0);     // Root note
  sendMIDI(0x80, rootNote + 4, 0); // Major third
  sendMIDI(0x80, rootNote + 7, 0); // Perfect fifth
}

// Function to send MIDI messages over Serial
void sendMIDI(byte command, byte pitch, byte velocity) {
  Serial.write(command);
  Serial.write(pitch);
  Serial.write(velocity);

  // Used for Debugging
  Serial.print("MIDI Message: Command = ");
  Serial.print(command, HEX);
  Serial.print(", Pitch = ");
  Serial.print(pitch);
  Serial.print(", Velocity = ");
  Serial.println(velocity);
}

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
  - Check that the **MIDI input** is enabled in Reaper and the correct virtual instrument is added.


