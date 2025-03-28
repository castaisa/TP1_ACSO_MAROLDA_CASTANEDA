.text
    // Test BEQ y BNE
    cmp X11, X11  // X11 == X11
    beq label_eq  // Debería saltar
    adds X2, X0, 10  // No debería ejecutarse

label_eq:
    adds X11, X11, 1  // Incrementa X11 para que no sea igual a X12
    cmp X11, X12  // X11 != X12
    bne label_ne  // Debería saltar
    adds X3, X0, 20  // No debería ejecutarse

label_ne:
    // Test BGT
    cmp X11, X12  // X11 > X12?
    bgt label_gt  // Salta si X11 > X12
    adds X4, X0, 30  // No debería ejecutarse si X11 > X12

label_gt:
    // Test BLE
    cmp X12, X11  // X12 <= X11?
    ble label_le  // Salta si X12 <= X11
    adds X5, X0, 40  // No debería ejecutarse si X12 <= X11

label_le:
    // Test BGE
    cmp X11, X12  // X11 >= X12?
    bge label_ge  // Salta si X11 >= X12
    adds X6, X0, 50  // No debería ejecutarse si X11 >= X12

label_ge:
    HLT 0
