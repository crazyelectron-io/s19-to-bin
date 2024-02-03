# Convert S19 to binary

bassed on (intel)hex2bin
Feb 2 2022 Herb Johnson

Part of my A18 (COSMAC 1802) assembler utilities, is a pair of Intel hex to binary, and binary to Intel hex, programs. They are included on the  A18 Web page. I updated my Intel-hex to binary program, because of an issue involved with the IDIOT monitor. You don't have to understand COSMAC 1802 code, to follow my notes here.

http://www.retrotechnology.com/memship/a18.html

From that hex2bin code, I created s2bin for Motorola S-record to binary conversion.

Here's the issues I've fixed with "hex2bin", the Intel hex to binary converter:

1) what if the hex records don't begin at 0000H? Well, you can still make a binary; watch out where you load it. But I report the first address to the user, and write the binary file.

2) What if the hex records aren't continuous? If there's a gap between records. This echos your "uninitialized data" stuff. Well, now I tell the user "here's the gap, I'm filling it with FFH", and I write the binary file. Previously I filled with
zeros 00H. But uninitialized programmable ROMS almost always contain FFH. 

3) What if the hex records overlap?  That is, a following record has a (starting!) address which overwrites the last data address written. Well, that's an error, I report that to the user and stop. They will have to fix the Intel hex file, in some fashion.

4)If the input filename is too long, I stop the program. I set the limit to 40; you can recompile if you want something longer. 

5) What if a hex record has text or spaces at the end? I trap for spaces
and I ignore text after the last hex-character. I compute the expected Intel
record length with the length byte-value and only read off the data bytes expected. I found this when I left a space on a record line.   

OK? Let me know if there's any gotchas introduced into the program. Or if it fails on some hex conversions.  

It's compiled for a 32-bit MS-DOS world, such as the Windows command-line DOS "window". It won't run on, say, an MS-DOS 5.0 computer, unless someone or I compile it with say Turbo C under MS-DOS. If I include a 16 bit version it may not be the most current.

## Format

From Wikipedia: SREC (file format)

An SREC format file consists of a series of ASCII text records. The records have the following structure from left to right:

Record structure
S	Type	Byte Count	Address	Data	Checksum


Record type, two characters, an uppercase "S" (0x53) then a numeric digit 0 to 9, defining the type of record.
S1 is a data record. 

Byte count, two hex digits, indicating the number of bytes (hex digit pairs) that follow in the rest of the record (address + data + checksum). This field has a minimum value of 3 for 16-bit address field plus 1 checksum byte, and a maximum value of 255 (0xFF).

Address, four  hex digits as determined by the record type. The address bytes are arranged in big endian format.

Data, a sequence of 2n hex digits, for n bytes of the data. For S1/S2/S3 records, a maximum of 32 bytes per record is typical since it will fit on an 80 character wide terminal screen, though 16 bytes would be easier to visually decode each byte at a specific address.

Checksum, two hex digits, the least significant byte of ones' complement of the sum of the values represented by the two hex digit pairs for the byte count, address and data fields. For example,
if the 8-bit sum of byte+address+data = 9EH, the checksum is 61H which sums to FFH.

Record types

S0 - optional header, with address 0000H, data may be an ASCII string. First record of file.

S1 - data, with some address, some amount of data, up to 72 bytes?.

S5 - optional count, a record with the address value equal to the number of S0+S1 records. Preceeds
last record of file if used.

S9 - start address, with the address value an arbitrary start address for the hex file; or 0000H.
Last record of file. 

EXAMPLE
Here is an example S-Record file. It contains the data �Hello, World\n" to be loaded at address 0
as a file with one data record, with first record with data "HDR", and a start address of 0000H.

S00600004844521B
S110000048656C6C6F2C20576F726C640A9D
S5030001FB
S9030000FC
