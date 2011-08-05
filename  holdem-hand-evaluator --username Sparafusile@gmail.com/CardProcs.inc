.DATA?
  ALIGN 16
  theHand           BYTE 12 DUP(?)
  kicker1           DWORD ?
  kicker2           DWORD ?
  kicker3           DWORD ?
  highSet           DWORD ?
  highPair          DWORD ?
  lowPair           DWORD ?

  ALIGN 16
  offCards          BYTE 64 DUP(?)

  scratchText       BYTE 1024 DUP(?)

.DATA
  theDeck           BYTE    7Eh, 7Dh, 7Ch, 7Bh, 7Ah, 79h, 78h, 77h, 76h, 75h, 74h, 73h, 72h
                    BYTE    5Eh, 5Dh, 5Ch, 5Bh, 5Ah, 59h, 58h, 57h, 56h, 55h, 54h, 53h, 52h
                    BYTE    3Eh, 3Dh, 3Ch, 3Bh, 3Ah, 39h, 38h, 37h, 36h, 35h, 34h, 33h, 32h
                    BYTE    1Eh, 1Dh, 1Ch, 1Bh, 1Ah, 19h, 18h, 17h, 16h, 15h, 14h, 13h, 12h
                    BYTE    00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h

.CONST
  STRAIGHT_FLUSH_MASK   EQU 80000000h
  FOUR_OF_A_KIND_MASK   EQU 40000000h
  FULL_HOUSE_MASK       EQU 20000000h
  FLUSH_MASK            EQU 10000000h
  STRAIGHT_MASK         EQU 08000000h
  THREE_OF_A_KIND_MASK  EQU 04000000h
  TWO_PAIR_MASK         EQU 02000000h
  PAIR_MASK             EQU 01000000h

  RANK_MASK             EQU 0000000Fh
  SUIT_MASK             EQU 000000F0h

Swap MACRO r1:REQ, r2:REQ
  xor   r1,r2
  xor   r2,r1
  xor   r1,r2
ENDM

MinMax MACRO min:REQ, max:REQ
  cmp   min, max
  jle   @F
  Swap  min, max
  @@:
ENDM

MinMax_BL MACRO min:REQ, max:REQ, tmp1:REQ, tmp2:REQ
  mov   tmp2, max
  sub   tmp2, min
  sbb   tmp1, tmp1
  and   tmp1, tmp2
  add   min,  tmp1
  sub   max,  tmp1
ENDM

.CODE
showHand PROC pMessage:DWORD
  movzx     eax, BYTE PTR theHand[10]
  push      eax
  movzx     eax, BYTE PTR theHand[9]
  push      eax
  movzx     eax, BYTE PTR theHand[8]
  push      eax
  movzx     eax, BYTE PTR theHand[7]
  push      eax
  movzx     eax, BYTE PTR theHand[6]
  push      eax
  movzx     eax, BYTE PTR theHand[5]
  push      eax
  movzx     eax, BYTE PTR theHand[4]
  push      eax
  movzx     eax, BYTE PTR theHand[3]
  push      eax
  invoke    wsprintf, ADDR scratchText, CTEXT("%s%02X %02X %02X %02X %02X %02X %02X %02X" ), pMessage
  invoke    MessageBox, NULL, ADDR scratchText, NULL, MB_OK

  ret
showHand ENDP

getCardHash: ; eax = rank, ebx = suit, eax = hash
    cmp     eax, '9'
    jle     RANK_DONE

    cmp     eax, 'A'
    jne     @F
    mov     eax, 14
    jmp     RANK_DONE

  @@:
    cmp     eax, 'K'
    jne     @F
    mov     eax, 13
    jmp     RANK_DONE

  @@:
    cmp     eax, 'Q'
    jne     @F
    mov     eax, 12
    jmp     RANK_DONE

  @@:
    cmp     eax, 'J'
    jne     @F
    mov     eax, 11
    jmp     RANK_DONE

  @@:
    cmp     eax, 'T'
    jne     @F
    mov     eax, 10
    jmp     RANK_DONE

  @@:
    invoke  MessageBox, NULL, CTEXT("Bad Rank!"), NULL, MB_OK
    xor     eax, eax
    ret

  RANK_DONE:
    and     eax, 0Fh

    cmp     ebx, 'd'
    jl      SUIT_Ld
    jg      SUIT_Gd
    or      eax, 10h
    ret

  SUIT_Ld:
    cmp     ebx, 'c'
    jne     SUIT_ERROR
    or      eax, 30h
    ret

  SUIT_Gd:
    cmp     ebx, 'h'
    jg      SUIT_Gh
    jne     SUIT_ERROR
    or      eax, 50h
    ret

  SUIT_Gh:
    cmp     ebx, 's'
    jne     SUIT_ERROR
    or      eax, 70h
    ret

  SUIT_ERROR:
    invoke  MessageBox, NULL, CTEXT("Suit Error!"), NULL, MB_OK
    xor     eax, eax
    ret

getCardValues proc listStr:DWORD
  mov       edx, listStr

  mov       DWORD PTR theHand[0], 0

  movzx     eax, BYTE PTR [edx+ 0]
  movzx     ebx, BYTE PTR [edx+ 1]
  call      getCardHash
  mov       BYTE PTR theHand[3], al

  movzx     eax, BYTE PTR [edx+ 3]
  movzx     ebx, BYTE PTR [edx+ 4]
  call      getCardHash
  mov       BYTE PTR theHand[4], al

  movzx     eax, BYTE PTR [edx+ 6]
  movzx     ebx, BYTE PTR [edx+ 7]
  call      getCardHash
  mov       BYTE PTR theHand[5], al

  movzx     eax, BYTE PTR [edx+ 9]
  movzx     ebx, BYTE PTR [edx+10]
  call      getCardHash
  mov       BYTE PTR theHand[6], al

  movzx     eax, BYTE PTR [edx+12]
  movzx     ebx, BYTE PTR [edx+13]
  call      getCardHash
  mov       BYTE PTR theHand[7], al

  movzx     eax, BYTE PTR [edx+15]
  movzx     ebx, BYTE PTR [edx+16]
  call      getCardHash
  mov       BYTE PTR theHand[8], al

  movzx     eax, BYTE PTR [edx+18]
  movzx     ebx, BYTE PTR [edx+19]
  call      getCardHash
  mov       BYTE PTR theHand[9], al

  mov       WORD PTR theHand[10], 0

  ret
getCardValues endp

SortHelp MACRO min:REQ, max:REQ
  cmp   min, max
  jc    @F
  mov   dh, min
  mov   min, max
  mov   max, dh
  @@:
ENDM

sortBy:
  ; Read from memory
  movzx     eax, WORD PTR theHand[3]
  movzx     ebx, WORD PTR theHand[5]
  movzx     ecx, WORD PTR theHand[7]
  movzx     edx, WORD PTR theHand[9]

  mov       dh, 0
  SortHelp  ah, al
  SortHelp  bl, ah
  SortHelp  bh, bl
  SortHelp  cl, bh
  SortHelp  ch, cl
  SortHelp  dl, ch
  cmp       dh, 0
  je        SORT_DONE

  mov       dh, 0
  SortHelp  ah, al
  SortHelp  bl, ah
  SortHelp  bh, bl
  SortHelp  cl, bh
  SortHelp  ch, cl
  cmp       dh, 0
  je        SORT_DONE

  mov       dh, 0
  SortHelp  ah, al
  SortHelp  bl, ah
  SortHelp  bh, bl
  SortHelp  cl, bh
  cmp       dh, 0
  je        SORT_DONE

  SortHelp  ah, al
  SortHelp  bl, ah
  SortHelp  bh, bl

  SortHelp  ah, al
  SortHelp  bl, ah

  SortHelp  ah, al
  mov       dh, 0

  ; Write to memory
  SORT_DONE:
  shl       ebx, 16
  or        ebx, eax
  shl       edx, 16
  or        edx, ecx
  mov       DWORD PTR theHand[3], ebx
  mov       DWORD PTR theHand[7], edx

  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; evaluateHandEx
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
evaluateHandEx:
  ; Sort by Suit/Rank
  call      sortBy

  ; FLUSHES (3.025%) AND STRAIGHT FLUSHES (0.031%)
  xor           eax, eax
  xor           ebx, ebx
  xor           ecx, ecx
  mov           edx, 2
  STR_FL_LOOP:
    add         edx, 1
    cmp         edx, 6
    jg          STR_FL_END

    mov         al, BYTE PTR theHand[edx+0]
    mov         bl, al
    sub         bl, BYTE PTR theHand[edx+3]
    .IF bl < RANK_MASK
      mov       bl, al
      sub       bl, BYTE PTR theHand[edx+4]
      .IF bl < RANK_MASK
        and     al, RANK_MASK
        .IF bl == 4
          or    eax, STRAIGHT_FLUSH_MASK
          ret
        .ELSEIF ecx == 0
          mov   ecx, DWORD PTR theHand[edx+1]   ; ecx = ?5?4?3?2
          mov   ebx, ecx                        ; ebx = ?5?4?3?2
          and   ebx, 000F0F00h                  ; ebx = 00040300
          and   ecx, 0F00000Fh                  ; ecx = 05000002
          bswap ecx                             ; ecx = 02000005
          or    ecx, ebx                        ; ecx = 02040305
          mov   ebx, ecx                        ; ebx = 02040305
          shr   ebx, 12                         ; ebx = 00002040
          and   ecx, 0000FFFFh                  ; ecx = 00000305
          or    ecx, ebx                        ; ecx = 00002345
          shl   eax, 16                         ; eax = 00010000
          or    ecx, eax                        ; ecx = 00012345

          xor   eax, eax                        ; Prevent partial register stalls (maybe)
          xor   ebx, ebx
        .ENDIF
      .ELSE
        mov     bl, al
        and     bl, RANK_MASK
        .IF bl == 5
          add   al, 9                           ; Ace of the same suit
          cmp   al, BYTE PTR theHand[edx-1]
          je    @F
          cmp   al, BYTE PTR theHand[edx-2]
          je    @F
          cmp   al, BYTE PTR theHand[edx-3]
          je    @F

          jmp   STR_FL_LOOP

          @@:
          mov   eax, STRAIGHT_FLUSH_MASK + 5
          ret
        .ENDIF
      .ENDIF
    .ENDIF
  jmp           STR_FL_LOOP
  STR_FL_END:
  .IF ecx > 0
    lea       eax, [ecx+FLUSH_MASK]
    ret
  .ENDIF

  ; MASK OUT THE SUIT OF THE CARDS
  and       DWORD PTR theHand[3], 0F0F0F0Fh
  and       DWORD PTR theHand[7], 0F0F0F0Fh

  call      sortBy
  cmp       BYTE PTR theHand[3], 0Eh
  jne       @F
  mov       BYTE PTR theHand[10], 1
  @@:

  ; FOUR OF A KINDS (0.168%)
  xor           eax, eax
  mov           edx, 2
  FOUR_KIND_LOOP:
    add         edx, 1
    cmp         edx, 6
    jg          FOUR_KIND_DONE
    
    mov         ah, BYTE PTR theHand[edx+0]
    cmp         ah, BYTE PTR theHand[edx+3]
    jne         FOUR_KIND_LOOP

    .IF edx == 3
      mov       al, BYTE PTR theHand[7]
    .ELSE
      mov       al, BYTE PTR theHand[3]
    .ENDIF
    or          eax, FOUR_OF_A_KIND_MASK
    ret    
  FOUR_KIND_DONE:

  ; STRAIGHTS (4.619%)
  xor           eax, eax
  movzx         ebx, BYTE PTR theHand[3]
  mov           edx, 3
  STRAIGHT_LOOP:
    cmp         edx, 6
    jg          STRAIGHT_END
    mov         ecx, 4
    STRAIGHT_INNER_LOOP:
      add       edx, 1
      mov       eax, ebx
      movzx     ebx, BYTE PTR theHand[edx]
      sub       eax, ebx

      jz        STRAIGHT_INNER_LOOP             ; If the two ranks are equal, continue
      cmp       eax, 1                          ; Compare the rank diff to 1
      jne       STRAIGHT_LOOP                   ; If they are not equal, no straight possible, continue
      sub       ecx, 1                          ; Otherwise, the next card in the straight was found
      jnz       STRAIGHT_INNER_LOOP             ; If not all 5 cards in the straight have been found, continue
      lea       eax, [ebx+4+STRAIGHT_MASK]      ; Otherwise, return the straight value
      ret    
  STRAIGHT_END:

  mov           highSet,  0
  mov           highPair, 0
  mov           lowPair,  0
  mov           kicker1,  0
  mov           kicker2,  0
  mov           kicker3,  0
  
  mov           edx, 3
  GROUP_LOOP:
    cmp         edx, 8
    jg          GROUP_END

    movzx       eax, BYTE PTR theHand[edx+0]
    movzx       ebx, BYTE PTR theHand[edx+1]
    sub         ebx, eax
    jnz         DO_KICKERS

    movzx       ebx, BYTE PTR theHand[edx+2]
    sub         ebx, eax
    jnz         DO_PAIRS
    MinMax_BL   eax, highSet, ebx, ecx
    add         edx, 1

    DO_PAIRS:
    MinMax_BL   eax, highPair, ebx, ecx
    MinMax_BL   eax, lowPair, ebx, ecx
    add         edx, 1

    DO_KICKERS:
    MinMax_BL   eax, kicker1, ebx, ecx
    MinMax_BL   eax, kicker2, ebx, ecx
    MinMax_BL   eax, kicker3, ebx, ecx

    add         edx, 1
    jmp         GROUP_LOOP    
  GROUP_END:

  mov           eax, highSet
  mov           ebx, highPair
  .IF eax == 0
    .IF ebx == 0                            ; High Card (17.412%)
      or        eax, kicker1
      shl       eax, 4
      or        eax, kicker2
      shl       eax, 4
      or        eax, kicker3
      shl       eax, 4
      movzx     ebx, BYTE PTR theHand[6]
      or        eax, ebx
      shl       eax, 4
      movzx     ebx, BYTE PTR theHand[7]
      or        eax, ebx
    .ELSE
      mov       eax, lowPair
      .IF eax == 0                          ; Pair (43.823%)
        or      eax, ebx
        shl     eax, 4
        or      eax, kicker1
        shl     eax, 4
        or      eax, kicker2
        shl     eax, 4
        or      eax, kicker3
        or      eax, PAIR_MASK
      .ELSE                                 ; Two Pair (23.496%)
        shl     eax, 4
        or      eax, ebx
        shl     eax, 4
        or      eax, kicker1
        or      eax, TWO_PAIR_MASK
      .ENDIF
    .ENDIF
  .ELSE
    .IF ebx == 0                            ; Three of a kind (4.830%)
      shl       eax, 4
      or        eax, kicker1
      shl       eax, 4
      or        eax, kicker2
      or        eax, THREE_OF_A_KIND_MASK
    .ELSE                                   ; Full House
      shl       eax, 4
      or        eax, ebx
      or        eax, FULL_HOUSE_MASK
    .ENDIF
  .ENDIF
  ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

rankToText PROC rank:DWORD
  mov           eax, rank
  .switch eax
    .case 14
      mov   eax, CTEXT("Ace")
      .break
    .case 13
      mov   eax, CTEXT("King")
      .break
    .case 12
      mov   eax, CTEXT("Queen")
      .break
    .case 11
      mov   eax, CTEXT("Jack")
      .break
    .case 10
      mov   eax, CTEXT("Ten")
      .break
    .case  9
      mov   eax, CTEXT("Nine")
      .break
    .case  8
      mov   eax, CTEXT("Eight")
      .break
    .case  7
      mov   eax, CTEXT("Seven")
      .break
    .case  6
      mov   eax, CTEXT("Six")
      .break
    .case  5
      mov   eax, CTEXT("Five")
      .break
    .case  4
      mov   eax, CTEXT("Four")
      .break
    .case  3
      mov   eax, CTEXT("Three")
      .break
    .case  2
      mov   eax, CTEXT("Two")
      .break
    .case  1
      mov   eax, CTEXT("Ace")
      .break
    .default
      mov   eax, CTEXT("?RANK?")
   .endswitch
  ret
rankToText ENDP

handToText PROC value:DWORD, pText:DWORD
  mov       eax, value

  test      eax, STRAIGHT_FLUSH_MASK
  jz        @F
  and       eax, 0Fh
  .IF eax == 14
    invoke  lstrcpy, pText, CTEXT("Royal Flush")
  .ELSE
    lea     ebx, [eax-4]
    mov     ecx, $invoke( rankToText, eax )
    mov     edx, $invoke( rankToText, ebx )
    invoke  wsprintf, pText, CTEXT("Straight Flush, %s to %s."), edx, ecx
  .ENDIF
  ret

  @@:
  test      eax, FOUR_OF_A_KIND_MASK
  jz        @F
  mov       ebx, eax
  and       eax, 000Fh
  and       ebx, 0F00h
  shr       ebx, 8
  mov       ecx, $invoke( rankToText, eax )
  mov       edx, $invoke( rankToText, ebx )
  invoke    wsprintf, pText, CTEXT("Four of a Kind, %ss (%s kicker)."), edx, ecx
  ret

  @@:
  test      eax, FULL_HOUSE_MASK
  jz        @F
  mov       ebx, eax
  and       eax, 000Fh
  and       ebx, 00F0h
  shr       ebx, 4
  mov       ecx, $invoke( rankToText, eax )
  mov       edx, $invoke( rankToText, ebx )
  invoke    wsprintf, pText, CTEXT("Full house, %ss full of %ss."), edx, ecx
  ret

  @@:
  test      eax, FLUSH_MASK
  jz        @F
  and       eax, 0F0000h
  shr       eax, 16
  invoke    rankToText, eax
  invoke    wsprintf, pText, CTEXT("%s high Flush."), eax
  ret

  @@:
  test      eax, STRAIGHT_MASK
  jz        @F
  and       eax, 0Fh
  lea       ebx, [eax-4]
  mov       ecx, $invoke( rankToText, eax )
  mov       edx, $invoke( rankToText, ebx )
  invoke    wsprintf, pText, CTEXT("Straight, %s to %s."), edx, ecx
  ret

  @@:
  test      eax, THREE_OF_A_KIND_MASK
  jz        @F
  mov       ebx, eax
  mov       ecx, eax
  mov       edx, eax
  and       ebx, 0F00h
  and       ecx, 00F0h
  and       edx, 000Fh
  shr       ebx, 8
  shr       ecx, 4
  mov       ebx, $invoke( rankToText, ebx )
  mov       ecx, $invoke( rankToText, ecx )
  mov       edx, $invoke( rankToText, edx )
  invoke    wsprintf, pText, CTEXT("Three of a Kind, %ss (%s, %s kicker)."), ebx, ecx, edx
  ret

  @@:
  test      eax, TWO_PAIR_MASK
  jz        @F
  mov       ebx, eax
  mov       ecx, eax
  mov       edx, eax
  and       ebx, 0F00h
  and       ecx, 00F0h
  and       edx, 000Fh
  shr       ebx, 8
  shr       ecx, 4
  mov       ebx, $invoke( rankToText, ebx )
  mov       ecx, $invoke( rankToText, ecx )
  mov       edx, $invoke( rankToText, edx )
  invoke    wsprintf, pText, CTEXT("Two Pair, %ss and %ss (%s kicker)."), ebx, ecx, edx
  ret

  @@:
  test      eax, PAIR_MASK
  jz        @F
  mov       ebx, eax
  mov       ecx, ebx
  and       ecx, 000Fh
  push      $invoke( rankToText, ecx )
  mov       ecx, ebx
  and       ecx, 00F0h
  shr       ecx, 4
  push      $invoke( rankToText, ecx )
  mov       ecx, ebx
  and       ecx, 0F00h
  shr       ecx, 8
  push      $invoke( rankToText, ecx )
  and       ebx, 0F000h
  shr       ebx, 12
  push      $invoke( rankToText, ebx )
  invoke    wsprintf, pText, CTEXT("Pair of %ss (%s, %s, %s kicker).")
  ret

  @@:
  mov       ebx, eax
  mov       ecx, ebx
  and       ecx, 00000Fh
  push      $invoke( rankToText, ecx )
  mov       ecx, ebx
  and       ecx, 0000F0h
  shr       ecx, 4
  push      $invoke( rankToText, ecx )
  mov       ecx, ebx
  and       ecx, 000F00h
  shr       ecx, 8
  push      $invoke( rankToText, ecx )
  mov       ecx, ebx
  and       ecx, 00F000h
  shr       ecx, 12
  push      $invoke( rankToText, ecx )
  mov       ecx, ebx
  and       ecx, 0F0000h
  shr       ecx, 16
  push      $invoke( rankToText, ecx )
  invoke    wsprintf, pText, CTEXT("%s high (%s, %s, %s, %s kicker).")
  ret
handToText ENDP

testProc PROC
  LOCAL text[1000]:BYTE

  invoke    getCardValues, CTEXT("Ad Kd Qd Jd Td 9d 2c")
  call      evaluateHandEx
  .IF eax != STRAIGHT_FLUSH_MASK + 0Eh
    invoke  wsprintf, ADDR text, CTEXT("Royal Flush - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ad 5d 4d 3d 2d 9d 2c")
  call      evaluateHandEx
  .IF eax != STRAIGHT_FLUSH_MASK + 05h
    invoke  wsprintf, ADDR text, CTEXT("Straight Flush, Ace to Five - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ac Kd Qd Jd Td 9d 2c")
  call      evaluateHandEx
  .IF eax != STRAIGHT_FLUSH_MASK + 0Dh
    invoke  wsprintf, ADDR text, CTEXT("Straight Flush, Nine to King - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ac Ad As Ah Td 9h 2c")
  call      evaluateHandEx
  .IF eax != FOUR_OF_A_KIND_MASK + 0E0Ah
    invoke  wsprintf, ADDR text, CTEXT("Four of a kind, Aces (Ten kicker) - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ac Kd Ks Kh Kc 9h 2c")
  call      evaluateHandEx
  .IF eax != FOUR_OF_A_KIND_MASK + 0D0Eh
    invoke  wsprintf, ADDR text, CTEXT("Four of a kind, Kings (Ace kicker) - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ac 2d 2s 2h Td 9h 2c")
  call      evaluateHandEx
  .IF eax != FOUR_OF_A_KIND_MASK + 020Eh
    invoke  wsprintf, ADDR text, CTEXT("Four of a kind, Twos (Ace kicker) - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("2h 3d 6c 8c Tc Qc Ac")
  call      evaluateHandEx
  .IF eax != FLUSH_MASK + 0ECA86h
    invoke  wsprintf, ADDR text, CTEXT("Ace high flush - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Jd Th 9c 8s 7d 2h 3s")
  call      evaluateHandEx
  .IF eax != STRAIGHT_MASK + 0Bh
    invoke  wsprintf, ADDR text, CTEXT("Straight, Seven to Jack - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Jd Th 9c 8s 7d 2h 7s")
  call      evaluateHandEx
  .IF eax != STRAIGHT_MASK + 0Bh
    invoke  wsprintf, ADDR text, CTEXT("Straight, Seven to Jack - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ad 8h 7c 5s 4d 3h 2s")
  call      evaluateHandEx
  .IF eax != STRAIGHT_MASK + 05h
    invoke  wsprintf, ADDR text, CTEXT("Straight, Ace to Five - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("Ac Kd 2s Jd Ts 9d 2c")
  call      evaluateHandEx
  .IF eax != PAIR_MASK + 002EDBh
    invoke  wsprintf, ADDR text, CTEXT("Pair of Twos (Ace, King, Jack kicker) - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    getCardValues, CTEXT("2c 3d 4s 5d 7s 8d 9c")
  call      evaluateHandEx
  .IF eax != 098754h
    invoke  wsprintf, ADDR text, CTEXT("Nine high (Eight, Seven, Five, Four kicker) - failure: (0x%08X)"), eax
    invoke  MessageBox, NULL, ADDR text, NULL, MB_OK
    ret
  .ENDIF

  invoke    MessageBox, NULL, CTEXT("All test cases passed!"), NULL, MB_OK
  xor       eax, eax

  ret
testProc ENDP

CalcHand MACRO card1:REQ, card2:REQ, result:REQ, wins:REQ
  mov       eax, card1
  mov       ebx, card2
  movzx     eax, BYTE PTR theDeck[eax]
  movzx     ebx, BYTE PTR theDeck[ebx]
  shl       eax, 16
  shl       ebx, 24
  or        eax, ebx
  mov       result, eax
  mov       wins, 0
ENDM

evaluateHand PROC USES esi edi ebx listStr:DWORD
  LOCAL playerHands[10]:DWORD
  LOCAL playerScore[10]:DWORD
  LOCAL  i,  j,  k,  l, m
  LOCAL highScore:DWORD
  LOCAL _i, _j, _k, _l
  LOCAL maxLoop:DWORD
  LOCAL numTies:DWORD
  LOCAL useFlop:BOOL
  LOCAL useTurn:BOOL

  ;call              testProc
  ;ret

  xor               esi, esi
  xor               edi, edi
  .WHILE esi < 52
    xor             ecx, ecx
    xor             edx, edx
    .WHILE ecx < 24 && edx == 0
      mov           eax, DWORD PTR playerCards[ecx*4]
      .IF esi == eax
        or          edx, 1
      .ENDIF
      add           ecx, 1
    .ENDW

    .IF edx == 0
      movzx         eax, BYTE PTR theDeck[esi]
      mov           BYTE PTR offCards[edi], al
      add           edi, 1
    .ENDIF

    add             esi, 1
  .ENDW
  mov               maxLoop, edi

  .IF flop1 != 52 && flop2 != 52 && flop3 != 52
    m2m             _i, flop1
    add             _i, 1
    m2m             _j, flop2
    add             _j, 1
    m2m             _k, flop3
    add             _k, 1
    mov             useFlop, TRUE
    .IF turn != 52
      m2m           _l, turn
      add           _l, 1
      mov           useTurn, TRUE

      mov           eax, turn
      movzx         eax, BYTE PTR theDeck[eax]
      mov           edx, eax
      shl           edx, 8
      mov           eax, flop3
      movzx         eax, BYTE PTR theDeck[eax]
      or            edx, eax
      shl           edx, 8
      mov           eax, flop2
      movzx         eax, BYTE PTR theDeck[eax]
      or            edx, eax
      shl           edx, 8
      mov           eax, flop1
      movzx         eax, BYTE PTR theDeck[eax]
      or            edx, eax
    .ELSE
      m2m           _l, maxLoop
      sub           _l, 1
      mov           useTurn, FALSE

      mov           eax, flop3
      movzx         eax, BYTE PTR theDeck[eax]
      mov           edx, eax
      shl           edx, 8
      mov           eax, flop2
      movzx         eax, BYTE PTR theDeck[eax]
      or            edx, eax
      shl           edx, 8
      mov           eax, flop1
      movzx         eax, BYTE PTR theDeck[eax]
      or            edx, eax
    .ENDIF
  .ELSE
    mov             eax, maxLoop
    sub             eax, 1
    mov             _l, eax
    sub             eax, 1
    mov             _k, eax
    sub             eax, 1
    mov             _j, eax
    sub             eax, 1
    mov             _i, eax
    mov             useFlop, FALSE
    mov             useTurn, FALSE
  .ENDIF

  xor               ecx, ecx
  .WHILE ecx < numPlayers
    CalcHand        DWORD PTR playerCards[ecx*8+0], DWORD PTR playerCards[ecx*8+4], DWORD PTR playerHands[ecx*4], DWORD PTR playerWins[ecx*4]
    add             ecx, 1
  .ENDW
  mov               totalHands, 0

  .IF useFlop
    mov             eax, flop1
  .ELSE
    mov             eax, 0
  .ENDIF
  mov               i, eax
  .WHILE eax < _i
    .IF useFlop
      mov           eax, flop2
    .ELSE
      movzx         edx, BYTE PTR offCards[eax]
      add           eax, 1
    .ENDIF
    mov             j, eax
    .WHILE eax < _j
      .IF useFlop
        mov         eax, flop3
      .ELSE
        movzx       ebx, BYTE PTR offCards[eax]
        shl         ebx, 8
        and         edx, 000000FFh
        or          edx, ebx
        add         eax, 1
      .ENDIF
      mov           k, eax
      .WHILE eax < _k
        .IF useTurn
          mov       eax, turn
        .ELSEIF useFlop
          mov       eax, 0
        .ELSE
          movzx     ebx, BYTE PTR offCards[eax]
          shl       ebx, 16
          and       edx, 0000FFFFh
          or        edx, ebx
          add       eax, 1
        .ENDIF
        mov         l, eax
        .WHILE eax < _l
          .IF useTurn
            mov     eax, 0
          .ELSE
            movzx   ebx, BYTE PTR offCards[eax]
            shl     ebx, 24
            and     edx, 00FFFFFFh
            or      edx, ebx
            add     eax, 1
          .ENDIF
          mov       m, eax
          .WHILE eax < maxLoop

            movzx   ecx, BYTE PTR offCards[eax]
            shl     ecx, 8

            ; edx = TTF3F2F1
            ; ecx = 0000RR00

            xor     ebx, ebx
            mov     numTies, 0
            mov     highScore, 0
            .WHILE ebx < numPlayers

              mov   eax, DWORD PTR playerHands[ebx*4]
              mov   DWORD PTR theHand[8], ecx
              mov   DWORD PTR theHand[5], edx
              mov   DWORD PTR theHand[1], eax
              mov   BYTE PTR theHand[0], 0

              push  edx
              push  ecx
              push  ebx
              call  evaluateHandEx
              pop   ebx
              pop   ecx
              pop   edx
              mov   DWORD PTR playerScore[ebx*4], eax
              .IF eax > highScore
                mov numTies, 1
                mov highScore, eax
              .ELSEIF eax == highScore
                add numTies, 1
              .ENDIF

              add   ebx, 1
            .ENDW

            xor     ebx, ebx
            mov     eax, highScore
            SCORING_LOOP:
              cmp   eax, DWORD PTR playerScore[ebx*4]
              jne   NO_HIGHSCORE_MATCH

                cmp wsopOdds, TRUE
                jne @F
                add DWORD PTR playerWins[ebx*4], 1000
                add totalHands, 1
                jmp NO_HIGHSCORE_MATCH

                @@:
                cmp wptOdds, TRUE
                jne @F
                cmp numTies, 1
                jne NO_HIGHSCORE_MATCH
                add DWORD PTR playerWins[ebx*4], 1000
                jmp NO_HIGHSCORE_MATCH

                @@:
                add DWORD PTR playerWins[ebx*4], 1000

              NO_HIGHSCORE_MATCH:
              add   ebx, 1
              cmp   ebx, numPlayers
            jl      SCORING_LOOP
            cmp     wsopOdds, TRUE
            je      @F
            add     totalHands, 1
            @@:

            add     m, 1
            mov     eax, m
          .ENDW    
          add       l, 1
          mov       eax, l
        .ENDW
        
        add         k, 1
        mov         eax, k
      .ENDW
      add           j, 1
      mov           eax, j
    .ENDW
    add             i, 1
    mov             eax, i
  .ENDW

  ret
evaluateHand ENDP

MapFile proc hFile:HANDLE
  LOCAL hFileMap:HANDLE

  ; Create the file map
  mov hFileMap, $invoke( CreateFileMapping, hFile, NULL, PAGE_READWRITE+SEC_COMMIT, 0, 0, NULL )
  .IF eax == NULL
    invoke MessageBox, NULL, CTEXT("Could not create file map!"), CTEXT("Error"), MB_OK
    xor eax, eax
    ret
  .ENDIF

  mov ebx, $invoke( MapViewOfFile, hFileMap, FILE_MAP_WRITE+FILE_MAP_READ, 0, 0, 0 )
  .IF eax == NULL
    invoke MessageBox, NULL, CTEXT("Could not map view of file!"), CTEXT("Error"), MB_OK
    invoke CloseHandle, hFileMap
    xor eax, eax
    ret
  .ENDIF

  invoke CloseHandle, hFileMap
  mov eax, ebx
  ret
MapFile endp