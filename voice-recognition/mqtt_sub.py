import paho.mqtt.client as mqtt

broker = "d1f898165d50428fbfa74f86172d4e11.s1.eu.hivemq.cloud"
port = 8883
topic = "voice-command"

username = "mohamed"
password = "Mohamed56"


class MQTTSubscriber:
    def __init__(self, broker, port, topic, username, password, message_handler=None):
        self.broker = broker
        self.port = port
        self.topic = topic
        self.username = username
        self.password = password
        self.message_handler = message_handler

        self.client = mqtt.Client()
        self.client.username_pw_set(self.username, self.password)
        self.client.tls_set()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def connect(self):
        print(f"🔗 Connecting to HiveMQ Cloud broker {self.broker}:{self.port}...")
        self.client.connect(self.broker, self.port, 60)
        self.client.loop_forever()

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("✅ Connected to HiveMQ Cloud!")
            self.client.subscribe(self.topic)
            print(f"📡 Subscribed to topic `{self.topic}`")
        else:
            print(f"❌ Failed to connect. Return code {rc}")

    def on_message(self, client, userdata, msg):
        payload = msg.payload.decode().lower().strip()
        print(f"📥 Received message: `{payload}` on topic `{msg.topic}`")

        if self.message_handler:
            self.message_handler(payload)


# 🧠 Command handler
def message_handle(payload):
    # Lights
    if "turning on room one light" in payload:
        print("💡 Action: Room 1 Light ON")
        # TODO: Add GPIO or ThingsBoard command here

    elif "turning off room one light" in payload:
        print("💡 Action: Room 1 Light OFF")

    elif "turning on room two light" in payload:
        print("💡 Action: Room 2 Light ON")

    elif "turning off room two light" in payload:
        print("💡 Action: Room 2 Light OFF")

    # Fans
    elif "turning on room one fan" in payload:
        print("🌀 Action: Room 1 Fan ON")

    elif "turning off room one fan" in payload:
        print("🌀 Action: Room 1 Fan OFF")

    elif "turning on room two fan" in payload:
        print("🌀 Action: Room 2 Fan ON")

    elif "turning off room two fan" in payload:
        print("🌀 Action: Room 2 Fan OFF")

    # Time
    elif "the time is" in payload:
        print(f"🕒 Voice assistant said: {payload}")

    else:
        print("❓ Unknown command received.")


mqtt_sub = MQTTSubscriber(broker, port, topic, username, password, message_handle)
mqtt_sub.connect()
