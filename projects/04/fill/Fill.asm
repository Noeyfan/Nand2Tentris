// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

@SCREEN
D=A
@addr
M=D

(WHILE)
  @KBD
  D=M

  @BLACK // if D==0 show black
  D;JNE

  // else set everything to white
  @i
  M=0
  @8192
  D=A
  @n
  M=D
  (WSCREEN)
    @i
    D=M
    @n
    D=M-D

    @WHILE
    D;JEQ

    @i
    D=M
    @addr // find current address to set
    A=M+D
    M=0

    @i
    M=M+1

    @WSCREEN
    0;JMP

  @WHILE
  0;JMP

  (BLACK)
    @i
    M=0
    @8192
    D=A
    @n
    M=D

    (BSCREEN)
      @i
      D=M
      @n
      D=M-D

      @WHILE
      D;JEQ

      @i
      D=M
      @addr // find current address to set
      A=M+D
      M=-1

      @i
      M=M+1

      @BSCREEN
      0;JMP

    @WHILE
    0;JMP
