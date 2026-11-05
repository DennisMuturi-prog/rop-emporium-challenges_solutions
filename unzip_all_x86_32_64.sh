#!/bin/bash

set -e

echo "[*] Processing all x86 (32-bit and 64-bit) ROP Emporium challenges..."

for zip_file in *.zip; do
    [ -e "$zip_file" ] || continue
    
    # Skip ARM and MIPS architecture zip files
    if [[ "$zip_file" == *"arm"* ]] || [[ "$zip_file" == *"mipsel"* ]]; then
        continue
    fi
    
    # Get the target folder name by stripping '.zip' (e.g., badchars32.zip -> badchars32)
    target_name="${zip_file%.zip}"
    
    # Check if the folder already exists and isn't empty
    if [ -d "$target_name" ] && [ "$(ls -A "$target_name")" ]; then
        echo "[*] Folder '$target_name' already exists and is organized, skipping."
        continue
    fi

    echo "[*] Unzipping $zip_file into $target_name/..."
    
    # Create the folder first
    mkdir -p "$target_name"
    
    # Extract directly into the target folder (-d flag)
    unzip -q -o "$zip_file" -d "$target_name"
    
    echo "[+] Organized $target_name successfully."
done

echo "[*] All x86 (32-bit and 64-bit) challenges organized successfully!"
