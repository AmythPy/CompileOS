#!/usr/bin/env python3
"""
CompileOS Splash Screen Test
Tests the splash.bmp file for embedded OS compatibility
"""

import os
import struct

def test_splash_bmp():
    """Test the splash.bmp file for embedded OS compatibility"""
    
    print("CompileOS Splash Screen Test")
    print("=" * 40)
    
    # Check if splash.bmp exists
    if not os.path.exists("splash.bmp"):
        print("❌ ERROR: splash.bmp not found!")
        return False
    
    # Get file size
    file_size = os.path.getsize("splash.bmp")
    print(f"📁 File: splash.bmp")
    print(f"📏 Size: {file_size:,} bytes ({file_size / (1024*1024):.1f} MB)")
    
    # Check if file is large enough for embedded OS
    min_size = 1024 * 1024  # 1MB minimum
    if file_size < min_size:
        print(f"❌ ERROR: File too small! Need at least {min_size:,} bytes")
        return False
    
    print(f"✅ File size is sufficient for embedded OS")
    
    # Read and parse BMP header
    try:
        with open("splash.bmp", "rb") as f:
            # Read BMP header
            header = f.read(14)
            if len(header) < 14:
                print("❌ ERROR: File too small to be a valid BMP")
                return False
            
            # Parse BMP header
            signature = header[0:2]
            file_size_bmp = struct.unpack('<I', header[2:6])[0]
            reserved = struct.unpack('<I', header[6:10])[0]
            data_offset = struct.unpack('<I', header[10:14])[0]
            
            print(f"\n📋 BMP Header Analysis:")
            print(f"   Signature: {signature}")
            print(f"   File Size: {file_size_bmp:,} bytes")
            print(f"   Reserved: {reserved}")
            print(f"   Data Offset: {data_offset:,} bytes")
            
            # Validate BMP signature
            if signature != b'BM':
                print("❌ ERROR: Invalid BMP signature!")
                return False
            
            print("✅ Valid BMP file")
            
            # Read BMP info header
            f.seek(14)
            info_header = f.read(40)
            if len(info_header) < 40:
                print("❌ ERROR: Incomplete BMP info header")
                return False
            
            # Parse BMP info header
            header_size = struct.unpack('<I', info_header[0:4])[0]
            width = struct.unpack('<i', info_header[4:8])[0]
            height = struct.unpack('<i', info_header[8:12])[0]
            planes = struct.unpack('<H', info_header[12:14])[0]
            bits_per_pixel = struct.unpack('<H', info_header[14:16])[0]
            compression = struct.unpack('<I', info_header[16:20])[0]
            image_size = struct.unpack('<I', info_header[20:24])[0]
            
            print(f"\n🖼️  Image Information:")
            print(f"   Width: {width} pixels")
            print(f"   Height: {height} pixels")
            print(f"   Bits per Pixel: {bits_per_pixel}")
            print(f"   Compression: {compression}")
            print(f"   Image Size: {image_size:,} bytes")
            
            # Calculate available space for embedded OS
            image_data_end = data_offset + image_size
            available_space = file_size - image_data_end
            
            print(f"\n💾 Space Analysis:")
            print(f"   Image Data End: {image_data_end:,} bytes")
            print(f"   Available Space: {available_space:,} bytes ({available_space / (1024*1024):.1f} MB)")
            
            # Estimate OS size (rough estimate)
            estimated_os_size = 500 * 1024  # 500KB
            if available_space > estimated_os_size:
                print(f"✅ Sufficient space for embedded OS (need ~{estimated_os_size:,} bytes)")
            else:
                print(f"⚠️  Limited space for embedded OS (need ~{estimated_os_size:,} bytes)")
            
            # Check for existing embedded OS
            f.seek(image_data_end)
            potential_header = f.read(4)
            if potential_header == b'COS1':
                print("🔍 Found existing embedded OS header!")
                return True
            else:
                print("📝 No existing embedded OS found - ready for embedding")
                return True
                
    except Exception as e:
        print(f"❌ ERROR: {e}")
        return False

def estimate_embedding():
    """Estimate the embedding process"""
    print(f"\n🚀 Embedding Process Estimate:")
    print(f"   1. Parse BMP headers ✅")
    print(f"   2. Calculate image data size ✅")
    print(f"   3. Find available space ✅")
    print(f"   4. Embed OS data at end of image")
    print(f"   5. Add OS embed header")
    print(f"   6. Update BMP file size")
    print(f"   7. Verify integrity with checksums")

if __name__ == "__main__":
    success = test_splash_bmp()
    if success:
        estimate_embedding()
        print(f"\n🎉 Your splash.bmp is ready for embedded OS!")
        print(f"   The mystical Tree of Knowledge will contain CompileOS!")
    else:
        print(f"\n❌ Issues found with splash.bmp")
    
    input("\nPress Enter to continue...")










