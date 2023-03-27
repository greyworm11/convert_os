# ConvertOS
Operating system for simple value conversion operations

## Compilation
First of all, you need to install "yasm" translator, "dd" program and "cl.exe" - Microsoft compiler.
</br>
Bootsect is written using syntax AT&T. To compile use:
>>> *yasm -p gas -f bin -o bootsect.tmp bootsect.asm*

>>> *dd bs=31744 skip=1 if=bootsect.tmp of=bootsect.bin*
</br>

To check bootsect (for example, with qemu) you can try:</br>
>>> *qemu-system-i386w.exe -fda bootsect.bin*
</br>

To compile kernel using Microsoft compiler use:
>>> *cl.exe /GS- /c kernel.cpp*

>>> *link.exe /OUT:kernel.bin /BASE:0x10000 /FIXED /FILEALIGN:512 /MERGE:.rdata=.data /IGNORE:4254 /NODEFAULTLIB /ENTRY:kmain /SUBSYSTEM:NATIVE kernel.obj*

</br>

To check full operating system use:
>>> *qemu-system-i386w.exe -fda bootsect.bin -fdb kernel.bin*

</br>

## OS-supported commands
* **info -** outputs information about the author and OS development tools (assembler, compiler), bootloader parameters - user-selected console font colour OS.
* **clear -** clear all commands entered by the user and the operating system output the user has to enter a new command.
* **shutdown -** switching off the computer.
* **nsconv <number_> <base_1> <base_2> -** converts the number specified in the parameter from the source number system to the destination number system.
* **posixtime <number_> -** prints out the date and time specified by the number that is the time stamp POSIX (time elapsed in seconds since 01.01.1970 00:00:00 (UTC)).
* **wintime <number_> -** prints out the date and time specified by a 64-bit number which is the Windows time stamp (time elapsed in 100-nanosecond intervals since
01.01.1601 00:00:00 UTC).

## Example of using

![info](https://github.com/greyworm11/convert_os/blob/main/4.%20info.JPG?raw=true)

![posixtime](https://github.com/greyworm11/convert_os/blob/main/9.%20posixtime%202.JPG?raw=true)