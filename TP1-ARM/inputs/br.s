.text

start:
movz X20, 0x40
lsl X20, X20, 16
add X20, X20, 20
br X20
adds X12, X12, 32
adds X15, X15, 64

HLT 0
