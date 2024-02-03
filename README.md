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

Herb JOhnson
