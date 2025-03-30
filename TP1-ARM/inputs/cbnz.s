.text
adds X3, X3, 3
cbnz X1, salto
adds X6, X6, 6
adds X7, X7, 7
adds X4, X4, 4
cbnz X4, saltouno
HLT 0

salto:
adds X5, X5, 5
HLT 0

saltouno:
adds X8, X8, 8

HLT 0
