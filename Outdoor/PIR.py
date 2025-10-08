import RPi.GPIO as GPIO
import time

# Set up GPIO mode
GPIO.setmode(GPIO.BCM)

# Define pins
PIR_PIN = 17
LED_PIN = 27

# Set up GPIO pins
GPIO.setup(PIR_PIN, GPIO.IN)
GPIO.setup(LED_PIN, GPIO.OUT)

print("PIR Sensor Test (Press Ctrl+C to exit)")
time.sleep(2)
print("Ready...")

try:
    while True:
        if GPIO.input(PIR_PIN):
            print("Motion Detected!")
            GPIO.output(LED_PIN, True)
            time.sleep(1)
        else:
            GPIO.output(LED_PIN, False)
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Exiting...")
    GPIO.cleanup()
