#!/bin/sh

# This script generetas JLink script used to load binary file to flash

baseScript=base.jlink
commmanderScript=flash.jlink
linkerScript=link.ld

# Get Flash addres from linker script
flashAddr="$(grep '^FLASH.*0x'  $linkerScript | grep -E -o '[0-9]{8}' | sed 's/^0*//')"

# Take binary name from argument
binaryName=$1

# Debug options

# Remove previous script
rm $commmanderScript

# Create script
cat $baseScript | sed "s/__binary__/"$binaryName"/" |  sed "s/__flash__/"$flashAddr"/" >> $commmanderScript

# If script was created execute it
if [ !$? ]
then
	JLinkExe -device STM32F411RE -if SWD -speed 4000 -autoconnect 1 -CommanderScript $commmanderScript 
fi

