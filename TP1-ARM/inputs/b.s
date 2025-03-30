.text
adds X0, X10, 1
adds X1, X11, 11
adds X2, X12, 12
cmp X11, X12
cmp X1, X0
b foo
adds X7, X1, 25

foo:
subs X2, X2, X1    
subs X2, X2, X0
b rancho_final

rancho_final:
adds X1, X20, 4
lsl X1, X1, 4
adds X1, X20, 4
HLT 0
