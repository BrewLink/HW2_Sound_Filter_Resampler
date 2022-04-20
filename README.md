# HW2_Sound_Filter_Resampler
# CS510 - HW2 -  4/10/22
# Ehsan Moghaddam

#### MacOS Build:
                  Includes a make file
                  Instructions - in terminal type:
                    make
                    halfrate ./sine.wav 
                    
                  Test script also generate the files
                    sh btest.sh


#### Xcode project:
                    I have support for a xcode project, but I've excluded them to keep things simple and nice. 

#### Linux Build:
                  Haven't had the time to try
                  but seems like it should be very straight forward given the macs makefile

#### Win Build:
                  Unknown
                  
                 
                 
#### Summary:
##### For this homework, reading the wavfile was very smooth. However, I first built a resmapler in HW, then built a filter in there. I then moved those functions into a new project which I read the wavfile. Most things were smooth. I haven't had time to optimize my code further more but it's already much faster than what was mentioned about python of this HW. I also haven't had time to look into cmake or try this on Linux. For now xcode and Makefile is where this homework is compatible. However, I did have the chance to look into learning more about bash scripting to automate some of the redundant related tasks while working on homework. 
                  
                  
##### Files included:
                        -extras:
                        ./org_songs/*.wav (Given sound files for this hw)
                        ./tests/btesth.sh - (Simple freq test)
                        ./linking_a_files/libportaudio.a
                        rfifths.wav (Additonal test file)
                        
                        -Asked:
                        Makefile (MacOS tested)
                        halfrate.c (main)
                            includes: bandpass_filter_coeff.c/h (given with HW - converted to C)
                        readme.md (this)
                        rgc.wav
                        rsine.wav
                        rsynth.wav
