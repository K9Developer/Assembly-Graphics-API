## What is this
This is a graphics library I made to help me create a school project.
This was made to help to draw different BMP images with the same palette so the colors dont look weird.
You put all of the game's sprites into 1 sprite sheet / sprite globe, then you cut different peices
of the sheet and treat each one as a different sprite.

> [!NOTE]
> 1. All arguemnts are passed via the stack
> 2. I started learning ASM about 6 months ago, so if you have any notes please leave then in the [issues page](https://github.com/K9Developer/Assembly-Graphics-API/issues)


## How to "install"
1. Put the graphics.inc and gdata.inc files in the folder of your main ASM file.
2. Add `include "graphics.inc"` at the end of your ASM file (just before the `END start`)
3. Add `include "gdata.inc"` somewhere in your data segment

## API docs

> ### LoadSpriteGlobe
> _**Arguments:**_
>   - The width of the globe
>   - The height of the globe
>   - The path of the BMP file (the globe) in asciiz ("<path>", 0)\
> 
> _**Output:**_
>   - AX will be 0 if there were no errors, otherwise it will be the error code
> 
>  _**Description:**_ This opens the sprite sheet image and puts the handle into a variable.\
>
> _**Example usage:**_
>  ```x86asm
>  push 320                  ; width
>  push 200                  ; height
>  push offset example_path  ; asciiz path
>  call LoadSpriteGlobe
>  ```
>  
>  _**Notes:**_ This is the first proc you will call

<br />

> ### UnloadSpriteGlobe
> _**Arguments:**_ None
> 
> **_Output:_**
>   - AX will be 0 if there were no errors, otherwise it will be the error code
>     
>  _**Description:**_ This closes the sprite sheet BMP file handle.
> 
>  _**Example usage:**_
>  ```x86asm
>  call UnloadSpriteGlobe
>  ```
>  
>  _**Notes:**_ This is the last proc you will call

<br />

> ### LoadSpriteGlobePalette
> _**Arguments:**_ None
>
> _**Output:**_
>   - AX will be 0 if there were no errors, otherwise it will be the error code
> 
>  _**Description:**_ This will read the palette stored in the loaded BMP and replace the OS' palette with the BMP one.
>
> _**Example usage:**_
>  ```x86asm
>  call LoadSpriteGlobePalette
>  ```
>  
>  _**Notes:**_ This is the 2nd proc you will call.

<br />

> ### SetTransparentColor
> _**Arguments:**_
>   - The Red component of the chosen transparent color
>   - The Green component of the chosen transparent color
>   - The Blue component of the chosen transparent color
> 
> _**Output:**_ None
> 
>  _**Description:**_ This will convert the given color to a 6 bit RGB color, then it will try to find the specified color in the palette, if it found it, it will be stored, if not, no color will be transparent. A transparent color means everytime this color is meant to be drawn it wont be.
>
> _**Example usage:**_
>  ```x86asm
>  push 100                  ; R
>  push 150                  ; G
>  push 255                  ; B
>  call SetTransparentColor
>  ```
>  
>  _**Notes:**_ This is the 3rd proc you will call, it is optional.

<br />

> ### LoadSprite
> _**Arguments:**_
>   - The left X position of the sprite in the sprite sheet
>   - The top  Y position of the sprite in the sprite sheet 
>   - The width of the sprite
>   - The height of the sprite
>   - An offset to a variable that will contain all that data (5 words -> 10 bytes)
> 
> _**Output:**_
>   - AX will be 0 if there were no errors, otherwise it will be the error code
> 
>  _**Description:**_ This will calculate the offset needed to get the sprite from the sprite sheet, it will store all that data in the chosen variable. (x,y,w,h,off)
>
> _**Example usage:**_
>  ```x86asm
>  push 10
>  push 23
>  push 26
>  push 28
>  push offset SPRITE_mario_data
>  call LoadSprite
>  ```
>  

<br />

## Error codes
* 01  Invalid function number
* 02  File not found
* 03  Path not found
* 04  Too many open files (no handles left)
* 05  Access denied
* 06  Invalid handle
* 07  Memory control blocks destroyed
* 08  Insufficient memory
* 09  Invalid memory block address
* 0A  Invalid environment
* 0B  Invalid format
* 0C  Invalid access mode (open mode is invalid)
* 0D  Invalid data
* 0E  Reserved
* 0F  Invalid drive specified
* 10  Attempt to remove current directory
* 11  Not same device
* 12  No more files
* 13  Attempt to write on a write-protected diskette
* 14  Unknown unit
* 15  Drive not ready
* 16  Unknown command
* 17  CRC error
* 18  Bad request structure length
* 19  Seek error
* 1A  Unknown media type
* 1B  Sector not found
* 1C  Printer out of paper
* 1D  Write fault
* 1E  Read fault
* 1F  General failure
* 20  Sharing violation
* 21  Lock violation
* 22  Invalid disk change
* 23  FCB unavailable
* 24  Sharing buffer overflow
* 25  Reserved
* 26  Unable to complete file operation (DOS 4.x)
* 27-31 Reserved
* 32  Network request not supported
* 33  Remote computer not listening
* 34  Duplicate name on network
* 35  Network name not found
* 36  Network busy
* 37  Network device no longer exists
* 38  NetBIOS command limit exceeded
* 39  Network adapter error
* 3A  Incorrect network response
* 3B  Unexpected network error
* 3C  Incompatible remote adapter
* 3D  Print queue full
* 3E  No space for print file
* 3F  Print file deleted
* 40  Network name deleted
* 41  Access denied
* 42  Network device type incorrect
* 43  Network name not found
* 44  Network name limit exceeded
* 45  NetBIOS session limit exceeded
* 46  Temporarily paused
* 47  Network request not accepted
* 48  Print or disk redirection is paused
* 49-4F Reserved
* 50  File already exists
* 51  Reserved
* 52  Cannot make directory entry
* 53  Fail on INT 24
* 54  Too many redirections
* 55  Duplicate redirection
* 56  Invalid password
* 57  Invalid parameter
* 58  Network device fault
* 59  Function not supported by network (DOS 4.x)
* 5A  Required system component not installed (DOS 4.x)
CUSTOM:
* 65  Sprite too big
* 66  X overflow
* 67  Y overflow
  
