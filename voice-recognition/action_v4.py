from gtts import gTTS
import os

def say(text):
    tts = gTTS(text, lang='en')
    tts.save("test.mp3")
    os.system("mpg123 -q test.mp3")