.486
.model flat,stdcall
option casemap:none

include    \masm32\include\windows.inc
include    \masm32\include\user32.inc
includelib \masm32\lib\user32.lib
include    \masm32\include\kernel32.inc
includelib \masm32\lib\kernel32.lib
includelib \masm32\lib\Msimg32.lib

WinMain proto :DWORD,:DWORD,:DWORD,:DWORD
TransparentBlt proto :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD

.DATA
  wsopOdds                  BOOL FALSE
  wptOdds                   BOOL FALSE

  sCardWidth                DWORD 17
  sCardHeight               DWORD 33
  lCardWidth                DWORD 42
  lCardHeight               DWORD 59
  tableWidth                DWORD 582
  tableHeight               DWORD 381

  maxPlayers                DWORD 5
  numPlayers                DWORD 0
  
  totalHands                DWORD 0
  calcDone                  BOOL  FALSE
  playerWins                DWORD 10 DUP(0)
  playerCards               DWORD 24 DUP(52)
  flop1                     EQU DWORD PTR playerCards[20*4]
  flop2                     EQU DWORD PTR playerCards[21*4]
  flop3                     EQU DWORD PTR playerCards[22*4]
  turn                      EQU DWORD PTR playerCards[23*4]

  buttonDown                BYTE FALSE
  isDragging                BYTE FALSE

  playerPrctPos             DWORD  10, 75   ,   132, 40   ,   267, 20   ,   405, 40   ,   520, 75
                            DWORD   0,  0   ,     0,  0   ,     0,  0   ,     0,  0   ,     0,  0

  deckCardPos               DWORD 52*2 DUP(0)

  playerCardPos             DWORD  29,106 ,  75,106     ,     134, 67 , 180, 67     ; player 1,2
                            DWORD 246, 51 , 292, 51     ,     360, 67 , 406, 67     ; player 3,4
                            DWORD 459,107 , 505,107     ,       0,  0 ,   0,  0     ; player 5,6
                            DWORD   0,  0 ,   0,  0     ,       0,  0 ,   0,  0     ; player 7,8
                            DWORD   0,  0 ,   0,  0     ,       0,  0 ,   0,  0     ; player 9,10
                            DWORD 198,184 , 244,184     ,     290,184 , 382,184     ; flop1, flop2, flop3, turn

  flop1X                    EQU DWORD PTR playerCardPos[40*4]
  flop1Y                    EQU DWORD PTR playerCardPos[41*4]
  flop2X                    EQU DWORD PTR playerCardPos[42*4]
  flop2Y                    EQU DWORD PTR playerCardPos[43*4]
  flop3X                    EQU DWORD PTR playerCardPos[44*4]
  flop3Y                    EQU DWORD PTR playerCardPos[45*4]
  turnX                     EQU DWORD PTR playerCardPos[46*4]
  turnY                     EQU DWORD PTR playerCardPos[47*4]

.DATA?
  hInstance                 HINSTANCE ?
  CommandLine               LPSTR ?

  hContextMenu              HWND ?

  hCards                    HWND ?
  hNumPlayers               HWND ?
  hGo                       HWND ?
  hReset                    HWND ?
  hHandValue                HWND ?

  hTableImg                 HANDLE ?
  hTableDC                  HDC ?
  hCardsImg                 HANDLE ?
  hCardsDC                  HDC ?
  hThumbImg                 HANDLE ?
  hThumbDC                  HDC ?

.CONST
  IDB_TABLE                 EQU 1001
  IDB_SMALL                 EQU 1002
  IDB_LARGE                 EQU 1003

  SPADES_X                  EQU 36
  SPADES_Y                  EQU 265
  HEARTS_X                  EQU 289
  HEARTS_Y                  EQU 265
  CLUBS_X                   EQU 289
  CLUBS_Y                   EQU 304
  DIAMONDS_X                EQU 36
  DIAMONDS_Y                EQU 304

  SMALL_CARD_WIDTH          EQU 17
  SMALL_CARD_HEIGHT         EQU 33
  SMALL_CARD_VERT_SPACE     EQU 0
  SMALL_CARD_HORIZ_SPACE    EQU 2

  IDC_GOBTN                 EQU WM_USER
  IDC_RESET                 EQU WM_USER+1
  IDC_NUMPLAYERS            EQU WM_USER+2

include                     Macros.inc
include                     GUI.inc
include                     CardProcs.inc

.CODE
start: 
  mov                       hInstance, $invoke( GetModuleHandle, NULL ) 
  mov                       CommandLine, $invoke( GetCommandLine )
  
  push                      $invoke( WinMain, hInstance, NULL, CommandLine, SW_SHOWDEFAULT )
  call                      ExitProcess
  call                      InitCommonControls

.DATA
  WndClassEx                WNDCLASSEX  <SIZEOF WNDCLASSEX, NULL, WinProc, NULL, NULL, NULL, NULL, NULL, COLOR_WINDOW, NULL, CTEXT("OddsCalculator")>
.CODE
WinMain proc hInst:HINSTANCE, hPrevInst:HINSTANCE, CmdLine:LPSTR, CmdShow:DWORD 
  LOCAL                     msg:MSG
  LOCAL                     hWnd:HWND

  mov                       WndClassEx.style, CS_HREDRAW or CS_VREDRAW
  m2m                       WndClassEx.hInstance, hInstance
  invoke                    LoadIcon, NULL, IDI_APPLICATION 
  mov                       WndClassEx.hIcon, eax 
  mov                       WndClassEx.hIconSm, eax 
  mov                       WndClassEx.hCursor, $invoke( LoadCursor, NULL, IDC_ARROW ) 
  invoke                    RegisterClassEx, addr WndClassEx

  mov                       eax, tableWidth
  add                       eax, 8
  mov                       ebx, tableHeight
  add                       ebx, 27

  mov                       hWnd, $invoke( CreateWindowEx, NULL, CTEXT("OddsCalculator"), CTEXT("Texas Hold'em Odds Calculator"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, eax, ebx, NULL, NULL, hInstance, NULL )
  invoke                    ShowWindow, hWnd, CmdShow
  invoke                    UpdateWindow, hWnd

  .WHILE TRUE
    .BREAK .IF (!$invoke( GetMessage, ADDR msg, NULL, 0, 0 )) 
    invoke                  TranslateMessage, ADDR msg 
    invoke                  DispatchMessage, ADDR msg 
  .ENDW 
  mov                       eax, msg.wParam
  ret
WinMain endp

GetImage PROC path:DWORD
  invoke LoadImage, NULL, path, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION + LR_DEFAULTSIZE + LR_LOADFROMFILE  
  ret
GetImage ENDP

DrawCard PROC USES ecx edx x:DWORD, y:DWORD, index:DWORD, cWidth:DWORD, cHeight:DWORD, hDC:DWORD, hImgDC:DWORD
  xor                       ecx, ecx    ; x
  xor                       edx, edx    ; y
  @@:
  cmp                       index, 13
  jc                        @F
  add                       edx, cHeight
  sub                       index, 13
  jmp                       @B
  @@:
  cmp                       index, 0
  je                        @F
  add                       ecx, cWidth
  sub                       index, 1
  jmp                       @B
  @@:

  invoke                    TransparentBlt, hDC, x, y, cWidth, cHeight, hImgDC, ecx, edx, cWidth, cHeight, 0000FF00h

  ret
DrawCard ENDP

DrawPercent PROC USES ebx ecx edx x:DWORD, y:DWORD, small:DWORD, large:DWORD, hDC:DWORD
  invoke                    SetTextColor, hDC, 00FFFFFFh
  invoke                    SetBkColor, hDC, 00020028h
  
  mov                       eax, small
  mov                       ecx, large
  xor                       edx, edx
  div                       ecx

  ; Rounding
  shr                       large, 1
  .IF edx >= large
    add                     eax, 1
  .ENDIF
  
  xor                       edx, edx
  mov                       ecx, 10
  div                       ecx
  push                      edx
  push                      eax
  invoke                    wsprintf, OFFSET scratchText, CTEXT("%d.%d%%")
  mov                       ecx, eax

  invoke                    TextOut, hDC, x, y, OFFSET scratchText, ecx
  ret
DrawPercent ENDP

isCardSelected PROC USES ecx index:DWORD
  mov                       eax, index
  mov                       ecx, 0
  @@:
    cmp                     ecx, numPlayers
    jnc                     @F

    .IF eax == playerCards[ecx*8+0] || eax == playerCards[ecx*8+4]
      mov                   eax, 1
      ret
    .ENDIF
    add                     ecx, 1
    jmp                     @B
  @@:

  .IF eax == flop1 || eax == flop2 || eax == flop3 || eax == turn
    mov                     eax, 1
    ret
  .ENDIF

  xor                       eax, eax
  ret
isCardSelected ENDP

TestTableCard MACRO left:REQ, top:REQ, card:REQ
  mov                       eax, left
  mov                       ebx, top
  .IF eax <= x && ebx <= y
    add                     eax, lCardWidth
    add                     ebx, lCardHeight
    .IF eax > x && ebx > y
      mov                   eax, card
      ret
    .ENDIF
  .ENDIF
ENDM

isMouseOverPlayerCard PROC x:DWORD, y:DWORD
  .IF numPlayers >= 2
    TestTableCard           flop1X, flop1Y, OFFSET flop1
    TestTableCard           flop2X, flop2Y, OFFSET flop2
    TestTableCard           flop3X, flop3Y, OFFSET flop3
    TestTableCard           turnX, turnY, OFFSET turn
    mov                     ecx, numPlayers
    lea                     ecx, [ecx*2-1]
    @@:
      mov                   edx, OFFSET playerCards
      lea                   edx, [edx+ecx*4]
      TestTableCard DWORD PTR playerCardPos[ecx*8+0], DWORD PTR playerCardPos[ecx*8+4], edx
      sub                   ecx, 1
    jnc                     @B
  .ENDIF
  xor                       eax, eax
  ret
isMouseOverPlayerCard ENDP

isMouseOverDeckCard PROC x:DWORD, y:DWORD
  mov                       ecx, x
  mov                       edx, y
  .IF     ecx >= SPADES_X && ecx <= SPADES_X+(SMALL_CARD_WIDTH*13+SMALL_CARD_HORIZ_SPACE*12) && edx >= SPADES_Y && edx <= SPADES_Y+(SMALL_CARD_HEIGHT+SMALL_CARD_VERT_SPACE)
    sub                     ecx, SPADES_X
    sub                     edx, SPADES_Y
    mov                     eax, 0
  .ELSEIF ecx >= HEARTS_X && ecx <= HEARTS_X+(SMALL_CARD_WIDTH*13+SMALL_CARD_HORIZ_SPACE*12) && edx >= HEARTS_Y && edx <= HEARTS_Y+(SMALL_CARD_HEIGHT+SMALL_CARD_VERT_SPACE)
    sub                     ecx, HEARTS_X
    sub                     edx, HEARTS_Y
    mov                     eax, 13
  .ELSEIF ecx >= CLUBS_X && ecx <= CLUBS_X+(SMALL_CARD_WIDTH*13+SMALL_CARD_HORIZ_SPACE*12) && edx >= CLUBS_Y && edx <= CLUBS_Y+(SMALL_CARD_HEIGHT+SMALL_CARD_VERT_SPACE)
    sub                     ecx, CLUBS_X
    sub                     edx, CLUBS_Y
    mov                     eax, 26
  .ELSEIF ecx >= DIAMONDS_X && ecx <= DIAMONDS_X+(SMALL_CARD_WIDTH*13+SMALL_CARD_HORIZ_SPACE*12) && edx >= DIAMONDS_Y && edx <= DIAMONDS_Y+(SMALL_CARD_HEIGHT+SMALL_CARD_VERT_SPACE)
    sub                     ecx, DIAMONDS_X
    sub                     edx, DIAMONDS_Y
    mov                     eax, 39
  .ELSE
    or                      eax, -1
    ret
  .ENDIF
  
  @@:
    cmp                     ecx, SMALL_CARD_WIDTH+SMALL_CARD_HORIZ_SPACE
    jc                      @F
    add                     eax, 1
    sub                     ecx, SMALL_CARD_WIDTH+SMALL_CARD_HORIZ_SPACE
  jmp                       @B
  @@:
  .IF ecx > SMALL_CARD_WIDTH
    or                      eax, -1
    ret
  .ENDIF
  mov                       ecx, eax
  .IF $invoke( isCardSelected, eax )
    or                      eax, -1
    ret
  .ENDIF
  mov                       eax, ecx
  ret
isMouseOverDeckCard ENDP

isMouseOverCard PROC USES ebx ecx edx x:DWORD, y:DWORD
  mov                       ebx, $invoke( isMouseOverPlayerCard, x, y )
  mov                       eax, $invoke( isMouseOverDeckCard, x, y )
  .If eax != -1 || ebx != 0
    mov                     eax, 1
  .ELSE
    xor                     eax, eax
  .ENDIF
  ret
isMouseOverCard ENDP

doPlayerCardClick PROC index:DWORD
  mov                       eax, index
  mov                       ecx, 0
  @@:
    cmp                     ecx, 24
    jnc                     @F
    .IF eax == DWORD PTR playerCards[ecx*4]
      mov                   DWORD PTR playerCards[ecx*4], 52
      ret
    .ENDIF
    add                     ecx, 1
    jmp                     @B
  @@:
  ret
doPlayerCardClick ENDP

doDeckCardClick PROC index:DWORD
  mov                       eax, index
  mov                       ecx, 0
  @@:
    cmp                     ecx, numPlayers
    jnc                     @F
    .IF DWORD PTR playerCards[ecx*8+0] == 52
      mov                   DWORD PTR playerCards[ecx*8+0], eax
      ret
    .ELSEIF DWORD PTR playerCards[ecx*8+4] == 52
      mov                   DWORD PTR playerCards[ecx*8+4], eax
      ret
    .ENDIF
    add                     ecx, 1
    jmp                     @B
  @@:

  .IF numPlayers >= 2
    .IF flop1 == 52
      mov                   flop1, eax
    .ELSEIF flop2 == 52
      mov                   flop2, eax
    .ELSEIF flop3 == 52
      mov                   flop3, eax
    .ELSEIF turn == 52
      mov                   turn, eax
    .ENDIF
  .ENDIF
  ret
doDeckCardClick ENDP

DrawDeck PROC hDC:DWORD
  xor                       ebx, ebx
  mov                       ecx, SPADES_X
  mov                       edx, SPADES_Y
  @@:
    invoke                  isCardSelected, ebx
    .IF !eax
      invoke                DrawCard, ecx, edx, ebx, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT, hDC, hThumbDC
    .ENDIF
    add                     ecx, SMALL_CARD_WIDTH+SMALL_CARD_HORIZ_SPACE
    add                     ebx, 1
    cmp                     ebx, 13
  jc                        @B
  mov                       ecx, HEARTS_X
  mov                       edx, HEARTS_Y
  @@:
    invoke                  isCardSelected, ebx
    .IF !eax
      invoke                DrawCard, ecx, edx, ebx, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT, hDC, hThumbDC
    .ENDIF
    add                     ecx, SMALL_CARD_WIDTH+SMALL_CARD_HORIZ_SPACE
    add                     ebx, 1
    cmp                     ebx, 26
  jc                        @B
  mov                       ecx, CLUBS_X
  mov                       edx, CLUBS_Y
  @@:
    invoke                  isCardSelected, ebx
    .IF !eax
      invoke                DrawCard, ecx, edx, ebx, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT, hDC, hThumbDC
    .ENDIF
    add                     ecx, SMALL_CARD_WIDTH+SMALL_CARD_HORIZ_SPACE
    add                     ebx, 1
    cmp                     ebx, 39
  jc                        @B
  mov                       ecx, DIAMONDS_X
  mov                       edx, DIAMONDS_Y
  @@:
    invoke                  isCardSelected, ebx
    .IF !eax
      invoke                DrawCard, ecx, edx, ebx, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT, hDC, hThumbDC
    .ENDIF
    add                     ecx, SMALL_CARD_WIDTH+SMALL_CARD_HORIZ_SPACE
    add                     ebx, 1
    cmp                     ebx, 52
  jc                        @B
  ret
DrawDeck ENDP

DrawPlayerCards PROC hDC:DWORD
  .IF numPlayers >= 2
    invoke                  DrawCard, flop1X, flop1Y, flop1, lCardWidth, lCardHeight, hDC, hCardsDC
    invoke                  DrawCard, flop2X, flop2Y, flop2, lCardWidth, lCardHeight, hDC, hCardsDC
    invoke                  DrawCard, flop3X, flop3Y, flop3, lCardWidth, lCardHeight, hDC, hCardsDC
    invoke                  DrawCard, turnX, turnY, turn, lCardWidth, lCardHeight, hDC, hCardsDC

    xor                     ecx, ecx
    .WHILE ecx < numPlayers
      mov                   ebx, ecx
      shl                   ebx, 4
        
      invoke                DrawCard, DWORD PTR playerCardPos[ebx+ 0], DWORD PTR playerCardPos[ebx+ 4], DWORD PTR playerCards[ecx*8+0], lCardWidth, lCardHeight, hDC, hCardsDC
      invoke                DrawCard, DWORD PTR playerCardPos[ebx+ 8], DWORD PTR playerCardPos[ebx+12], DWORD PTR playerCards[ecx*8+4], lCardWidth, lCardHeight, hDC, hCardsDC

      .IF calcDone
        pushad
        invoke              DrawPercent, DWORD PTR playerPrctPos[ecx*8+0], DWORD PTR playerPrctPos[ecx*8+4], playerWins[ecx*4], totalHands, hDC
        popad
      .ENDIF
      add                   ecx, 1
    .ENDW
  .ENDIF
  ret
DrawPlayerCards ENDP

LoadSkin PROC hWnd:HWND
  LOCAL hFile:HANDLE
  LOCAL pFileMap:DWORD
  LOCAL monoFont:HANDLE
  invoke                    CreateFile, CTEXT("Settings.ini"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
  .IF eax != INVALID_HANDLE_VALUE
    mov                     hFile, eax
    mov                     pFileMap, $invoke( MapFile, hFile )

    invoke                  UnmapViewOfFile, pFileMap
    invoke                  CloseHandle, hFile
  .ELSE
    GetFont                 monoFont, 14, CTEXT("Tahoma")
    AddComboBox             hNumPlayers, WS_VISIBLE, 86, 354, 177, IDC_NUMPLAYERS
    AddButton               hGo, CTEXT("Reset All"), WS_VISIBLE, 268, 352, 81, 24, IDC_RESET, monoFont
    AddButton               hGo, CTEXT("Calculate Odds"), WS_VISIBLE, 353, 352, 138, 24, IDC_GOBTN, monoFont

    AddComboItem            hNumPlayers, CTEXT("Select Number of Players"), 0
    AddComboItem            hNumPlayers, CTEXT("Two Players"), 2
    AddComboItem            hNumPlayers, CTEXT("Three Players"), 3
    AddComboItem            hNumPlayers, CTEXT("Four Players"), 4
    AddComboItem            hNumPlayers, CTEXT("Five Players"), 5
    invoke                  SendMessage, hNumPlayers, CB_SETCURSEL, 0, 0

    ; Load the table image
    mov                     hTableImg, $invoke( LoadBitmap, hInstance, IDB_TABLE )
    mov                     ebx, $invoke( GetDC, hWnd )
    mov                     hTableDC, $invoke( CreateCompatibleDC, ebx )
    invoke                  ReleaseDC, hWnd, ebx
    invoke                  SelectObject, hTableDC, hTableImg
    invoke                  DeleteObject, hTableImg

    ; Load the card images
    mov                     hCardsImg, $invoke( LoadBitmap, hInstance, IDB_LARGE )
    mov                     ebx, $invoke( GetDC, hWnd )
    mov                     hCardsDC, $invoke( CreateCompatibleDC, ebx )
    invoke                  ReleaseDC, hWnd, ebx
    invoke                  SelectObject, hCardsDC, hCardsImg
    invoke                  DeleteObject, hCardsImg

    ; Load the card thumbnails
    mov                     hThumbImg, $invoke( LoadBitmap, hInstance, IDB_SMALL )
    mov                     ebx, $invoke( GetDC, hWnd )
    mov                     hThumbDC, $invoke( CreateCompatibleDC, ebx )
    invoke                  ReleaseDC, hWnd, ebx
    invoke                  SelectObject, hThumbDC, hThumbImg
    invoke                  DeleteObject, hThumbImg
  .ENDIF
  ret
LoadSkin ENDP

WinProc proc hWnd:HWND, uMsg:UINT, wParam:WPARAM, lParam:LPARAM
  LOCAL paintStruct:PAINTSTRUCT
  LOCAL text[100]:BYTE
  LOCAL hTemp:HWND
  LOCAL hDC:HDC

  .IF uMsg == WM_CREATE
    invoke                  LoadSkin, hWnd
    mov                     hContextMenu, $invoke( LoadMenu, hInstance, CTEXT("ContextMenu") )
    
  .ELSEIF uMsg == WM_COMMAND
    mov                     eax, wParam
    .switch( ax )
      .case IDC_GOBTN
        invoke              GetWindowText, hCards, ADDR text, 999
        invoke              evaluateHand, ADDR text
        mov                 calcDone, TRUE
        invoke              InvalidateRect, hWnd, NULL, FALSE
        .break
      .case IDC_RESET
        mov                 ecx, 0
        @@:
          mov               DWORD PTR playerCards[ecx*4], 52
          add               ecx, 1
          cmp               ecx, 24
        jc                  @B
        mov                 numPlayers, 0
        invoke              SendMessage, hNumPlayers, CB_SETCURSEL, 0, 0
        invoke              InvalidateRect, hWnd, NULL, FALSE
        mov                 calcDone, FALSE
        .break
      .case IDC_NUMPLAYERS
        shr                 eax, 16
        .IF ax == CBN_SELCHANGE
          GetComboData      hNumPlayers
          mov               numPlayers, eax
          @@:
            cmp             eax, 10
            jnc             @F
            mov             DWORD PTR playerCards[eax*8+0], 52
            mov             DWORD PTR playerCards[eax*8+4], 52
            add             eax, 1
            jmp             @B
          @@:
          mov               flop1, 52
          mov               flop2, 52
          mov               flop3, 52
          mov               turn, 52
          invoke            InvalidateRect, hWnd, NULL, FALSE
          mov               calcDone, FALSE
        .ENDIF
        .break
    .endswitch

  .ELSEIF uMsg == WM_MOUSEMOVE
    mov                     eax, lParam
    mov                     ebx, eax
    and                     eax, 0000FFFFh  ; x
    shr                     ebx, 16         ; y
    .IF $invoke( isMouseOverCard, eax, ebx )
      invoke                LoadCursor, NULL, IDC_HAND
    .ELSE
      invoke                LoadCursor, NULL, IDC_ARROW
    .ENDIF
    invoke                  SetCursor, eax

  .ELSEIF uMsg == WM_CONTEXTMENU
    mov                     eax, lParam
    mov                     ebx, eax
    and                     eax, 0000FFFFh
    shr                     ebx, 16
    invoke                  TrackPopupMenu, hContextMenu, TPM_LEFTALIGN, eax, ebx, 0, hWnd, 0

  .ELSEIF uMsg == WM_LBUTTONDOWN
    invoke                  isMouseOverDeckCard, ecx, edx
    .IF eax != -1
      mov                   buttonDown, TRUE
      invoke                LoadCursor, NULL, IDC_HAND
      invoke                SetCursor, eax
    .ENDIF

  .ELSEIF uMsg == WM_LBUTTONUP
    mov                     ecx, lParam
    mov                     edx, ecx
    and                     ecx, 0000FFFFh  ; x
    shr                     edx, 16         ; y
    .IF isDragging
      invoke                isMouseOverPlayerCard, ecx, edx
      .IF eax != -1
      .ENDIF
    .ELSE
      invoke                isMouseOverDeckCard, ecx, edx
      .IF eax != -1
        invoke              doDeckCardClick, eax
        invoke              InvalidateRect, hWnd, NULL, FALSE
        mov                 calcDone, FALSE
        ret
      .ENDIF
      invoke                isMouseOverPlayerCard, ecx, edx
      .IF eax != 0
        mov                 DWORD PTR [eax], 52
        invoke              InvalidateRect, hWnd, NULL, FALSE
        mov                 calcDone, FALSE
        ret
      .ENDIF
    .ENDIF
    mov                     buttonDown, FALSE

  .ELSEIF uMsg == WM_PAINT
    mov                     hDC, $invoke( BeginPaint, hWnd, ADDR paintStruct )

    ; Draw the table background
    invoke                  BitBlt, hDC, 0, 0, tableWidth, tableHeight, hTableDC, 0, 0, SRCCOPY

    ; Draw the player's cards
    invoke                  DrawPlayerCards, hDC

    ; Draw the deck
    invoke                  DrawDeck, hDC
    invoke                  EndPaint, hWnd, ADDR paintStruct

  .ELSEIF uMsg == WM_DESTROY
    invoke                  ReleaseDC, hWnd, hTableDC
    invoke                  ReleaseDC, hWnd, hCardsDC
    invoke                  ReleaseDC, hWnd, hThumbDC
    invoke                  PostQuitMessage, NULL
  .ELSE
    invoke                  DefWindowProc, hWnd, uMsg, wParam, lParam
    ret
  .ENDIF
  xor                       eax, eax
  ret
WinProc endp

end start