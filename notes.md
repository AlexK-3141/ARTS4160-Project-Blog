Xfer OTT info:
 - "Multiband upwards/downwards compressor" plugin based on the OTT compressor preset in Ableton
    - Multiband compression: splits the frequency spectrum into different bands (ranges from a lower to a higher frequency), each band can be compressed independently
        - OTT splits frequency spectrum into 3 bands
    - Upwards compression: increases the volume of anything under the threshold
    - Downwards compression: decreases the volume of anything above the threshold
- Components:
    - Depth knob - adjusts dryness/wetness - amount of processing applied to the signal
    - Time knob - adjusts the attack and release of the signal simultaneously
    - In Gain knob - gain of signal before processing
    - Out Gain knob - gain of signal after processing
    - Upwards Compression knob - adjusts the amount the volume of anything under the threshold increases
    - Downwards Compression knob - adjusts the amount the volume of anything above the threshold decreases
    - A knob for the gain of each band (high, medium, and low)
    - 3 bars (sliders) allowing you to change the threshold of each band

Other notes:
- Compressor vs. filter: compressors reduce dynamic range, which is the difference between the loudest and quietest parts of a sound (measured in decibels), while filters manipulate sounds by boosting and/or attenuating (making softer) certain frequency ranges
- The most important compressor parameters:
    - Threshold - the level at which compression begins (measured in decibels) - if the absolute value of the signal's amplitude is above/below this threshold, the compressor will apply negative/positive gain such that the signal stays near or below/above the threshold, depending on if the compressor is applying downwards or upwards compression
    - Ratio - given ratio x:y, for every x db of signal above the threshold, the output amplitude will be y db of signal above the threshold
        - If ratio is infinity:1, compressor turns into brick wall limiter - nothing will be above/below the threshold
    - Attack - how long to wait after the signal exceeds the threshold before applying gain to it (milliseconds)
    - Release - how long to wait after the signal no longer exceeds the threshold before the compressor stops applying gain to it (milliseconds)
- How audio processing actually works: samples are organized into blocks a.k.a buffers before being processed
    - If a buffer isn't processed in time, the buffer will just have silence