# MP3ToWAV
##### A simple tool to bulk convert MP3 files to WAVs.

This simple C# program accepts a list of MP3 files as inputs, and converts them to WAV files using NAudio. The most recent version takes advantage of multiple CPU cores, decoding multiple files at once.

### Usage:
Simply drag all of the MP3 files you want to convert onto the Mp3ToWAV exe. It will convert them and save them as WAV files in the same folder as the exe. Be careful, though, as the produced WAV files are *much* larger than the input MP3s. Converting a ton of files is an easy way to fill up a small hard drive.

### Compiling:
Open the solution in Visual Studio and build it. Adding the DLLs as references may be needed. To pack all of the DLLs into one file, use ILMerge. I prefer ILMerge-GUI.

### Todo:
* Add a progress bar of some sort.
* More error handling.
