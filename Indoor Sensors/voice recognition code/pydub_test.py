# simple_ogg_to_wav_test.py
import os
from pydub import AudioSegment

def convert_ogg_to_wav_simple(ogg_file_path):
    """Simple OGG to WAV conversion test"""
    try:
        print(f"🔄 Converting {ogg_file_path} to WAV...")
        
        # Check if input file exists
        if not os.path.exists(ogg_file_path):
            print(f"❌ File not found: {ogg_file_path}")
            return False
            
        print(f"📁 Input file size: {os.path.getsize(ogg_file_path)} bytes")
        
        # Generate output filename
        wav_file_path = ogg_file_path.replace('.ogg', '.wav')
        
        # Convert OGG to WAV
        print("🎵 Loading OGG file...")
        audio = AudioSegment.from_ogg(ogg_file_path)
        
        print(f"📊 Audio info: {len(audio)}ms, {audio.frame_rate}Hz, {audio.channels} channels")
        
        print("💾 Exporting as WAV...")
        audio.export(wav_file_path, format="wav")
        
        # Verify output
        if os.path.exists(wav_file_path):
            print(f"✅ Conversion successful!")
            print(f"📁 Output file: {wav_file_path}")
            print(f"📁 Output size: {os.path.getsize(wav_file_path)} bytes")
            return True
        else:
            print("❌ Conversion failed - output file not created")
            return False
            
    except Exception as e:
        print(f"❌ Conversion error: {e}")
        return False

# Test the conversion
if __name__ == "__main__":
    print("🎧 OGG to WAV Conversion Test")
    print("=" * 40)
    
    # Test file - replace with your actual OGG file name
    test_ogg_file = "voice_1759755275.ogg"  # Change this to your actual file name
    
    if os.path.exists(test_ogg_file):
        success = convert_ogg_to_wav_simple(test_ogg_file)
        if success:
            print("\n🎉 Conversion test PASSED!")
        else:
            print("\n💥 Conversion test FAILED!")
    else:
        print(f"❌ Test file '{test_ogg_file}' not found in current directory:")
        print("📁 Files in current directory:")
        for file in os.listdir('.'):
            if file.endswith('.ogg'):
                print(f"   - {file}")