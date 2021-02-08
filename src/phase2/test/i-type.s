    .text
    .code

    addi $1, $0, 1
    addi $30, $0, 0
    lui $2, 0x1234
    ori $2, $2, 0xF678
    
    ;; Data Transfer
    ;; M[0] = 0x34
    sw $2, 0($30)
    lw $3, 0($30)
    lh $4, 0($30)
    
    ;; may need add sign bits
    lhu $5, 0($30)
    lb $6, 0($30)

    ;; not register reference, need to refer immediate
    ;; 0x1234
    lui $7, 0x12
    addi $7, 0x34

    ;; M[0x4]=0x34
    sh $7, 0x4($30)

    ;; M[0x8]=0x4
    sb $7, 0x8($30)

    ;; $8=0x1230
    andi $8, $7, 0x30

    ;; $8=0x1239
    ori $8, $7, 0x9

    ;; slti, sltiu
    slti $9, $7, 0x6FFFFFFF

    ;; true
    sltiu $10, $7, 0xFFFFFFFF

    ;; false
    sltiu $10, $7, 0x2FFF

    addi $30, $0, 0 ;; $30=Pointer

    addi $1, $0, 2
    addi $2, $0, 0xFE ;; $15=12;
    
    lui $3, 0xFF
    or $4, $3, $2


addi $1, $0, 1
addi $30, $0, 0
lui $2, 0x1234
ori $2, $2, 0xF678
sw $2, 0($30)
lw $3, 0($30)
lh $4, 0($30)
lhu $5, 0($30)
lb $6, 0($30)
lui $7, 0x12
addi $7, 0x34
sh $7, 0x4($30)
sb $7, 0x8($30)
andi $8, $7, 0x30
ori $9, $7, 0xF
slti $10, $8, 0x6FFF
sltiu $11, $8, 0xFFFF
sltiu $12, $8, 0x2FFF
addi $30, $0, 0
addi $1, $0, 2
addi $2, $0, 0xFE
lui $3, 0xFF
or $4, $3, $2
    