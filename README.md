TODO

This is a minimal port of Sparkfun [MaKeyMakey](https://github.com/sparkfun/MaKeyMaKey) for ESP32S3. Which uses high-impedance resistors and digital filtering to detect contact through media like human skin. 

This version is different in that it exposes a USB-MIDI device instead of the keyboard/mouse HID used in the original code. 

I picked ESP32S3 because I already had a couple of those boards available. But given the simplicity of the code it should work with many other versions, provided they support the USB device stack. 
