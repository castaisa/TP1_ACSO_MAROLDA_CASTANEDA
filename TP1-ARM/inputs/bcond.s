.text
cmp X1, X1
bne fool
adds X2, X0, 10
cmp X1, X2
bne bar_bgt
adds X3, X0, 10

bar_bgt:
adds X4, X0, 5
subs X5, X2, 8
cmp X4, X5
bgt rancho_ble
HLT 0

fool:
adds X6, X0, 10
HLT 0

rancho_ble:
adds X7, X0, 10
cmp X7, X2
ble casa_bge
HLT 0

casa_bge:
adds X8, X0, 8
cmp X8, X5
bge parking_final
adds X9, X5, X2
HLT 0

parking_final:
adds X10, X0, 10
HLT 0

