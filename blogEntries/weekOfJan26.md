I found a framework called JUCE for developing my own plugins. Next week, I'll finish researching how the plugins listed below work, and I'll get started with learning how to use JUCE with the tutorials on JUCE's official website. Since plugin development doesn't seem easy and OTT doesn't seem to complicated, I think that will be the first plugin I'll recreate.

Popular EDM plugins:
- Xfer OTT
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
        - Upwards Compression knob
        - Downwards Compression knob
        - A knob for the gain of each band (high, medium, and low)
        - 3 bars allowing you to change the threshold of each band
- Serum
- Omnisphere

Stuff I looked at:
- "Upwards Vs Downwards Compression & Expansion Explained" (https://www.youtube.com/watch?v=_89TRju4rWw)
- "OTT Explained" (https://www.youtube.com/watch?v=bYyuN4QaQRA)
- "Why Does OTT Sound So Good?" (https://www.youtube.com/watch?v=VhLB5GoLBLM)