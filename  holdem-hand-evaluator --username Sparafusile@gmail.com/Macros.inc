; Copies the contents of src to dest
m2m MACRO dest:REQ, src:REQ
  push  src
  pop   dest
ENDM

CTEXT MACRO y:VARARG
  LOCAL sym, dummy
  dummy EQU $	;; MASM error fix
  CONST SEGMENT
    IFIDNI <y>,<>
      sym db 0
    ELSE
      sym db y,0
    ENDIF
  CONST ends
  EXITM <OFFSET sym>
ENDM

$invoke MACRO Function:REQ, Args:VARARG
  IFB <Args>
    invoke  Function
  ELSE
    invoke  Function, Args
  ENDIF
  EXITM <eax>
ENDM

GetFont MACRO Variable, Size:=<14>, Face:=<CTEXT("Courier New")>
  invoke  CreateFont,Size,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH+FF_DONTCARE,Face
  mov     Variable, eax
ENDM

malloc MACRO Size:REQ
  IFDIF <Size>, <0>
    EXITM <$invoke( HeapAlloc, $invoke( GetProcessHeap ), HEAP_ZERO_MEMORY, Size )>
  ENDIF
  EXITM <0>
ENDM

mfree MACRO pMem:REQ
  invoke    HeapFree, $invoke( GetProcessHeap ), 0, pMem
ENDM

StrDecToBin MACRO accum:REQ, temp:REQ, src:REQ
  movzx     temp, BYTE PTR [src]
  xor       accum, accum
  sub       temp, '0'
  @@:
    imul    accum, 10
    add     accum, temp
    add     src, 1
    movzx   temp, BYTE PTR [src]
    sub     temp, '0'
    cmp     temp, 10
    jc      @B
  EXITM <accum>
ENDM

StrHexToBin MACRO accum:REQ, temp:REQ, src:REQ
  movzx   temp, BYTE PTR [src]
  xor     accum, accum
  sub     temp, '0'
  cmp     temp, 10
  jl      @F
  sub     temp, 7
  @@:
    shl   accum, 4
    or    accum, temp
    add   src, 1
    movzx temp, BYTE PTR [src]
    sub   temp, '0'
    cmp   temp, 10
    jc    @B
    sub   temp, 7
    cmp   temp, 16
    jc    @B
  EXITM <accum>
ENDM

; The ecx, esi and edi register are preserved only if they
; are NOT passed as parameters.  Otherwise, ecx is zero,
; esi is pointing the the next source character, and edi
; is pointing to the NULL at the end of the destination
; buffer.
StrCopyLen MACRO src:REQ, dst:REQ, len:REQ
  IFDIF <src>, <esi>
    push    esi
    mov     esi, src
  ENDIF
  IFDIF <dst>, <edi>
    push    edi
    mov     edi, dst
  ENDIF
  IFDIF <len>, <ecx>
    push    ecx
    mov     ecx, len
  ENDIF

  rep       movsb
  mov       BYTE PTR [edi], 0

  IFDIF <len>, <ecx>
    pop     ecx
  ENDIF
  IFDIF <dst>, <edi>
    pop     edi
  ENDIF
  IFDIF <src>, <esi>
    pop     esi
  ENDIF
ENDM

strcmp MACRO str1:REQ, str2:REQ
  push      ebx
  push      ecx
  push      edx

  mov       ecx, str1
  mov       edx, str2
  @@:
    movzx   eax, BYTE PTR [ecx]
    lea     ecx, [ecx+1]
    movzx   ebx, BYTE PTR [edx]
    lea     edx, [edx+1]
    or      ebx, ebx
    jz      @F
    sub     eax, ebx
    jz      @B
  @@:

  pop       edx
  pop       ecx
  pop       ebx
ENDM

strlen MACRO pStr:REQ
  push      edi
  push      ecx
  IFDIF <pStr>, <edi>
    mov     edi, pStr
  ENDIF
  or        ecx, -1
  xor       eax, eax
  repne scasb
  lea       eax, [edi+(-1)]
  sub       eax, pStr
  pop       ecx
  pop       edi
  EXITM <eax>
ENDM

CheckCommonDialogError MACRO
  call   CommDlgExtendedError
  .IF eax == CDERR_DIALOGFAILURE
    mov eax, CTEXT("CDERR_DIALOGFAILURE: The dialog box could not be created.")
  .ELSEIF eax == CDERR_FINDRESFAILURE
    mov eax, CTEXT("CDERR_FINDRESFAILURE: Failed to find a specified resource.")
  .ELSEIF eax == CDERR_NOHINSTANCE
    mov eax, CTEXT("CDERR_NOHINSTANCE: No instance handle for template.")
  .ELSEIF eax == CDERR_INITIALIZATION
    mov eax, CTEXT("CDERR_INITIALIZATION: Dialog box initialization failed (out of memory?).")
  .ELSEIF eax == CDERR_NOHOOK
    mov eax, CTEXT("CDERR_NOHOOK: You failed to provide a pointer to a corresponding hook procedure.")
  .ELSEIF eax == CDERR_LOCKRESFAILURE
    mov eax, CTEXT("CDERR_LOCKRESFAILURE: The common dialog box function failed to lock a specified resource.")
  .ELSEIF eax == CDERR_NOTEMPLATE
    mov eax, CTEXT("CDERR_NOTEMPLATE: You failed to provide a corresponding template.")
  .ELSEIF eax == CDERR_LOADRESFAILURE
    mov eax, CTEXT("CDERR_LOADRESFAILURE: The common dialog box function failed to load a specified resource.")
  .ELSEIF eax == CDERR_STRUCTSIZE
    mov eax, CTEXT("CDERR_STRUCTSIZE: lStructSize member of the initialization structure is invalid.")
  .ELSEIF eax == CDERR_LOADSTRFAILURE
    mov eax, CTEXT("CDERR_LOADSTRFAILURE: The common dialog box function failed to load a specified string.")
  .ELSEIF eax == FNERR_BUFFERTOOSMALL
    mov eax, CTEXT("FNERR_BUFFERTOOSMALL: The buffer pointed to by the lpstrFile member of the OPENFILENAME structure is too small for the specified file name.")
  .ELSEIF eax == CDERR_MEMALLOCFAILURE
    mov eax, CTEXT("CDERR_MEMALLOCFAILURE: The common dialog box function was unable to allocate memory for internal structures.")
  .ELSEIF eax == FNERR_INVALIDFILENAME
    mov eax, CTEXT("FNERR_INVALIDFILENAME: A file name is invalid.")
  .ELSEIF eax == CDERR_MEMLOCKFAILURE
    mov eax, CTEXT("CDERR_MEMLOCKFAILURE: The common dialog box function was unable to lock the memory associated with a handle.")
  .ELSEIF eax == FNERR_SUBCLASSFAILURE
    mov eax, CTEXT("FNERR_SUBCLASSFAILURE: An attempt to subclass a list box failed because sufficient memory was not available.")
  .ENDIF
  .IF eax != 0
    invoke MessageBox, NULL, eax, CTEXT("Error"), MB_OK
  .ENDIF
  xor eax, eax
  ret
ENDM








;this macros are for switch/case support (similar to C) 

; mov eax,uMsg 
; .switch eax 
; .case WM_COMMAND 
;   (...) 
;   .break 
; .case WM_NOTIFY 
;   (...) 
;   .break 
; .default 
;   (...) 
; .endswitch 

option dotname

; .switch {register or variable} 
.switch macro regname:req 
    ifndef SwitchNesting 
        SwitchNesting = 1 
    else 
        SwitchNesting = SwitchNesting + 1 
    endif 
    @CatStr ( <SwitchRegister_>, %SwitchNesting ) textequ <&regname&> 
    @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) = 1 
    @CatStr ( <SwitchLastCase_>, %SwitchNesting ) = 0 
    @CatStr ( <SwitchNeedDefault_>, %SwitchNesting ) = 0 
    .repeat 
endm 

; .case {value 1}, [value 2], (...), [value n] 
.case macro var:vararg 
    local value,isfirst,cond 

    ifndef SwitchNesting 
        .err <.case cannot be outside of a .switch/.endswitch block.> 
        exitm 
    endif 
    if SwitchNesting lt 1 
        .err <Bad .switch/.endswitch block.> 
        exitm 
    endif 
    ife @CatStr ( <SwitchLastCase_>, %SwitchNesting ) 
        ife @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) 
            .endif 
        else 
            @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) = 0 
        endif 
        isfirst = 1 
        for value, <var> 
            if isfirst ne 0 
                cond catstr <( SwitchRegister_>, %SwitchNesting, < == &value& )> 
                isfirst = 0 
            else 
                cond textequ @CatStr ( %cond, < || ( SwitchRegister_>, %SwitchNesting, < == >, <&value&>, < )> ) 
            endif 
        endm 
        if isfirst eq 0 
            .if cond 
        endif 
    else 
        .err <.case cannot follow .default.> 
    endif 
endm 

; .between {min 1}, {max 1}, [min 2], [max 2], (...), [min n], [max n] 
; values must always be given in pairs 
.between macro var:vararg 
    local value,isfirst,iseven,cond,cond2 

    ifndef SwitchNesting 
        .err <.between cannot be outside of a .switch/.endswitch block.> 
        exitm 
    endif 
    if SwitchNesting lt 1 
        .err <Bad .switch/.endswitch block.> 
        exitm 
    endif 
    ife @CatStr ( <SwitchLastCase_>, %SwitchNesting ) 
        ife @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) 
            .endif 
        else 
            @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) = 0 
        endif 
        isfirst = 1 
        iseven = 1 
        for value, <var> 
            if isfirst ne 0 
                ;( ( reg >= value ) 
                cond catstr <( ( SwitchRegister_>, %SwitchNesting, < !>= &value& )> 
                isfirst = 0 
            else 
                if iseven eq 1 
                    ; || ( ( reg >= value ) 
                    cond2 catstr <cond>, < || ( ( SwitchRegister_>, %SwitchNesting, < !>= &value& )> 
                    cond textequ cond2 
                else 
                    ; && ( reg <= value ) ) 
                    cond2 catstr <cond>, < && ( SwitchRegister_>, %SwitchNesting, < !<= &value& ) )> 
                    cond textequ cond2 
                endif 
            endif 
            iseven = -iseven 
        endm 
        if isfirst eq 0 
            .if cond 
        endif 
    else 
        .err <.between cannot follow .default.> 
    endif 
endm 

; .caseif {condition} 
; The condition must be a text literal to be passed to the .if macro. 
; Remember to escape angle brackets with an exclamation point. 
.caseif macro cond:req 
    ifndef SwitchNesting 
        .err <.caseif cannot be outside of a .switch/.endswitch block.> 
        exitm 
    endif 
    if SwitchNesting lt 1 
        .err <Bad .switch/.endswitch block.> 
        exitm 
    endif 
    ife @CatStr ( <SwitchLastCase_>, %SwitchNesting ) 
        ife @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) 
            .endif 
        else 
            @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) = 0 
        endif 
        .if cond 
    else 
        .err <.caseif cannot follow .default.> 
    endif 
endm 

.default macro 
    ifndef SwitchNesting 
        .err <.default cannot be outside of a .switch/.endswitch block.> 
        exitm 
    endif 
    if SwitchNesting lt 1 
        .err <Bad .switch/.endswitch block.> 
        exitm 
    endif 
    ifndef SwitchCount 
        SwitchCount = 0 
    endif 
    ife @CatStr ( <SwitchLastCase_>, %SwitchNesting ) 
        ife @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) 
            .endif 
        else 
            .err <There should be at least one .case before .default.> 
        endif 
        @CatStr ( <SwitchLastCase_>, %SwitchNesting ) = 1 
    else 
        .err <There cannot be more that one .default in a .switch/.endswitch block.> 
    endif 
    @CatStr ( <SwitchNeedDefault_>, %SwitchNesting ) = 0 
    @CatStr ( <SwitchLastCase_>, %SwitchNesting ) = 1 
    @CatStr ( <SwitchDefault_>, %SwitchCount, <:> ) 
    SwitchCount = SwitchCount + 1 
endm 

.breakdef macro 
    ifndef SwitchNesting 
        .err <.breakdef cannot be outside of a .switch/.endswitch block.> 
        exitm 
    endif 
    if SwitchNesting lt 1 
        .err <Bad .switch/.endswitch block.> 
        exitm 
    endif 
    ifndef SwitchCount 
        SwitchCount = 0 
    endif 
    @CatStr ( <SwitchNeedDefault_>, %SwitchNesting ) = 1 
    jmp @CatStr ( <SwitchDefault_>, %SwitchCount ) 
endm 

.endswitch macro 
    if SwitchNesting lt 1 
        .err <Bad .switch/.endswitch block.> 
        exitm 
    endif 
    ife @CatStr ( <SwitchFirstCase_>, %SwitchNesting ) 
        ife @CatStr ( <SwitchLastCase_>, %SwitchNesting ) 
            .endif 
        endif 
    else 
        .err <Empty .switch/.endswitch block?> 
    endif 
    if @CatStr ( <SwitchNeedDefault_>, %SwitchNesting ) ne 0 
        .err <Unmatched .breakdef, could branch to unexpected location!> 
    endif 
    .until TRUE 
    SwitchNesting = SwitchNesting - 1 
    if SwitchNesting lt 0 
        .err <Bad .switch/.endswitch block.> 
    endif 
endm