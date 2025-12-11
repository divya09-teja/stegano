🔐 Steganography Using LSB in C
This project implements a **Steganography system in C** using the **Least Significant Bit (LSB)** technique to hide and retrieve secret data inside BMP images.
It demonstrates how low-level programming, bitwise operations, and image processing work together to provide secure data hiding.

Features
* Hide secret data inside 24-bit BMP images
* Extract the hidden file with complete accuracy
* Supports multiple file types (`.txt`, `.c`, `.h`, `.sh`, etc.)
* Validates input files, output capacity, and workflow
* Handles BMP 54-byte header and binary structures
* Magic string (`#*`) verification for encoded images
* Modular design with separate encode/decode pipelines

 Technologies & Concepts Used
* **C Programming**
* **Binary file handling (`fread`, `fwrite`)**
* **Bitwise operations & LSB manipulation**
* **Image format understanding (BMP)**
* **Modular architecture (multiple header & source files)**

Project Structure
├── common.h
├── encode.h / encode.c
├── decode.h / decode.c
├── types.h
├── main.c
├── Makefile
└── sample.bmp (example image)

How It Works
Encoding
1. Read source image
2. Copy BMP header
3. Embed magic string
4. Embed file extension
5. Embed file size
6. Embed file data byte-by-byte using LSB

Decoding
1. Read encoded BMP
2. Verify magic string
3. Extract extension & size
4. Reconstruct original file

Usage (CLI)
./stego -e abc.bmp secret.txt output.bmp     # Encoding
./stego -d output.bmp extracted_secret.txt   # Decoding

Learning Outcomes
* System-level programming
* Understanding file formats and memory buffers
* Handling binary data safely
* Image data manipulation using bitwise logic
* Writing scalable, modular C applications

Example Output
* Original BMP
* Encoded BMP (visually same)
* Extracted file with original content
