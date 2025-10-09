import pvporcupine
import pyaudio
import struct
import wave 
import stt_v2



ACCESS_KEY = "krw8de2MeRRLuCye9v84Jh4O5WKdVIdTdZztdRKR707Wn9QlZSehxw=="
KEYWORD_PATH = "./wake-word/hi-elva_en_linux_v3_0_0.ppn"
RECORD_SECONDS = 5



porcupine = pvporcupine.create(
    access_key=ACCESS_KEY,
    keyword_paths=[KEYWORD_PATH]
)

audio = pyaudio.PyAudio()
stream = audio.open(
    rate=porcupine.sample_rate,
    channels=1,
    format=pyaudio.paInt16,
    input=True,
    frames_per_buffer= porcupine.frame_length
)

def record_command(filename,stream,sample_rate,frame_length):
    frames = []
    for _ in range (0,int(sample_rate/porcupine.frame_length * RECORD_SECONDS)):
        data = stream.read(porcupine.frame_length,exception_on_overflow=False)
        frames.append(data)
    
    wf = wave.open(filename,"wb")
    wf.setnchannels(1)
    wf.setsampwidth(audio.get_sample_size(pyaudio.paInt16))
    wf.setframerate(sample_rate)
    wf.writeframes(b"".join(frames))
    wf.close()


print("Listening for wake words..... (Ctrl+C to stop)")
try:
    command_number = 0
    while True:
        pcm = stream.read(porcupine.frame_length,exception_on_overflow=False)
        pcm = struct.unpack_from("h" * porcupine.frame_length,pcm)
        keyword_index = porcupine.process(pcm)
        if keyword_index >= 0:
            print("Wake word detected: hi elva")
            filename = f"commands/hi_elva_recording_{command_number%10}.wav"

            record_command(filename,stream,porcupine.sample_rate,porcupine.frame_length)
            print(f"💾 Saved recording as: {filename}\nListening again...")


            stt_v2.stt_function(filename)
            command_number+=1

except KeyboardInterrupt:
    print("Exiting......")
finally:
    stream.stop_stream()
    stream.close()
    audio.terminate()
    porcupine.delete()
    stt_v2.leopard.delete()
    stt_v2.mqtt_pub.publisher.disconnect()
