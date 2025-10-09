import paho.mqtt.client as mqtt

broker = "d1f898165d50428fbfa74f86172d4e11.s1.eu.hivemq.cloud"
port = 8883
topic = "voice-command"

username = "mohamed"
password = "Mohamed56"


class MQTTPublisher:
    def __init__(self, broker, port, topic, username, password):
        self.broker = broker
        self.port = port
        self.topic = topic
        self.username = username
        self.password = password

        self.client = mqtt.Client()
        self.client.username_pw_set(username=username, password=password)
        self.client.tls_set()

    def connect(self):
        self.client.connect(self.broker, self.port, 60)
        self.client.loop_start()

    def publish(self, message, topic=None):
        topic = topic or self.topic
        result = self.client.publish(topic, message)
        status = result[0]
        if status == 0:
            print(f"📤 Sent `{message}` to topic `{topic}`")
        else:
            print(f"⚠️ Failed to send message to topic `{topic}`")

    def disconnect(self):
        self.client.loop_stop()
        self.client.disconnect()
        print("🔌 Disconnected from HiveMQ Cloud.")


publisher = MQTTPublisher(broker, port, topic, username, password)
publisher.connect()


def pub(text):
    publisher.publish(text)
