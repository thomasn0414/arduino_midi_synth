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

  delay(100); 
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

  // For Debugging
  Serial.print("MIDI Message: Command = ");
  Serial.print(command, HEX);
  Serial.print(", Pitch = ");
  Serial.print(pitch);
  Serial.print(", Velocity = ");
  Serial.println(velocity);
}

