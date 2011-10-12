

    ;This version is written for naken430asm.
    ;http://www.mikekohn.net/micro/naken430asm_msp430_assembler.php
    ;naken430asm -o filename.hex filename.s
    ;mspdebug takes hex files as well as elfs.

WDTCTL equ 0x0120


CALBC1_1MHZ equ 0x10FF
CALDCO_1MHZ equ 0x10FE

DCOCTL  equ 0x56
BCSCTL1 equ 0x57
BCSCTL2 equ 0x58

TACTL   equ 0x0160
TAR     equ 0x0170
TACCR0  equ 0x0172
TAIV    equ 0x012E
TACCTL0 equ 0x0162


P1OUT   equ 0x0021
P1DIR   equ 0x0022


    org 0xFC00

reset:
    mov #0x0280,r1

    mov #0x5A80,&WDTCTL ; 0x5A00|WDTHOLD

    ; use calibrated clock
    clr.b &DCOCTL
    mov.b &CALBC1_1MHZ,&BCSCTL1
    mov.b &CALDCO_1MHZ,&DCOCTL

    ; make p1.0 and p1.6 outputs
    bis.b #0x41,&P1DIR
    bic.b #0x41,&P1OUT
    bis.b #0x40,&P1OUT


    ; 1MHz is 1000000 clocks per second
    ; 1000000 = 0xF4240
    ; The timers are 16 bit
    ; Using a divide by 8 in BCSCTL2 gives
    ; 125000 (0x1E848) clocks in a second
    ; Using a divide by 8 in the timer gives
    ; 15625 (0x3D09) timer ticks per second.

    ; If both divisors are by 8, and we set
    ; TACCR0 to 0x3D08 and set for count up mode
    ; then, theory, we can measure seconds.

    bis.b #0x06,&BCSCTL2
    mov #0x10,&TACCTL0
    mov #0x3D08,&TACCR0
    mov #0x02D0,&TACTL

    bis.w #0x0018,r2
    ; cpu should actually be off and not enter this loop
forever:
    jmp forever


intled:
    xor.b #0x41,&P1OUT
    reti

hang:
    jmp hang

    org 0xFFE0
    dw hang  ; 0xFFE0
    dw hang  ; 0xFFE2
    dw hang  ; 0xFFE4
    dw hang  ; 0xFFE6
    dw hang  ; 0xFFE8
    dw hang  ; 0xFFEA
    dw hang  ; 0xFFEC
    dw hang  ; 0xFFEE
    dw hang  ; 0xFFF0
    dw intled; 0xFFF2
    dw hang  ; 0xFFF4
    dw hang  ; 0xFFF6
    dw hang  ; 0xFFF8
    dw hang  ; 0xFFFA
    dw hang  ; 0xFFFC
    dw reset ; 0xFFFE
