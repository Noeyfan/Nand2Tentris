// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@EQ_FALSE.2
D;JNE
@2
D=A
@SP
A=M-D
M=-1
(EQ_FALSE.2)
@SP
M=M-1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@EQ_FALSE.5
D;JNE
@2
D=A
@SP
A=M-D
M=-1
(EQ_FALSE.5)
@SP
M=M-1
// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@EQ_FALSE.8
D;JNE
@2
D=A
@SP
A=M-D
M=-1
(EQ_FALSE.8)
@SP
M=M-1
// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@LT_FALSE.11
D;JLT
@2
D=A
@SP
A=M-D
M=0
(LT_FALSE.11)
@SP
M=M-1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@LT_FALSE.14
D;JLT
@2
D=A
@SP
A=M-D
M=0
(LT_FALSE.14)
@SP
M=M-1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@LT_FALSE.17
D;JLT
@2
D=A
@SP
A=M-D
M=0
(LT_FALSE.17)
@SP
M=M-1
// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@GT_FALSE.20
D;JGT
@2
D=A
@SP
A=M-D
M=0
(GT_FALSE.20)
@SP
M=M-1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@GT_FALSE.23
D;JGT
@2
D=A
@SP
A=M-D
M=0
(GT_FALSE.23)
@SP
M=M-1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@GT_FALSE.26
D;JGT
@2
D=A
@SP
A=M-D
M=0
(GT_FALSE.26)
@SP
M=M-1
// push constant 57
@57
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 31
@31
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 53
@53
D=A
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
A=M-1
D=M
A=A-1
M=M+D
@SP
M=M-1
// push constant 112
@112
D=A
@SP
A=M
M=D
@SP
M=M+1
// sub
@SP
A=M-1
D=M
A=A-1
M=M-D
@SP
M=M-1
// neg
@SP
A=M-1
M=-M
// and
@SP
A=M-1
D=M
A=A-1
M=M&D
@SP
M=M-1
// push constant 82
@82
D=A
@SP
A=M
M=D
@SP
M=M+1
// or
@SP
A=M-1
D=M
A=A-1
M=M|D
@SP
M=M-1
// not
@SP
A=M-1
M=!M
