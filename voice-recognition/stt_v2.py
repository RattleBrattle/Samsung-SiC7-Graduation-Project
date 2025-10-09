import pvleopard
import action_v4
import mqtt_pub
from datetime import datetime



ACCESS_KEY = "krw8de2MeRRLuCye9v84Jh4O5WKdVIdTdZztdRKR707Wn9QlZSehxw=="

leopard = pvleopard.create(access_key=ACCESS_KEY)

def stt_function(file):
    transcript, words = leopard.process_file(file)
    print(transcript)
    # Light Control 
    if "light" in transcript or "led" in transcript:
        if "room one" in transcript or "room 1" in transcript:
            if "on " in transcript:
                mqtt_pub.pub("Turning on room one light")
                action_v4.say("Turning on room one light") 
            elif "off" in transcript or "of" in transcript:
                mqtt_pub.pub("Turning off room one light")
                action_v4.say("Turning off room one light") 
            else:
                mqtt_pub.pub("Command not recognized")
                action_v4.say("Command not recognized")
        elif "room two" in transcript or "room to" in transcript or "room 2" in transcript:
            if "on " in transcript:
                mqtt_pub.pub("Turning on room two light")
                action_v4.say("Turning on room two light") 
            elif "off" in transcript or "of" in transcript:
                mqtt_pub.pub("Turning off room two light")
                action_v4.say("Turning off room two light") 
            else:
                mqtt_pub.pub("Command not recognized")
                action_v4.say("Command not recognized")
        else:
            mqtt_pub.pub("Command not recognized")
            action_v4.say("Command not recognized")
    elif "fan" in transcript:
        if "room one" in transcript or "room 1" in transcript:
            if "on " in transcript:
                mqtt_pub.pub("Turning on room one fan")
                action_v4.say("Turning on room one fan") 
            elif "off" in transcript or "of" in transcript:
                mqtt_pub.pub("Turning off room one fan")
                action_v4.say("Turning off room one fan") 
            else:
                mqtt_pub.pub("Command not recognized")
                action_v4.say("Command not recognized")
        elif "room two" in transcript or "room to" in transcript or "room 2" in transcript:
            if "on " in transcript:
                mqtt_pub.pub("Turning on room two fan")
                action_v4.say("Turning on room two fan") 
            elif "off" in transcript or "of" in transcript:
                mqtt_pub.pub("Turning off room two fan")
                action_v4.say("Turning off room two fan") 
            else:
                mqtt_pub.pub("Command not recognized")
                action_v4.say("Command not recognized")
        else:
            mqtt_pub.pub("Command not recognized")
            action_v4.say("Command not recognized")
    elif "time" in transcript:
        now = datetime.now()
        hour = now.strftime("%I")  # 12-hour format
        minute = now.strftime("%M")
        am_pm = now.strftime("%p").lower()

        # Convert to text
        time_text = f"The time is {hour}:{minute} {am_pm}."
        mqtt_pub.pub(time_text)
        action_v4.say(time_text)
    else:
        mqtt_pub.pub("Command not recognized")
        action_v4.say("Command not recognized")

    



