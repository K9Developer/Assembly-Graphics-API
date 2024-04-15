; ----------------------------
; Graphics API
; Available functions:
;   - LoadSpriteGlobe
;   - UnloadSpriteGlobe
;   - SetTransparentColor
;   - LoadSprite
;   - DrawSprite
;   - LoadSpriteGlobePalette
;   - FillScreen
;   - HasColidedPoint
;   - DrawPoint
;   - DrawRect
;   - DrawLine
;   - DrawPalette
;
; Made by K9Dev / Ilai K
; ----------------------------

; #####################################
;
;   Function Name: LoadSpriteGlobe (PUBLIC)
;   Description: Load the sprite globe file and put the handle into GAPI_globe_file_handle
;   Input:
;      - [STACK] Globe image width
;      - [STACK] Globe image height
;      - [STACK] Globe file path (asciiz)
;   Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;
; #####################################
GAPI_ARG_globe_width  equ [bp+8]
GAPI_ARG_globe_height equ [bp+6]
GAPI_ARG_globe_path   equ [bp+4]
proc LoadSpriteGlobe
    push bp
    mov bp, sp
    

    ; Update the globe width and height
    mov ax, GAPI_ARG_globe_width
    mov [GAPI_globe_width], ax
    mov ax, GAPI_ARG_globe_height
    mov [GAPI_globe_height], ax

    push dx

    ; Open the globe file, get the handle
    mov ah, 3Dh
    mov al, 00h ; readonly
    mov dx, GAPI_ARG_globe_path
    int 21h
    
    pop dx
    ; If errors, keep the error code in AX
    jc @@ret

    ; If no errors, save the file handle and put 0 in AX (no error)
    mov [GAPI_globe_file_handle], ax
    mov ax, 0

    @@ret:
        mov sp, bp
        pop bp
        ret 6
endp LoadSpriteGlobe

; #####################################
;
;   Function Name: UnloadSpriteGlobe (PUBLIC)
;   Description: Close the sprite globe file handle
;   Input: None
;   Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;
; #####################################
proc UnloadSpriteGlobe
    
    push bx

    ; Close the file handle
    mov ah, 3Eh
    mov bx, [GAPI_globe_file_handle]
    int 21h
    
    pop bx

    ; If errors, keep the error code in AX
    jc @@ret

    ; If no errors, save the file handle and put 0 in AX (no error)
    mov ax, 0

    @@ret:
        ret
endp UnloadSpriteGlobe

; #####################################
;
;   Function Name: _AdvanceFilePointer (LOCAL)
;   Description: Advance the file pointer by a number of bytes
;   Input:
;      - [STACK] File handle (expected to be read)
;      - [STACK] Number of bytes to move the pointer from its current position
;   Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;
; #####################################
GAPI_ARG_file_handle   equ [bp+6]
GAPI_ARG_pointer_off   equ [bp+4]
proc _AdvanceFilePointer
    push bp
    mov bp, sp

    push bx
    push dx
    push cx

    ; Advance the file pointer by `GAPI_ARG_bytes_to_read` bytes
    xor cx, cx
    mov ah, 42h
    mov al, 01h
    mov bx, GAPI_ARG_file_handle
    mov dx, GAPI_ARG_pointer_off
    int 21h
    jc @@ret
    mov ax, 0

    @@ret:
        pop cx
        pop dx
        pop bx

        mov sp, bp
        pop bp
        ret 4
endp _AdvanceFilePointer

; #####################################
;
;   Function Name: _SetFilePointer (LOCAL)
;   Description: Set the pointer of the file to a specified position
;   Input:
;      - [STACK] File handle (expected to be read)
;      - [STACK] Number of bytes to move the pointer from its current position
;   Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;
; #####################################
GAPI_ARG_file_handle   equ [bp+6]
GAPI_ARG_pointer_off   equ [bp+4]
proc _SetFilePointer
    push bp
    mov bp, sp

    push bx
    push dx
    push cx

    ; Set the file pointer to `GAPI_ARG_pointer_off`
    xor cx, cx
    mov ah, 42h
    mov al, 00h
    mov bx, GAPI_ARG_file_handle
    mov dx, GAPI_ARG_pointer_off
    int 21h
    jc @@ret
    mov ax, 0

    @@ret:
        pop cx
        pop dx
        pop bx

        mov sp, bp
        pop bp
        ret 4
endp _SetFilePointer

; #####################################
;
;   Function Name: SetTransparentColor (PUBLIC)
;   Description: Sets the color to ignore when drawing the sprite (finds the color index in the palette)
;   Input:
;      - [STACK] Red component of the color   (R)
;      - [STACK] Green component of the color (G)
;      - [STACK] Blue component of the color  (B)
;   Output: None
;
; #####################################
GAPI_ARG_red   equ [bp+8]
GAPI_ARG_green equ [bp+6]
GAPI_ARG_blue  equ [bp+4]
proc SetTransparentColor
    push bp
    mov bp, sp

    ; When you set the transparent color,
    ; this function finds that color in the palette and saves its index to filter it out later

    push ax
    push bx
    push cx
    push dx

    ; Convert byte RGB colors to 6-bit colors
    shr GAPI_ARG_red,2
    shr GAPI_ARG_green,2
    shr GAPI_ARG_blue,2

    ; Set port data address
    mov dx, 03C7h
    mov al, 0
    out dx, al
    mov dx, 03C9h
    mov cx, 256
    xor bx, bx
    @@get_curr_color:
        push cx

        ; Read the red component
        in al, dx
        cmp al, GAPI_ARG_red
        jne @@first_bad

        ; Read the green component
        in al, dx
        cmp al, GAPI_ARG_green
        jne @@second_bad

        ; Read the blue component
        in al, dx
        cmp al, GAPI_ARG_blue
        jne @@skip_color

        ; Found the color, save the index
        mov [GPAI_transparent_clr_id], bl

        ; Make sure to move to the next color
        @@first_bad:
            in al, dx
        @@second_bad:
            in al, dx

        @@skip_color:
            inc bx
            pop cx
            loop @@get_curr_color

    @@ret:
        pop dx
        pop cx
        pop bx
        pop ax

        mov sp, bp
        pop bp
        ret 6
endp SetTransparentColor

; #####################################
;
;   Function Name: LoadSprite (PUBLIC)
;   Description: Copy all data of a sprite (area from BMP) to a variable
;   Input:
;      - [STACK] The X position of the sprite (left) in the BMP
;      - [STACK] The Y position of the sprite (top) in the BMP
;      - [STACK] The width of the sprite
;      - [STACK] The height of the sprite
;      - [STACK] The offset to the variable that will store the sprite data
;   Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;   Notes: The sprite data will be in the following format: w,h,x,y,byte offset
;
; #####################################
GAPI_ARG_sprite_x      equ [word bp+12]
GAPI_ARG_sprite_y      equ [word bp+10]
GAPI_ARG_sprite_width  equ [word bp+8]
GAPI_ARG_sprite_height equ [word bp+6]
GAPI_ARG_sprite_data   equ [word bp+4]
GAPI_LOC_off           equ [word bp-2]
GAPI_LOC_flippedY      equ [word bp-4]
proc LoadSprite
    push bp
    mov bp, sp
    sub sp, 4

    ; This function saves the sprite data in the following format: w,h,x,y,byte offset
    ; For later use.

    push bx
    push cx
    push dx
    push di

    ; Check if the sprite is too big
    mov ax, GAPI_ARG_sprite_width
    mov bx, GAPI_ARG_sprite_height
    xor dx, dx
    mul bl
    cmp ax, 320*200
    jbe @@noerr

    ; ERROR CODE (65h) -> Sprite too big
    mov ax, 65h
    jmp @@ret

    @@noerr:

    ; Save Y before flipping
    mov ax, GAPI_ARG_sprite_y
    mov GAPI_LOC_flippedY, ax

    ; Get the byte offset of the sprite in the BMP
    mov GAPI_LOC_off, 54 + 400h ; header + palette

    ; Flip Y
    mov ax, [GAPI_globe_height]
    sub ax, GAPI_ARG_sprite_y
    sub ax, GAPI_ARG_sprite_height
    mov GAPI_ARG_sprite_y, ax

    ; Calculate bytes to skip to get to sprite start position
    mov ax, [GAPI_globe_width]
    mov bx, GAPI_ARG_sprite_y
    cmp GAPI_ARG_sprite_y, 0 ; check before flip
    je @@skp
    dec bx
    @@skp:
    xor dx, dx
    mul bx
    add ax, GAPI_ARG_sprite_x
    add GAPI_LOC_off, ax

    ; Check if the sprite is out of the BMP (x)
    mov ax, GAPI_ARG_sprite_x
    add ax, GAPI_ARG_sprite_width
    cmp ax, [GAPI_globe_width]
    jg @@x_overflow_error

    ; Check if the sprite is out of the BMP (y)
    mov ax, GAPI_LOC_flippedY
    add ax, GAPI_ARG_sprite_height
    cmp ax, [GAPI_globe_height]
    jg @@y_overflow_error

    ; Save the sprite data
    mov si, GAPI_ARG_sprite_data
    mov ax, GAPI_ARG_sprite_x
    mov [word ds:si], ax
    mov ax, GAPI_ARG_sprite_y
    mov [word ds:si+2], ax
    mov ax, GAPI_ARG_sprite_width
    mov [word ds:si+4], ax
    mov ax, GAPI_ARG_sprite_height
    mov [word ds:si+6], ax
    mov ax, GAPI_LOC_off
    mov [word ds:si+8], ax

    jmp @@cont

    @@x_overflow_error:
        ; ERROR CODE (66h) -> X overflow
        mov ax, 66h
        jmp @@ret

    @@y_overflow_error:
        ; ERROR CODE (67h) -> Y overflow
        mov ax, 67h
        jmp @@ret

    @@cont:
        mov ax, 0

    @@ret:
        pop di
        pop dx
        pop cx
        pop bx

        mov sp, bp
        pop bp
        ret 10
endp LoadSprite

; #####################################
;
;   Function Name: DrawSprite (PUBLIC)
;   Description: Draw a sprite on the screen
;   Input:
;      - [STACK] The x position to draw the sprite
;      - [STACK] The y position to draw the sprite
;      - [STACK] The offset to the variable that stores the sprite data
;   Output: None
;   Notes: The sprite data will be in the following format: x,y,w,h,byte offset
;
; #####################################
GAPI_ARG_draw_x                 equ [word bp+8]
GAPI_ARG_draw_y                 equ [word bp+6]
GAPI_ARG_draw_data              equ [word bp+4]
GAPI_LOC_spriteX                equ [word bp-2]
GAPI_LOC_spriteY                equ [word bp-4]
GAPI_LOC_spriteW                equ [word bp-6]
GAPI_LOC_spriteH                equ [word bp-8]
GAPI_LOC_spriteOff              equ [word bp-10]
proc DrawSprite
    push bp
    mov bp, sp
    sub sp, 10
    
    push ax
    push bx
    push cx
    push dx
    push di
    push si
    push es

    ; Make ES point to the video memory
    mov ax, 0A000h
    mov es, ax

    ; Get the sprite data
    xor bx, bx
    mov si, GAPI_ARG_draw_data
    mov bx, [word ds:si]
    mov GAPI_LOC_spriteX, bx
    mov bx, [word ds:si+2]
    mov GAPI_LOC_spriteY, bx
    mov bx, [word ds:si+4]
    mov GAPI_LOC_spriteW, bx
    mov bx, [word ds:si+6]
    mov GAPI_LOC_spriteH, bx
    mov bx, [word ds:si+8]
    mov GAPI_LOC_spriteOff, bx

    ; Prepare the video memory offset (DI -> video memory index) | start from end because Y is flipped
    mov ax, GAPI_ARG_draw_y
    mov bx, 320
    xor dx, dx
    mul bx
    mov di, ax

    mov ax, GAPI_LOC_spriteH
    dec ax
    xor dx, dx
    mul bx

    add di, ax
    add di, GAPI_ARG_draw_x

    ; Advance the file pointer to the sprite data
    mov ax, GAPI_LOC_spriteOff
    push [GAPI_globe_file_handle]
    push ax
    call _SetFilePointer

    mov cx, GAPI_LOC_spriteH
    @@line_loop:
        push cx
        
        ; Read the line
        xor ax, ax
        mov ah, 3Fh
        mov bx, [GAPI_globe_file_handle]
        mov cx, [GAPI_globe_width]
        mov dx, offset GAPI_curr_bmp_line
        int 21h
        jc @@ret

        ; Draw the line
        mov cx, GAPI_LOC_spriteW
        mov bx, offset GAPI_curr_bmp_line
        @@col_loop:
            xor ax, ax
            mov al, [byte ds:bx]
            cmp al, [GPAI_transparent_clr_id]
            je @@skip_draw

            mov [byte es:di], al
            
            @@skip_draw:
            inc bx
            inc di
            loop @@col_loop

        ; Move to the next line (Screen)
        sub di, 320 ; skip currently drawn line and move to the next line
        sub di, GAPI_LOC_spriteW

        pop cx
        loop @@line_loop

    mov ax, 0
    @@ret:
        pop es
        pop si
        pop di
        pop dx
        pop cx
        pop bx
        pop ax

        mov sp, bp
        pop bp
        ret 6
endp DrawSprite

; #####################################
;
;  Function Name: LoadSpriteGlobePalette (PUBLIC)
;  Description: Load the sprite globe palette
;  Input: None
;  Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;
; #####################################
proc LoadSpriteGlobePalette
    push bx
    push cx
    push dx
    push si

    ; Skip the BMP header
    push [GAPI_globe_file_handle]
    push 54 ; Skip header
    call _SetFilePointer

    ; Load the palette
    mov cx, 256
    mov dx, 3C8h
    mov al, 0
    out dx, al
    inc dx
    @@copy_color:

        push cx

        ; Read the palette
        mov ax, 3F00h
        mov bx, [GAPI_globe_file_handle]
        mov cx, 4
        mov dx, offset GPAI_globe_palette_clr
        int 21h
        jc @@ret

        mov si, offset GPAI_globe_palette_clr
        mov dx, 3C9h

        mov al, [si+2] ; Red
        shr al,2 ; divide by 4 (because of 6 bit color)
        out dx,al

        mov al, [si+1] ; Green.
        shr al,2 ; divide by 4 (because of 6 bit color)
        out dx,al

        mov al, [si] ; Blue.
        shr al,2 ; divide by 4 (because of 6 bit color)
        out dx,al
        
        pop cx
        loop @@copy_color

    mov ax, 0

    @@ret:
        pop si
        pop dx
        pop cx
        pop bx

        ret
endp LoadSpriteGlobePalette

; #####################################
;
;  Function Name: FillScreen (PUBLIC)
;  Description: Fill the screen with a single color
;  Input:
;      - [STACK] The color to fill the screen with
;  Output: None
;
; #####################################
GAPI_ARG_color equ [byte bp+4]
proc FillScreen
    push bp
    mov bp, sp

    push ax
    push es
    push di
    push ax
    push cx

    ; Make ES point to the video memory
    mov ax, 0A000h
    mov es, ax

    ; Loop over all pixels and set the color
    mov cx, 320*200
    @@loop:
        mov al, GAPI_ARG_color
        mov [es:di], al
        inc di
        loop @@loop

    @@ret:
        pop cx
        pop ax
        pop di
        pop es
        pop ax

        mov sp, bp
        pop bp
        ret 2
endp FillScreen

; #####################################
;
;  Function Name: HasColidedPoint (PUBLIC)
;  Description: Check if a point has colided with a sprite
;  Input:
;      - [STACK] The x position of the point
;      - [STACK] The y position of the point
;      - [STACK] The offset to the variable that stores the sprite data
;  Output:
;      - [REG]   AX = 0 if no error, otherwise error code
;      - [REG]   DI = 1 if the point has colided with the sprite, otherwise 0
;   Notes: The sprite data will be in the following format: x,y,w,h,byte offset
;
; #####################################
GAPI_ARG_point_x           equ [word bp+12]
GAPI_ARG_point_y           equ [word bp+10]
GAPI_ARG_sprite_x          equ [word bp+8]
GAPI_ARG_sprite_y          equ [word bp+6]
GAPI_ARG_sprite_data       equ [word bp+4]
GAPI_LOC_spriteX           equ [word bp-2]
GAPI_LOC_spriteY           equ [word bp-4]
GAPI_LOC_spriteW           equ [word bp-6]
GAPI_LOC_spriteH           equ [word bp-8]
GAPI_LOC_spriteOff         equ [word bp-10]
GAPI_LOC_point_offset      equ [word bp-12]
proc HasColidedPoint
    push bp
    mov bp, sp
    sub sp, 12

    push bx
    push cx
    push dx
    push si

    ; Calculate the video memory index for point
    mov ax, GAPI_ARG_point_y
    cmp ax, 0
    je @@skp
    dec ax
    @@skp:
    mov bx, 320
    xor dx, dx
    mul bx
    add ax, GAPI_ARG_point_x
    mov GAPI_LOC_point_offset, ax

    ; Get the sprite data
    xor bx, bx
    mov si, GAPI_ARG_draw_data
    mov bx, [word ds:si]
    mov GAPI_LOC_spriteX, bx
    mov bx, [word ds:si+2]
    mov GAPI_LOC_spriteY, bx
    mov bx, [word ds:si+4]
    mov GAPI_LOC_spriteW, bx
    mov bx, [word ds:si+6]
    mov GAPI_LOC_spriteH, bx
    mov bx, [word ds:si+8]
    mov GAPI_LOC_spriteOff, bx

    ; Calculate the video memory index for sprite
    mov ax, GAPI_ARG_sprite_y
    mov bx, 320
    xor dx, dx
    mul bx
    mov di, ax
    mov ax, GAPI_LOC_spriteH
    dec ax
    xor dx, dx
    mul bx
    add di, ax
    add di, GAPI_ARG_sprite_x

    ; Advance the file pointer to the sprite data
    mov ax, GAPI_LOC_spriteOff
    push [GAPI_globe_file_handle]
    push ax
    call _SetFilePointer

    mov cx, GAPI_LOC_spriteH
    @@line_loop:
        push cx
        
        ; Read the line
        xor ax, ax
        mov ah, 3Fh
        mov bx, [GAPI_globe_file_handle]
        mov cx, [GAPI_globe_width]
        mov dx, offset GAPI_curr_bmp_line
        int 21h
        jc @@ret

        ; Go through the line
        mov cx, GAPI_LOC_spriteW
        mov bx, offset GAPI_curr_bmp_line
        @@col_loop:
            xor ax, ax
            
            ; Get the color of the current pixel
            mov al, [byte ds:bx]

            ; Check if the color is the transparent color
            cmp al, [GPAI_transparent_clr_id]
            je @@skip_touch

            ; Check if the point has colided with the sprite
            mov ax, GAPI_LOC_point_offset
            cmp ax, di
            je @@colided

            @@skip_touch:
            inc bx
            inc di
            loop @@col_loop

        ; Move to the next line (Screen)
        sub di, 320 ; skip currently drawn line and move to the next line
        sub di, GAPI_LOC_spriteW

        pop cx
        loop @@line_loop

    mov di, 0
    mov ax, 0
    jmp @@ret
    @@colided:
        mov di, 1
        mov ax, 0
        jmp @@ret

    @@ret:
        pop si
        pop dx
        pop cx
        pop bx

        mov sp, bp
        pop bp
        ret 10
endp HasColidedPoint

; #####################################
;
;  Function Name: DrawPoint (PUBLIC)
;  Description: Draw a point on the screen
;  Input:
;      - [STACK] The x position of the point
;      - [STACK] The y position of the point
;      - [STACK] The color of the point
;  Output: None
;
; #####################################
GAPI_ARG_point_x     equ [word bp+8]
GAPI_ARG_point_y     equ [word bp+6]
GAPI_ARG_point_color equ [byte bp+4]
proc DrawPoint
    push bp
    mov bp, sp

    push ax
    push bx
    push cx
    push dx
    push es

    ; Make ES point to the video memory
    mov ax, 0A000h
    mov es, ax

    ; Calculate the video memory index
    mov ax, GAPI_ARG_point_y
    cmp ax, 0
    je @@skp
    dec ax
    @@skp:
    mov bx, 320
    xor dx, dx
    mul bx
    add ax, GAPI_ARG_point_x
    mov di, ax

    ; Draw the point
    mov al, GAPI_ARG_point_color
    mov [es:di], al

    @@ret:
        pop es
        pop dx
        pop cx
        pop bx
        pop ax

        mov sp, bp
        pop bp
        ret 6

endp DrawPoint

; #####################################
;
;  Function Name: DrawRect (PUBLIC)
;  Description: Draw a rectangle on the screen
;  Input:
;      - [STACK] The x position of the rectangle
;      - [STACK] The y position of the rectangle
;      - [STACK] The width of the rectangle
;      - [STACK] The height of the rectangle
;      - [STACK] The color of the rectangle
;      - [STACK] The fill flag (0 = no fill, 1 = fill)
;      - [STACK] The fill color
;  Output: None
;
; #####################################
GAPI_ARG_rect_x          equ [word bp+16]
GAPI_ARG_rect_y          equ [word bp+14]
GAPI_ARG_rect_width      equ [word bp+12]
GAPI_ARG_rect_height     equ [word bp+10]
GAPI_ARG_rect_color      equ [byte bp+8]
GAPI_ARG_rect_fill       equ [byte bp+6]
GAPI_ARG_rect_fill_clr   equ [byte bp+4]
proc DrawRect
    push bp
    mov bp, sp

    push ax
    push bx
    push cx
    push dx
    push es
    push di

    ; Make ES point to the video memory
    mov ax, 0A000h
    mov es, ax

    ; Calculate the video memory index
    mov ax, GAPI_ARG_rect_y
    cmp ax, 0
    je @@skp
    dec ax
    @@skp:
    mov bx, 320
    xor dx, dx
    mul bx
    add ax, GAPI_ARG_rect_x
    mov di, ax

    mov al, GAPI_ARG_rect_color
    
    ; Draw the outline
    push di
    mov cx, GAPI_ARG_rect_width
    @@draw_top:
        mov [es:di], al
        inc di
        loop @@draw_top
    
    mov cx, GAPI_ARG_rect_height
    @@draw_right:
        mov [es:di], al
        add di, 320
        loop @@draw_right
    
    mov cx, GAPI_ARG_rect_width
    @@draw_bottom:
        mov [es:di], al
        dec di
        loop @@draw_bottom
    
    mov cx, GAPI_ARG_rect_height
    @@draw_left:
        mov [es:di], al
        sub di, 320
        loop @@draw_left
    pop di

    cmp GAPI_ARG_rect_fill, 0
    je @@ret

    ; Fill the rectangle
    mov al, GAPI_ARG_rect_fill_clr
    mov cx, GAPI_ARG_rect_height
    dec cx
    add di, 320
    @@fill_loop:
        push cx
        mov cx, GAPI_ARG_rect_width
        dec cx
        inc di
        @@fill_line:
            mov [es:di], al
            inc di
            loop @@fill_line
        pop cx
        add di, 320
        sub di, GAPI_ARG_rect_width
        loop @@fill_loop



    @@ret:
        pop di
        pop es
        pop dx
        pop cx
        pop bx
        pop ax

        mov sp, bp
        pop bp
        ret 14

endp DrawRect

GAPI_ARG_y2       equ [word bp+4]
GAPI_ARG_x2       equ [word bp+6]
GAPI_ARG_y1       equ [word bp+8]
GAPI_ARG_x1       equ [word bp+10]
GAPI_ARG_width    equ [word bp+12]
GAPI_ARG_color    equ [word bp+14]
GAPI_LOC_deltX        equ [word bp-2]
GAPI_LOC_deltY        equ [word bp-4]
GAPI_LOC_slope        equ [word bp-6]
GAPI_LOC_error        equ [word bp-8]
GAPI_LOC_y            equ [word bp-10]
GAPI_LOC_x            equ [word bp-12]
GAPI_LOC_ystep        equ [word bp-14]
proc DrawLine
    ; Taken from https://github.com/anushaihalapathirana/Bresenham-line-drawing-algorithm/blob/master/src/index.js

    push bp
    mov bp, sp
    
    sub sp, 14
    
    push ax
    push bx
    push cx
    push dx
    push di
    push es

    ; dx = abs(x2 - x1)
    mov ax, GAPI_ARG_x2
    sub ax, GAPI_ARG_x1
    push ax
    call _AbsNum
    mov GAPI_LOC_deltX, di

    ; dy = abs(y2 - y1)
    mov ax, GAPI_ARG_y2
    sub ax, GAPI_ARG_y1
    push ax
    call _AbsNum
    mov GAPI_LOC_deltY, di

    ; GAPI_LOC_slope = dy > dx
    mov ax, GAPI_LOC_deltY
    cmp ax, GAPI_LOC_deltX
    jg @@dyGreater
    mov GAPI_LOC_slope, 0
    jmp @@cont
    @@dyGreater:
        mov GAPI_LOC_slope, 1
    @@cont:
    
    ; if GAPI_LOC_slope
    mov ax, GAPI_LOC_slope
    cmp ax, 0
    je @@skip1

    ; x1, y1 = y1, x1
    ; x2, y2 = y2, x2
    mov ax, GAPI_ARG_x1
    mov bx, GAPI_ARG_y1
    mov GAPI_ARG_x1, bx
    mov GAPI_ARG_y1, ax
    mov ax, GAPI_ARG_x2
    mov bx, GAPI_ARG_y2
    mov GAPI_ARG_x2, bx
    mov GAPI_ARG_y2, ax

    @@skip1:
    
    ; if x1 > x2
    mov ax, GAPI_ARG_x1
    cmp ax, GAPI_ARG_x2
    jle @@skip2

    ; x1, x2 = x2, x1
    ; y1, y2 = y2, y1
    mov ax, GAPI_ARG_x1
    mov bx, GAPI_ARG_x2
    mov GAPI_ARG_x1, bx
    mov GAPI_ARG_x2, ax
    mov ax, GAPI_ARG_y1
    mov bx, GAPI_ARG_y2
    mov GAPI_ARG_y1, bx
    mov GAPI_ARG_y2, ax
    
    @@skip2:
    
    ; dx = abs(x2 - x1)
    mov ax, GAPI_ARG_x2
    sub ax, GAPI_ARG_x1
    push ax
    call _AbsNum
    mov GAPI_LOC_deltX, di

    ; dy = abs(y2 - y1)
    mov ax, GAPI_ARG_y2
    sub ax, GAPI_ARG_y1
    push ax
    call _AbsNum
    mov GAPI_LOC_deltY, di

    ; GAPI_LOC_error = dx / 2 (round)
    mov ax, GAPI_LOC_deltX
    shr ax, 1
    mov GAPI_LOC_error, ax
    
    ; y = y1
    mov ax, GAPI_ARG_y1
    mov GAPI_LOC_y, ax
    
    ; GAPI_LOC_ystep = 1 if y1 < y2 else -1
    mov GAPI_LOC_ystep, 0
    mov ax, GAPI_ARG_y1
    cmp ax, GAPI_ARG_y2
    je @@cont2
    jl @@y1LessY2
    mov GAPI_LOC_ystep, -1
    jmp @@cont2
    @@y1LessY2:
        mov GAPI_LOC_ystep, 1
    @@cont2:

    ; for x in range(x1, x2 + 1)
    mov ax, GAPI_ARG_x1
    mov cx, GAPI_LOC_deltX
    inc cx
    mov GAPI_LOC_x, ax
    @@forX:

        ; coord = (y, x) if GAPI_LOC_slope else (x, y) -> draw
        cmp GAPI_LOC_slope, 0
        je @@c1
        
        push GAPI_LOC_y
        push GAPI_LOC_x
        push GAPI_ARG_width
        push GAPI_ARG_width
        push GAPI_ARG_color
        push 0
        push GAPI_ARG_color
        call DrawRect

        jmp @@cont3

        @@c1:
            push GAPI_LOC_x
            push GAPI_LOC_y
            push GAPI_ARG_width
            push GAPI_ARG_width
            push GAPI_ARG_color
            push 0
            push GAPI_ARG_color
            call DrawRect

        @@cont3:

        ; GAPI_LOC_error -= dy
        mov ax, GAPI_LOC_error
        sub ax, GAPI_LOC_deltY
        mov GAPI_LOC_error, ax

        ; if GAPI_LOC_error < 0
        mov ax, GAPI_LOC_error
        cmp ax, 0
        jge @@skip3
            ; y += GAPI_LOC_ystep
            mov ax, GAPI_LOC_y
            cmp GAPI_LOC_ystep, 1
            jl @@yStepNeg
            inc ax
            jmp @@cont4
            @@yStepNeg:
                dec ax
            @@cont4:

    
            mov GAPI_LOC_y, ax

            ; GAPI_LOC_error += dx
            mov ax, GAPI_LOC_error
            add ax, GAPI_LOC_deltX
            mov GAPI_LOC_error, ax
        @@skip3:

        inc GAPI_LOC_x
        loop @@forX


    @@ret:
    
        pop es
        pop di
        pop dx
        pop cx
        pop bx
        pop ax
        
        mov sp, bp
        pop bp
        
        ret 12
endp DrawLine

; #####################################
;
;  Function Name: _AbsNum (LOCAL)
;  Description: Get the absolute value of a number
;  Input:
;      - [STACK] The number to get its absolute value
;  Output:
;      - [REG]   DI = The absolute value of the number
;
; #####################################
num equ [word bp+4]
proc _AbsNum
    push bp
    mov bp, sp
    
    cmp num, 0
    jl @@neg
    jmp @@cont
    @@neg:
        neg num
    @@cont:
    mov di, num

    @@ret:
        pop bp
        ret 2
endp _AbsNum

; #####################################
;
;  Function Name: DrawPalette (PUBLIC)
;  Description: Draw the palette on the screen
;  Input:
;      - [STACK] The x position of the palette
;      - [STACK] The y position of the palette
;  Output: None
;
; #####################################
GAPI_ARG_palette_y equ [word bp+6]
GAPI_ARG_palette_x equ [word bp+4]
proc DrawPalette
    push bp
    mov bp, sp

    push ax
    push bx
    push cx
    push dx
    push es

    ; Make ES point to the video memory
    mov ax, 0A000h
    mov es, ax

    mov bx, GAPI_ARG_palette_x ; Backup the x position

    ; Draw the palette
    xor dx, dx ; color index
    mov cx, 16
    @@row_loop:
        push cx
        mov cx, 16
        @@col_loop:
            push GAPI_ARG_palette_x
            push GAPI_ARG_palette_y
            push 5
            push 5
            push dx
            push 1
            push dx
            call DrawRect

            add GAPI_ARG_palette_x, 7
            inc dx
            loop @@col_loop

        add GAPI_ARG_palette_y, 7
        mov GAPI_ARG_palette_x, bx
        pop cx
        loop @@row_loop

    @@ret:
        pop es
        pop dx
        pop cx
        pop bx
        pop ax

        mov sp, bp
        pop bp
        ret 4
    
endp DrawPalette

; ------+++===== Made by K9Dev / Ilai K =====+++------
