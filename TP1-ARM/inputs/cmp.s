.text
adds X0, X0, 1
cmp X0, X1
beq fool
cmp X0, X0
beq bar
HLT 0

fool:
adds X2, X0, 10
HLT 0

bar:
adds X3, X0, 10
cmp X3, 10
beq tavern
HLT 0

tavern:
adds X4, X0, 10
HLT 0
