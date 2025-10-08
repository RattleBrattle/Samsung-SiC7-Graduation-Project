import RPi.GPIO as GPIO
from time import sleep
import requests
from datetime import datetime

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# ========== Servo setup ==========
SERVO_PIN = 18
GPIO.setup(SERVO_PIN, GPIO.OUT)
servo = GPIO.PWM(SERVO_PIN, 50)  # 50Hz
servo.start(0)

# ========== Buzzer setup ==========
BUZZER_PIN = 26
GPIO.setup(BUZZER_PIN, GPIO.OUT)

# ========== Keypad setup ==========
ROWS = [5, 6, 13, 19]
COLS = [12, 16, 20, 21]

KEYPAD = [
    ["1", "2", "3", "A"],
    ["4", "5", "6", "B"],
    ["7", "8", "9", "C"],
    ["*", "0", "#", "D"]
]

for r in ROWS:
    GPIO.setup(r, GPIO.OUT)
    GPIO.output(r, GPIO.LOW)

for c in COLS:
    GPIO.setup(c, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

# ========== Constants ==========
PASSWORD = "452002"
entered = ""
wrong_attempts = 0

# ========== Telegram Bot Info ==========
BOT_TOKEN = "8053585829:AAFLIATZNePOuXPrZji86uLjfPLlvjry6SE"
CHAT_ID = "1143710208"

def send_telegram_message(message):
    """Send a message to your Telegram bot."""
    url = f"https://api.telegram.org/bot{BOT_TOKEN}/sendMessage"
    data = {"chat_id": CHAT_ID, "text": message}
    try:
        requests.post(url, data=data)
        print("Telegram message sent.")
    except Exception as e:
        print("Error sending Telegram message:", e)

# ========== Functions ==========
def open_door():
    print("Correct password - Opening door...")
    servo.ChangeDutyCycle(7.5)
    sleep(5)
    print("Closing door...")
    servo.ChangeDutyCycle(2.5)
    sleep(1)
    servo.ChangeDutyCycle(0)

def buzzer_alarm(times=5, delay=0.2):
    """Activate buzzer alarm."""
    for _ in range(times):
        GPIO.output(BUZZER_PIN, True)
        sleep(delay)
        GPIO.output(BUZZER_PIN, False)
        sleep(delay)

def read_keypad():
    for r in range(4):
        GPIO.output(ROWS[r], GPIO.HIGH)
        for c in range(4):
            if GPIO.input(COLS[c]) == GPIO.HIGH:
                GPIO.output(ROWS[r], GPIO.LOW)
                return KEYPAD[r][c]
        GPIO.output(ROWS[r], GPIO.LOW)
    return None

# ========== Main Program ==========
print("Enter password then press# ")

try:
    while True:
        key = read_keypad()
        if key:
            print(key)
            if key == "*":
                entered = ""
                print("Cleared")
            elif key == "#":
                print(f"Entered: {entered}")
                if entered == PASSWORD:
                    open_door()
                    wrong_attempts = 0
                else:
                    wrong_attempts += 1
                    print(f"Wrong password! Attempt {wrong_attempts}")

                    if wrong_attempts == 3:
                        print("3 wrong attempts! Triggering buzzer...")
                        buzzer_alarm()

                    elif wrong_attempts >= 5:
                        print("5 wrong attempts! Triggering alarm + sending Telegram alert...")
                        buzzer_alarm(times=10, delay=0.1)

                        # Get current date and time
                        now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                        # Send alert with timestamp
                        message = f"ALERT: 5 failed password attempts detected!\nTime: {now}"
                        send_telegram_message(message)

                        wrong_attempts = 0  # reset counter after alert

                entered = ""
            else:
                entered += key
                print("*", end="", flush=True)
            sleep(0.3)
        sleep(0.1)

except KeyboardInterrupt:
    print("\nExiting...")
    servo.stop()
    GPIO.cleanup()

