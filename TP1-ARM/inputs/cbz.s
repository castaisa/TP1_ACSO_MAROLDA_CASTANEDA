.text
    adds X3, X3, 3
    cbz X1, salto
    adds X6, X6, 6
    adds X7, X7, 7

salto:
    adds X4, X4, 4
    cbz X4, saltouno
    adds X5, X5, 5

saltouno:
    adds X8, X8, 8

HLT 0
