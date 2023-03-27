.code16
.org 0x7c00
.bss
color:
    .space 4

color_num:
    .space 2

.text
start:
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %ss
    movw $start, %sp
    movw $loading_str, %bx
    call puts
    xor %bx, %bx
    call set_start_pos
    jmp request

cursor_row:
    .byte 0x0C

clear_screen:
    mov $0x0600, %ax
    mov $7, %bh
    mov $0x00, %ch
    mov $0x184F, %dx
    int $0x10
    ret

request:
    movb $0x00, %ah
    int $0x16

    cmp $0x1c, %ah
    je request_end

    cmp $80, %ah
    je DOWN
    cmp $72, %ah
    je UP
    jne request_again
    request_again:
        jmp request

    jmp entry_message
    UP:
        cmp $0x00, (color_num) # всего 6 возможных цветов для выбора
        jz reset_pos_1
        sub $0x01, (color_num)
        sub $1, cursor_row
        call set_cursor
        jmp request
    DOWN:
        add $0x01, (color_num) # смещаем позицию выбранного цвета
        cmp $0x06, (color_num) # всего 6 возможных цветов для выбора
        jz reset_pos
        add $1, cursor_row
        call set_cursor
        jmp request

    set_cursor:
        movb cursor_row, %dh
        movb $0x02, %ah
        movb $0x00, %bh
        int $0x10
        ret
    reset_pos:
        movb $0x0C, cursor_row
        movb $0x00, (color_num)
        movb $0x0C, %dh
        movb $0x04, %dl
        movb $0x02, %ah
        movb $0x00, %bh
        int $0x10
        jmp request
    reset_pos_1:
        movb $0x11, cursor_row
        movb $0x05, (color_num)
        movb $0x11, %dh
        movb $0x04, %dl
        movb $0x02, %ah
        movb $0x00, %bh
        int $0x10
        jmp request
    request_end:
        jmp entry_message

set_start_pos:
    movb $0x0C, %dh
    movb $0x04, %dl
    movb $0x02, %ah
    movb $0x00, %bh
    int $0x10
    ret

entry_message:
    cmp $0x00, (color_num)
    je color_gray
    jmp white_
    color_gray:
        mov $0x08, (color)
    white_:
    cmp $0x01, (color_num)
    je color_white
    jmp yellow_
    color_white:
        mov $0x07, (color)
    yellow_:
    cmp $0x02, (color_num)
    je color_yellow
    jmp blue_
    color_yellow:
        mov $0x0E, (color)
    blue_:
    cmp $0x03, (color_num)
    je color_blue
    jmp red_
    color_blue:
        mov $0x09, (color)
    red_:
    cmp $0x04, (color_num)
    je color_red
    jmp green_
    color_red:
        mov $0x04, (color)
    green_:
    cmp $0x05, (color_num)
    je color_green
    jmp load
    color_green:
        mov $0x02, (color)

load:
    mov $0x1100, %ax
    mov %ax, %es
    mov $0x00, %bx # текстовый режим терминала

    mov $1, %dl # номер диска
    mov $0, %dh # номер головки
    mov $0, %ch # номер цилиндра (старшие 2 бита cl/ch)
    mov $2, %cl # начиная с какого сектора
    mov $32, %al # кол-во секторов

    mov $0x02, %ah
    int $0x13 # прерывание i/o диска

    mov $0x1500, %ax
    mov %ax, %es
    mov $0x00, %bx

# аналогичные действия с другим сектором
    mov $1, %dl
    mov $0, %dh
    mov $0, %ch
    mov $34, %cl # начиная с какого сектора
    mov $6, %al # кол-во секторов

    mov $0x02, %ah
    int $0x13 # прерывание i/o диска

protected:
    xor %bx, %bx
	cli
	lgdt gdt_info
	inb $0x92, %al
	orb $2, %al
	outb %al, $0x92
	movl %cr0, %eax # переход процессора в защищенный режим
	orb $1, %al
	movl %eax, %cr0
	ljmp $0x8, $protected_mode # "дальний" переход для корректной загрузки информации

puts:
    movb 0(%bx), %al
    test %al, %al
    jz end_puts
    movb $0x0e, %ah
    int $0x10
    addw $1, %bx
    jmp puts

    end_puts:
        ret

loading_str:
    .asciz "Choose a color\n\r    Gray\n\r    White\n\r    Yellow\n\r    Blue\n\r    Red\n\r    Green\n\r    "

.code32
protected_mode:
 	movw $0x10, %ax
	movw %ax, %es
	movw %ax, %ds
	movw %ax, %ss
	xor %ebx, %ebx
	mov (color), %ebx
    push %ebx

	call 0x10000
	
gdt:
    # нулевой дескриптор
.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    # сегмент кода
.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    # сегмент данных
.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
	.word gdt_info - gdt
	.word gdt, 0

.zero (512-(. - start) - 2)
.byte 0x55, 0xAA