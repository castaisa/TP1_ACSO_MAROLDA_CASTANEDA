.text

mov X1, 0x1000
lsl X1, X1, 16
mov X2, 0x0000ABCD 
mov X5, 0x2000
sturh X2, [X5, 0x4]
ldurh X3, [X5, 0x4]
HLT 0