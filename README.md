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
>  push 10 ; X
>  push 23 ; Y
>  push 26 ; W
>  push 28 ; H
>  push offset SPRITE_mario_data ; Sprite data off
>  call LoadSprite
>  ```
>  

<br />

> ### DrawSprite
> _**Arguments:**_
>   - The left X position to draw the sprite
>   - The top  Y position to draw the sprite
>   - An offset to the variable that contains the sprite information
> 
> _**Output:**_ None
> 
>  _**Description:**_ This takes in the variable data, reads the BMP data and draws the sprite at the selected location (excluding the transparent color if it exists)
>
> _**Example usage:**_
>  ```x86asm
>  push 10 ; X
>  push 15 ; Y
>  push offset SPRITE_mario_data ; Sprite data off
>  call DrawSprite
>  ```
>  

<br />

> ### FillScreen
> _**Arguments:**_
>   - The color ID of the chosen background color
> 
> _**Output:**_ None
> 
>  _**Description:**_ This will fill the whole screen with a chosen color
>
> _**Example usage:**_
>  ```x86asm
>  push 40 ; Color ID
>  call FillScreen
>  ```

<br />

> ### HasCollidedPoint
> _**Arguments:**_
>   - The point's X value
>   - The point's Y value
>   - The sprite's X value
>   - The sprite's Y value
>   - The offset to the variable that contains the sprite data
> 
> _**Output:**_
>   - AX will be 0 if there were no errors, otherwise it will be the error code
>   - DI will be 1 if a collision has occured, 0 if not
> 
>  _**Description:**_ This checks if a certain position is within a sprite (excluding transparent parts)
>
> _**Example usage:**_
>  ```x86asm
>  push 20 ; Point X
>  push 20 ; Point Y
>  push 10 ; Sprite X
>  push 15 ; Sprite Y
>  offset SPRITE_mario_data ; Sprite data off
>  call HasCollidedPoint
>  ```

<br />

> ### DrawPoint
> _**Arguments:**_
>   - The point's X value
>   - The point's Y value
>   - The point's color ID
> 
> _**Output:**_ None
> 
>  _**Description:**_ This draws a pixel with a color in a specified position
>
> _**Example usage:**_
>  ```x86asm
>  push 20 ; Point X
>  push 20 ; Point Y
>  push 40 ; Color ID
>  call DrawPoint
>  ```

<br />

> ### DrawRect
> _**Arguments:**_
>   - The X position of the rect
>   - The Y position of the rect
>   - The width of the rect
>   - The height of the rect
>   - The X position of the rect
>   - The outline color of the rect
>   - The fill flag (1 = fill, 0 = no fill)
>   - The fill color of the rect
> 
> _**Output:**_ None
> 
>  _**Description:**_ Draws a rect at a specified position with a colored outline or fill
>
> _**Example usage:**_
>  ```x86asm
>  push 20 ; Rect X
>  push 20 ; Rect Y
>  push 10 ; Rect width
>  push 23 ; Rect height
>  push 40 ; Outline color ID
>  push 1  ; fill flag
>  push 2  ; Fill color ID
>  call DrawRect
>  ```

<br />

> ### DrawLine
> _**Arguments:**_
>   - The color ID of the line
>   - The line width
>   - The start pos' X value
>   - The start pos' Y value
>   - The end pos' X value
>   - The end pos' Y value
> 
> _**Output:**_ None
> 
>  _**Description:**_ Draws a line from a start pos to an end point using Bresenham's line algorithm
>
> _**Example usage:**_
>  ```x86asm
>  push 40 ; Color ID
>  push 1  ; Line width
>  push 10 ; Start pos X
>  push 10 ; Start pos Y
>  push 80 ; End pos X
>  push 70 ; End pos Y
>  call DrawLine
>  ```

<br />

> ### DrawPalette
> _**Arguments:**_
>   - The X value to draw the palette in
>   - The Y value to draw the palette in
> 
> _**Output:**_ None
> 
>  _**Description:**_ Draws the palette in a certain position
>
> _**Example usage:**_
>  ```x86asm
>  push 10  ; The X pos to draw the palette
>  push 10  ; The Y pos to draw the palette
>  call DrawPalette
>  ```

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

## Example usage
```
IDEAL
MODEL small
STACK 100h
DATASEG
    ; Create needed graphics variables (331 bytes, can be lowered with a little modifications if needed)
    include 'gdata.inc'
    ; Asciiz sprite sheet path
    packman_sheet db "packman_spritesheet.bmp", 0

    ; Create the variables that will contain sprite data
    packman dw 5 dup(?)
    ghost   dw 5 dup(?)
CODESEG
start:
	mov ax, @data
	mov ds, ax

	; Activate graphics mode
    	mov ax, 13h
    	int 10h

    	; Load the sprite sheet
    	push 227
  	push 160
  	push offset packman_sheet
  	call LoadSpriteGlobe

    	; Load the sprite sheet's palette
  	call LoadSpriteGlobePalette

    	; Set the transparent color to be black
    	push 0 ; R
  	push 0 ; G
  	push 0 ; B
  	call SetTransparentColor

    	; Load in sprites
    	push 20
	push 1
  	push 12
  	push 13
  	push offset packman
  	call LoadSprite
    
    	push 4
	push 65
  	push 14
  	push 14
  	push offset ghost
  	call LoadSprite

    	; Wait for a keypress
    	mov ah, 1
	int 21h

    	; Draw the packman sprite
    	push 10
  	push 10
  	push offset packman
  	call DrawSprite

    	; Wait for a keypress
    	mov ah, 1
	int 21h

    	; Draw the ghost sprite
    	push 50
  	push 50
  	push offset ghost
  	call DrawSprite

    	; Wait for a keypress
    	mov ah, 1
	int 21h

    	; Check if the point (55,55) colides with the ghost drawn at (50,50)
   	push 55
	push 55
	push 50
	push 50
	push offset ghost
	call HasCollidedPoint
    	; DI should be 1 as they colide

    	; Wait for a keypress
    	mov ah, 1
	int 21h

    	; Wait for a keypress
    	mov ah, 1
	int 21h

    	; Clear the screen
    	push 0
    	call FillScreen

    	; Wait for a keypress
    	mov ah, 1
	int 21h

    	; Draw a point at (100,100)
    	push 100
    	push 100
    	push 1
    	call DrawPoint

    	; Wait for a keypress
    	mov ah, 1
	int 21h

	; Draw a filled rect at (100,125) with the width of 5 and height of 10, outline color is 4 and the filled color is 40
	push 100 ; X
	push 125 ; Y
	push 5   ; W
	push 10  ; H
	push 4   ; Outline color
	push 1   ; Is filled
	push 40  ; Fill color

	; Wait for a keypress
	mov ah, 1
	int 21h

	; Clear the screen
	push 0
	call FillScreen

	; Wait for a keypress
	mov ah, 1
	int 21h

	; Draw a line from (20, 10) to (60, 21) with the color ID of 5 and width of 3
	push 5
	push 3
	push 20
	push 10
	push 60
	push 21
	call DrawLine

	; Draw the current palette at (50,50)
	push 50
	push 50
	call  DrawPalette

	; Wait for a keypress
	mov ah, 1
	int 21h

	; De-activate graphics mode
	mov ax, 2
	int 10h

exit:
	mov ax, 4c00h
	int 21h

; Add all graphics functions
include 'graphics.inc'
END start


```

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
  
