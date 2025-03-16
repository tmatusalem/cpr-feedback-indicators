# Arduino-Based CPR Feedback System
An Arduino-based system designed to provide real-time monitoring of CPR performance through visual and audio indicators.

**Purpose**

Sudden cardiac arrest requires immediate intervention, with high-quality CPR being a key determinant of survival. However, both novice and experienced responders struggle to maintain proper compression depth and rate [1]. Research has shown that CPR feedback devices significantly improve performance by providing real-time visual and auditory cues [2].

This project implements an accelerometer-based CPR feedback system using an Arduino setup. The system calculates CPR rate in real time and displays this information on an OLED screen, assisting users in adhering to AHA-recommended guidelines (100–120 compressions per minute at 5–6 cm depth) [3].

**Hardware**

<ins>Components</ins>

* **Arduino Uno** - Microcontroller to receive and process sensor data
* **ADXL355 Accelerometer** - Measures acceleration of compression movements
* **Adafruit SSD1306 OLED Display** - Displays CPR rate
* **Buzzer 5V - Breadboard Friendly** - Provides audio indicator based on CPR performance
* **Red and Green LEDs** - Provide visual indicators based on CPR performance (red indicates too slow, green indicates too fast)

**Working Principle**

The CPR feedback system operates by detecting chest compression movements using an accelerometer. The Arduino processes this acceleration data and applies peak detection to identify compression cycles. 

Based on the detected peaks:
* CPR rate is determined by measuring the time between successive compression peaks.
* A slide switch allows the user to switch between receiving auditory feedback indication or visual feedback indication.
* LEDs and a Buzzer provide real-time feedback, alerting users when compressions are too slow, too fast, or within the optimal range (none light up or no audio indicator).

The program continuously reads analog input from the accelerometer and tracks the moving maximum (A<sub>max</sub>) and minimum (A<sub>min</sub>) values to determine CPR rate. A decay factor is applied to the maximum value to create an adaptive envelope which helps establish dynamic thresholds to detect peaks. Applying hysteresis, the program identifies when the signal chooses an upper threshold to mark compression cycles and calculate CPR rate based on the time between each successive peak.

$$Upper Threshold = 0.7A<sub>max</sub> + 0.3A<sub>min</sub>$$
$$Lower Threshold = 0.3A<sub>max</sub> + 0.7A<sub>min</sub>$$

When the signal exceeds the upper threshold, the time between successive peaks (Δt in seconds) is used to compute the CPR rate (f<sub>CPR</sub>) in Hz:

$$f<sub>CPR</sub> = 1/Δt$$

**Rationale**

The system is designed to consider:

* Proper rate: studies show that visual feedback improves CPR performance. An OLED display provides real-time monitoring of the numerical CPR rate while LEDs and a buzzer will help users know to adjust their compressions immediately.
* Ease of use: The intuitive slide switch system allows users to easily choose if they'd like visual or auditory indicators to evaluate their performance.
* Accuracy and calibration: The peak detection method dynamically adjusts detection thresholds. A future revision could incorporate calibration to improve rate measurements.

**Feedback Mechanism**

* OLED Display: Displays CPR rate
* LED and Buzzer Alerts:
  * Below 100 BPM: Red LED on or buzzer on
  * Above 140 BPM: Green LED on or buzzer on
  * 100-140 BPM: LEDs off, buzzer off

**Examples of Input-Output Scenarios**

<ins>Case 1: Correct CPR rate (110 BPM)</ins>

* INput: Acceleration signal corresponding to 110 compressions per minute.
* OUTput: Display shows "CPR Rate: 110". No buzzer or warning LEDs activate.

<ins>Case 2: Incorrect CPR rate (80 BPM)</ins>

* INput: Slower acceleration signal corresponding to 80 compressions per minute.
* OUTput: Display shows "CPR Rate: 80". Slow LED on or buzzer activates.

<ins>Case 3: Incorrect CPR rate (160 BPM)</ins>

* INput: Faster acceleration signal corresponding to 160 compressions per minute.
* OUTput: Display shows "CPR Rate: 160". Fast LED on or buzzer activates.

**Areas for Improvement**

* CPR Depth Implementation:
  * Due to time constraints, I was unable to additionally implement the logic of calculating the depth of the accelerometer's movements. Adding this functionality along with separate indicators for suboptimal compression depth performance would be significantly beneficial.
* Larger OLED Display:
  * Adding a larger display to read the CPR rate would enhance the interface for users.
* Piezo Buzzers:
  * Having functional buzzers which change tone depending on the incorrect CPR rate (fast or slow) would allow users using the audio function to know if they are moving too fast or too slow. A higher pitch could indicate moving too fast and a lower pitch could indicate moving too slow.
* Spoken Instructions:
  * In tandem with piezo buzzers, adding speakers which play audio files saying "Slow Down" or "Speed Up" could also help improve user experience.

# Acknowledgements
This project was developed as part of the BIOEN 568 course at the University of Washington. The course provided the hardware components, project logic, and key programming concepts including the application of hysteresis and threshold-based detection for CPR feedback. Special thanks to Dr. Christopher Neils and the teaching staff for their guidance and support.

**References**
1. Wik, Lars et al. “Quality of cardiopulmonary resuscitation during out-of-hospital cardiac arrest.” JAMA vol. 293,3 (2005): 299-304. doi:10.1001/jama.293.3.299
2. González-Otero, Digna M et al. “A new method for feedback on the quality of chest compressions during cardiopulmonary resuscitation.” BioMed research international vol. 2014 (2014): 865967. doi:10.1155/2014/865967
3. Merchant, Raina M et al. “Part 1: Executive Summary: 2020 American Heart Association Guidelines for Cardiopulmonary Resuscitation and Emergency Cardiovascular Care.” Circulation vol. 142 (2020): S337-S357. doi:10.1161/CIR.0000000000000918
