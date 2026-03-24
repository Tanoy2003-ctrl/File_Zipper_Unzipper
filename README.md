Huffman File Zipper & Unzipper

A lightweight, lossless file compression utility written in C using Huffman Coding. This project demonstrates efficient data structures (Min-Heaps, Binary Trees) and bitwise manipulation to reduce file sizes by assigning variable-length codes to characters based on their frequency.

🚀 Features

*Custom Compression: Uses the Huffman algorithm to compress any file type.

*Cross-Platform: Works on macOS (Clang/GCC) and Windows (MinGW/Visual Studio).

*Binary Support: Handles non-text files (images, PDFs) by processing raw bytes.

*Self-Contained: Stores the frequency table in the file header for standalone decompression.

💻 Installation & Usage

🍎 macOS / Linux

1.Open Terminal and navigate to your project folder:

Bash
cd ~/Path/To/Your/Folder

2.Compile the tools:

Bash
gcc -o zipper zipper.c
gcc -o unzipper unzipper.c

3.Compress a file:

Bash
./zipper input.txt compressed.huff

4.Decompress the file:

Bash
./unzipper compressed.huff restored.txt

🪟 Windows

1.Open Command Prompt (CMD) or PowerShell.

2.Compile the tools (Requires MinGW installed):

DOS

gcc -o zipper.exe zipper.c
gcc -o unzipper.exe unzipper.c
Compress a file:

DOS

zipper.exe input.txt compressed.huff
Decompress the file:

DOS

unzipper.exe compressed.huff restored.txt

📊 Understanding Performance

When to expect high compression:

Huffman coding thrives on redundancy. Large text files or uncompressed data (like .bmp or .txt) will see significant size reductions (often 40%–70%).

When the file size might increase:

1.Small Files: For files under 2KB, the 1,024-byte header (storing the frequency table) may be larger than the data saved.

2.Already Compressed Files: Files like .jpg, .mp3, or .zip are already optimized. Huffman coding may add overhead, making the output slightly larger.

🛠️ How it Works

1.Frequency Analysis: Scans the input file to count occurrences of each of the 256 possible byte values.

2.Tree Construction: Builds a Min-Heap of nodes and merges the two lowest-frequency nodes repeatedly until a single Huffman Tree remains.

3.Encoding: Traverses the tree to assign bit-strings (e.g., 'e' becomes 10, 'z' becomes 110101).

4.Bit-Packing: Packs these variable-length bits into standard 8-bit bytes to be written to the disk.

5.Header Storage: Previews the compressed data with the frequency map so the unzipper can rebuild the exact same tree.
