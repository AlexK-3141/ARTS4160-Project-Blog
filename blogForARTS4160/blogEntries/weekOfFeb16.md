To learn the basics of JUCE, I wanted to make a plugin that plays a single sound when a button is pressed, but it took me way longer than expected to set up everything properly. So far, I have made a JUCE project called OneSound using the basic plugin template. I've yet to do anything with it.

Some issues I ran into:
- I had a bit of trouble with running the plugin template because I initially didn't realize I had to use Visual Studio instead of VSCode
- Since I never used Visual Studio before, it took me a bit to figure out how everything works, especially where to see my repository files
- The JUCE plugin host initially couldn't find my OneSound plugin although I added paths for the host to search for it - fixed this by making the paths not go too deep into my repository

Resources used:
- Tutorial: Projucer Part 1: Getting started with the Projucer (https://docs.juce.com/master/tutorial_new_projucer_project.html)
- How to Set Up JUCE on Windows from Scratch (https://www.youtube.com/watch?v=XJ8GfZbFkYM)