.text
mov X1, 0x1000
lsl X1, X1, 16
mov X2, 0x0000ABCD
mov X5, 0x2000
sturh W2, [X5, #4]
ldurh W3, [X5, #4]
adds X0, X3, X1
subs X4, X3, X2
HLT 0
