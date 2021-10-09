#!/bin/sh

curl -O -L http://subtersonic.weebly.com/uploads/1/2/8/1/12816899/subtersonic-_brother_charger_11_typewriter.zip
unzip subtersonic-_brother_charger_11_typewriter.zip

cp "Subtersonic- Brother Charger 11 typewriter/Key 1 press.wav" press_0.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 2 press.wav" press_1.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 3 press.wav" press_2.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 4 press.wav" press_3.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 1 release.wav" release_0.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 2 release.wav" release_1.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 3 release.wav" release_2.wav
cp "Subtersonic- Brother Charger 11 typewriter/Key 4 release.wav" release_3.wav
cp "Subtersonic- Brother Charger 11 typewriter/Shift press.wav" shift_press.wav  
cp "Subtersonic- Brother Charger 11 typewriter/Shift release.wav" shift_release.wav
cp "Subtersonic- Brother Charger 11 typewriter/Space bar press.wav" space_press.wav
cp "Subtersonic- Brother Charger 11 typewriter/Space bar release.wav" space_release.wav
cp "Subtersonic- Brother Charger 11 typewriter/Backspace press.wav" backspace_press.wav
cp "Subtersonic- Brother Charger 11 typewriter/Backspace release.wav" backspace_release.wav
cp "Subtersonic- Brother Charger 11 typewriter/New Line 1.wav" enter_0.wav
cp "Subtersonic- Brother Charger 11 typewriter/New Line 2.wav" enter_1.wav

rm -r "Subtersonic- Brother Charger 11 typewriter"
rm subtersonic-_brother_charger_11_typewriter.zip
