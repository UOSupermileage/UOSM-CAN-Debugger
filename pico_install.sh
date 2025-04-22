#!/bin/bash

# Ensure a .uf2 file path is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <path-to-uf2-file>"
    exit 1
fi

UF2_FILE="$1"

# Check if the file exists
if [ ! -f "$UF2_FILE" ]; then
    echo "Error: File '$UF2_FILE' not found!"
    exit 1
fi

echo "Found UF2 file: $UF2_FILE"

# Find the Pico drive (assumes it's mounted as /media/$USER/RPI-RP2 or /Volumes/RPI-RP2)
if [ -d "/media/$USER/RPI-RP2" ]; then
    PICO_DRIVE="/media/$USER/RPI-RP2"
elif [ -d "/Volumes/RPI-RP2" ]; then
    PICO_DRIVE="/Volumes/RPI-RP2"
else
    echo "Error: Pico drive not found! Ensure your Pico is in bootloader mode."
    exit 1
fi

# Copy the UF2 file to the Pico drive
echo "Copying $UF2_FILE to the Pico..."
cp "$UF2_FILE" "$PICO_DRIVE/"

# Check if the copy was successful
if [ $? -eq 0 ]; then
    echo "Copy successful! Your Pico should reboot with the new firmware."
else
    echo "Error: Failed to copy the file. Check if the Pico is properly connected."
    exit 1
fi
