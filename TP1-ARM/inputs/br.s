.text
adds X0, X10, 1
adds X1, X11, 11
adds X2, X12, 12
cmp X11, X12
cmp X1, X0
b foo
adds X7, X1, 25

foo2:
adds X1, X0, foo2 - .  // Calcula la dirección de foo2 usando X0 como base
br X1                  // Salta a foo2

hlt:
HLT 0

foo:
subs X2, X2, X1    
subs X2, X2, X0
b foo2


HLT 0
