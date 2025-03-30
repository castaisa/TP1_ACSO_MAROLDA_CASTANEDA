.text
    cmp X11, X11
    beq casa_eq
    adds X2, X0, 10

casa_eq:
    adds X11, X11, 1
    cmp X11, X12
    bne rancho_ne
    adds X3, X0, 20

rancho_ne:
    cmp X11, X12
    bgt morada_gt
    adds X4, X0, 30

morada_gt:
    cmp X12, X11
    ble depto_le
    adds X5, X0, 40

depto_le:
    cmp X11, X12
    bge parking_ge
    adds X6, X0, 50

parking_ge:
    HLT 0
