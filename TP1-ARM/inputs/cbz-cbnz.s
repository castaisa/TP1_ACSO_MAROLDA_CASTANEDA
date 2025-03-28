.text

adds X3, X0, 0x13
cbz X1, X3
movz X2, #0xABCD
movz X3, #0x1234
adds X4, X4, 0x5
cbnz X5, X6
cbnz X4, X6
movz X5, #0xDEAD
movz X6, #0xBEEF

HLT 0
