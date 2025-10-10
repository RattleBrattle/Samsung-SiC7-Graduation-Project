# telegram_voice_simple.py
import pvleopard
import paho.mqtt.client as mqtt
import json
import requests
import os
import time
import subprocess
import sys
import tempfile
from pydub import AudioSegment

# Configuration
ACCESS_KEY = "krw8de2MeRRLuCye9v84Jh4O5WKdVIdTdZztdRKR707Wn9QlZSehxw=="
TELEGRAM_BOT_TOKEN = "8053585829:AAFLIATZNePOuXPrZji86uLjfPLlvjry6SE"
TELEGRAM_CHAT_ID = "1143710208"

# MQTT Configuration
MQTT_SERVER = "mqtt.thingsboard.cloud"
MQTT_PORT = 1883
MQTT_TOKEN = "9ojgb2333x7gt5i9rgmo"

# Initialize speech-to-text
leopard = pvleopard.create(access_key=ACCESS_KEY)

class VoiceCommandProcessor:
    def __init__(self):
        self.mqtt_client = None
        self.setup_mqtt_simple()
        self.last_update_id = 0

    def setup_mqtt_simple(self):
        """Simple MQTT setup without callbacks to avoid loops"""
        try:
            print("🔗 Setting up MQTT connection...")
            self.mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)  # <-- FIXED HERE
            self.mqtt_client.username_pw_set(MQTT_TOKEN)
        
            # Optional: Set a client ID (not strictly needed, but helps with debugging)
            self.mqtt_client._client_id = f"voice_bot_{int(time.time())}"
        
            print(f"📡 Connecting to {MQTT_SERVER}:{MQTT_PORT} with token: {MQTT_TOKEN[:10]}...")
            self.mqtt_client.connect(MQTT_SERVER, MQTT_PORT, 60)
            self.mqtt_client.loop_start()
        
            time.sleep(2)
            print("✅ MQTT setup completed")
        
        except Exception as e:
            print(f"❌ MQTT setup failed: {e}")
            self.mqtt_client = None

    def send_mqtt_command(self, topic, data):
        """Send MQTT command with error handling"""
        if not self.mqtt_client:
            print("❌ MQTT client not available")
            return False
            
        try:
            result = self.mqtt_client.publish(topic, json.dumps(data))
            if result.rc == mqtt.MQTT_ERR_SUCCESS:
                print(f"📡 MQTT message sent to {topic}")
                return True
            else:
                print(f"❌ MQTT publish failed with code: {result.rc}")
                return False
        except Exception as e:
            print(f"❌ MQTT send error: {e}")
            return False

    def send_telegram_message(self, message):
        """Send message back to Telegram"""
        url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/sendMessage"
        data = {
            "chat_id": TELEGRAM_CHAT_ID,
            "text": message,
            "parse_mode": "Markdown"
        }
        try:
            response = requests.post(url, data=data, timeout=10)
            return response.status_code == 200
        except Exception as e:
            print(f"❌ Failed to send Telegram message: {e}")
            return False

    def send_device_command(self, device, state):
        """Send RPC command to control devices"""
        value = "ON" if state else "OFF"
        
        # Try different command formats
        commands_to_try = [
            # RPC format
            ("v1/devices/me/rpc/request/1", {
                "method": "setState",
                "params": {device: value}
            }),
            # Simple attribute format
            ("v1/devices/me/attributes", {device: value}),
            # Telemetry format  
            ("v1/devices/me/telemetry", {device: value}),
            # Direct RPC
            (f"v1/devices/me/rpc/request/{int(time.time())}", {
                "method": "set" + device.capitalize(),
                "params": value
            })
        ]
        
        success = False
        for topic, data in commands_to_try:
            if self.send_mqtt_command(topic, data):
                success = True
                print(f"✅ Command sent via {topic}: {device} = {value}")
        
        if success:
            return f"✅ {device.upper()} turned {value}"
        else:
            return f"❌ Failed to send command to {device}"

    def send_led_command(self, led_number, state):
        """Send command for specific LED"""
        room_names = {1: "Bedroom", 2: "Living Room", 3: "Kitchen"}
        room_name = room_names.get(led_number, f"Room {led_number}")
        value = "ON" if state else "OFF"
        device_name = f"led{led_number}"
        
        # Try different command formats
        commands_to_try = [
            # RPC format
            ("v1/devices/me/rpc/request/1", {
                "method": "setState",
                "params": {device_name: value}
            }),
            # Simple attribute format
            ("v1/devices/me/attributes", {device_name: value}),
            # Telemetry format  
            ("v1/devices/me/telemetry", {device_name: value}),
        ]
        
        success = False
        for topic, data in commands_to_try:
            if self.send_mqtt_command(topic, data):
                success = True
                print(f"✅ Command sent via {topic}: {room_name} Light = {value}")
        
        if success:
            return f"💡 {room_name} light turned {value}"
        else:
            return f"❌ Failed to control {room_name} light"

    def download_voice_message(self, file_id):
        """Download voice message from Telegram"""
        try:
            print("📥 Downloading voice message from Telegram...")
            
            # Get file path
            url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/getFile"
            response = requests.post(url, data={"file_id": file_id}, timeout=10)
            
            if response.status_code != 200:
                print("❌ Failed to get file path from Telegram")
                return None
                
            file_path = response.json()["result"]["file_path"]
            print(f"📁 File path: {file_path}")
            
            # Download file
            download_url = f"https://api.telegram.org/file/bot{TELEGRAM_BOT_TOKEN}/{file_path}"
            response = requests.get(download_url, timeout=30)
            
            # Save as OGG file
            ogg_file = f"voice_{int(time.time())}.ogg"
            with open(ogg_file, "wb") as f:
                f.write(response.content)
            
            print(f"✅ Voice message saved: {ogg_file} ({os.path.getsize(ogg_file)} bytes)")
            return ogg_file
            
        except Exception as e:
            print(f"❌ Error downloading voice message: {e}")
            return None

    def convert_ogg_to_wav_simple(self, ogg_file_path):
        """Simple and reliable OGG to WAV conversion using pydub"""
        try:
            print(f"🔄 Converting {ogg_file_path} to WAV...")
            
            # Check if input file exists
            if not os.path.exists(ogg_file_path):
                print(f"❌ File not found: {ogg_file_path}")
                return None
                
            # Generate output filename
            wav_file_path = ogg_file_path.replace('.ogg', '.wav')
            
            # Convert OGG to WAV using the same method that worked in our test
            print("🎵 Loading OGG file with pydub...")
            audio = AudioSegment.from_ogg(ogg_file_path)
            
            print(f"📊 Audio info: {len(audio)}ms, {audio.frame_rate}Hz, {audio.channels} channels")
            
            print("💾 Exporting as WAV...")
            audio.export(wav_file_path, format="wav")
            
            # Verify output
            if os.path.exists(wav_file_path) and os.path.getsize(wav_file_path) > 0:
                print(f"✅ Conversion successful: {wav_file_path} ({os.path.getsize(wav_file_path)} bytes)")
                return wav_file_path
            else:
                print("❌ Conversion failed - output file is empty or missing")
                return None
                
        except Exception as e:
            print(f"❌ Conversion error: {e}")
            return None

    def process_voice_command(self, file_id):
        """Process voice message with reliable pydub conversion"""
        ogg_file = None
        wav_file = None
        
        try:
            # Download voice message
            ogg_file = self.download_voice_message(file_id)
            if not ogg_file:
                return "❌ Failed to download voice message"
            
            # Convert OGG to WAV using our proven method
            wav_file = self.convert_ogg_to_wav_simple(ogg_file)
            
            if not wav_file:
                return "❌ Failed to convert voice message to WAV format"
            
            # Process the WAV file with speech recognition
            print("🎤 Attempting speech recognition on WAV file...")
            try:
                transcript, words = leopard.process_file(wav_file)
                transcript = transcript.lower().strip()
                print(f"📝 Recognized speech: '{transcript}'")
                
                if not transcript:
                    return "❌ No speech detected in the voice message"
                
                # Process command
                response = self.execute_command(transcript)
                return response
                
            except Exception as e:
                print(f"❌ Speech recognition failed: {e}")
                return "❌ Could not process voice message. Please try again."
            
        except Exception as e:
            print(f"❌ Error in process_voice_command: {e}")
            return f"❌ Error: {str(e)}"
        
        finally:
            # Clean up temporary files
            for file_path in [ogg_file, wav_file]:
                if file_path and os.path.exists(file_path):
                    try:
                        os.remove(file_path)
                        print(f"🧹 Cleaned up: {file_path}")
                    except Exception as e:
                        print(f"⚠️ Could not remove {file_path}: {e}")

    def execute_command(self, transcript):
        """Execute command based on voice transcript"""
        transcript = transcript.lower().strip()
        print(f"🔍 Analyzing command: '{transcript}'")
        
        # Command matching
        commands = {
            # Light commands
            "light on": lambda: self.send_led_command(1, True),
            "turn on light": lambda: self.send_led_command(1, True),
            "lights on": lambda: self.send_led_command(1, True),
            "light off": lambda: self.send_led_command(1, False),
            "turn off light": lambda: self.send_led_command(1, False),
            "lights off": lambda: self.send_led_command(1, False),
            
            # Fan commands
            "fan on": lambda: self.send_device_command("fan", True),
            "turn on fan": lambda: self.send_device_command("fan", True),
            "start fan": lambda: self.send_device_command("fan", True),
            "fan off": lambda: self.send_device_command("fan", False),
            "turn off fan": lambda: self.send_device_command("fan", False),
            "stop fan": lambda: self.send_device_command("fan", False),
            
            # AC commands
            "ac on": lambda: self.send_device_command("ac", True),
            "turn on ac": lambda: self.send_device_command("ac", True),
            "air conditioner on": lambda: self.send_device_command("ac", True),
            "ac off": lambda: self.send_device_command("ac", False),
            "turn off ac": lambda: self.send_device_command("ac", False),
            "air conditioner off": lambda: self.send_device_command("ac", False),
        }
        
        # Exact match
        if transcript in commands:
            return commands[transcript]()
        
        # Partial match
        for cmd, action in commands.items():
            if cmd in transcript:
                return action()
        
        return f"❌ Command not recognized: '{transcript}'\n\n💡 Try these commands:\n• 'light on/off'\n• 'fan on/off' \n• 'ac on/off'"

    def get_telegram_updates(self):
        """Get new messages from Telegram"""
        url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/getUpdates"
        params = {"offset": self.last_update_id + 1, "timeout": 10}
        
        try:
            response = requests.get(url, params=params, timeout=15)
            if response.status_code == 200:
                return response.json()
        except Exception as e:
            print(f"❌ Error getting updates: {e}")
        return None

    def run(self):
        """Main bot loop"""
        print("🎤 TELEGRAM VOICE PROCESSOR - SIMPLE MQTT VERSION")
        print("=" * 50)
        print("📡 Listening for voice commands...")
        print("💡 Supported commands:")
        print("   • 'light on' / 'light off'")
        print("   • 'fan on' / 'fan off'") 
        print("   • 'ac on' / 'ac off'")
        print("=" * 50)
        print(f"🔗 MQTT Status: {'✅ AVAILABLE' if self.mqtt_client else '❌ UNAVAILABLE'}")
        print("✅ Voice processing is working!")
        print("✅ Ready to process voice messages...")
        
        while True:
            updates = self.get_telegram_updates()
            
            if updates and "result" in updates:
                for update in updates["result"]:
                    self.last_update_id = update["update_id"]
                    
                    if "message" in update:
                        message = update["message"]
                        chat_id = str(message["chat"]["id"])
                        
                        if chat_id == TELEGRAM_CHAT_ID:
                            if "voice" in message:
                                print("\n" + "🎤" * 20)
                                print("🎤 NEW VOICE MESSAGE DETECTED!")
                                print("🎤" * 20)
                                
                                file_id = message["voice"]["file_id"]
                                response = self.process_voice_command(file_id)
                                
                                # Send result back to Telegram
                                self.send_telegram_message(response)
                                print(f"✅ FINAL RESULT: {response}")
                                print("🎤" * 20)
            
            time.sleep(3)

if __name__ == "__main__":
    processor = VoiceCommandProcessor()
    processor.run()