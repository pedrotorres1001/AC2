	.equ    RESET_CORE_TIMER, 12
        .equ    READ_CORE_TIMER, 11
        .equ    SFR_BASE_HI, 0xBF88
        .equ    TRISB, 0x6040
	.equ    PORTB, 0x6050
	.equ    LATB, 0x6060
        .data
        .text
        .globl main

main:                                   # void main(void) {
        addiu   $sp, $sp, -12           #       abrir espaco na stack
        sw      $ra, 0($sp)             #       salvaguardar $ra
        sw      $s0, 4($sp)             #       salvaguardar $s0
        sw      $s1, 8($sp)             #       salvaguardar $s1

        lui     $s0, SFR_BASE_HI        #
        lw      $t1, TRISB($s0)         #       $t1 = [TRISB];
        andi    $t1, $t1, 0xFFF0        #       TRISB[3..0] = 0;
        sw      $t1, TRISB($s0)         #       WRITE TRISB Register
        li      $s1, 0                  #       count = 0;
                                        #
loop:                                   #       while(1) {
        lw      $t1, LATB($s0)          #               $t1 = [LATB];
        andi    $t1, $t1, 0xFFF0        #               RB[3..0] = 0;
        or      $t1, $t1, $t2           #               RB[3..0] = count;
        sw      $t1, LATB($s0)          #               WRITE LATB Register;
        li      $a0, 1		        #
        jal     delay                   #               delay(1000);
        addi    $s1, $s1, 1             #               count++;
        andi    $s1, $s1, 0x000F        #               count &= 0x000F
        j       loop                    #       }
        
        lw      $ra, 0($sp)             #       repor $ra
        lw      $s0, 4($sp)             #       salvaguardar $s0
        lw      $s1, 8($sp)             #       salvaguardar $s1
        addiu   $sp, $sp, 12            #       repor espaco na stack
        jr      $ra                     # }

delay:                                  # void delay(int ms) {
        move    $t0, $a0                #       $t0 = ms;
        
for:    ble     $t0, 0, endfor          #       for(; ms > 0; ms--) {
        li      $v0, RESET_CORE_TIMER   #               
        syscall                         #               resetCoreTimer();
read:   li      $v0, READ_CORE_TIMER    #
        syscall                         #               readCoreTimer();
        blt     $v0, 20000, read        #               while(readCoreTimer() < K);
        addi    $t0, $t0, -1            #               ms--;
        j       for                     #       }
endfor:                                 #       
        jr      $ra                     # }
