include    \masm32\include\Gdi32.inc
includelib \masm32\lib\Gdi32.lib
include    \masm32\include\comctl32.inc 
includelib \masm32\lib\comctl32.lib 

.DATA
  EditClass             db "Edit",0
  LabelClass            db "Label",0
  StaticClass           db "Static",0
  ButtonClass           db "Button",0
  ComboClass            db "Combobox",0
  ScrollbarClass        db "Scrollbar",0
  ListClass             db "SysListView32",0
  TreeClass             db "SysTreeView32",0
  TabClass              db "SysTabControl32",0
  ProgressClass         db "msctls_progress32",0

.DATA?
  defStaticProc         DWORD ?

.CONST
  WM_SETTEXTCOLOR       EQU WM_USER

; Adds a component to a window (this requires that there be a hWnd and hInstance variable within scope)
AddComponent MACRO h:REQ, ExStyle:REQ, Class:REQ, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ, id:REQ, Font:=<monoFont>
  invoke CreateWindowEx, ExStyle, Class, Text, Style, x, y, width, height, hWnd, id, hInstance, NULL
  IFDIF <h>,<NULL>
    mov h, eax
  ENDIF
  IFDIF <Font>,<NULL>
    invoke SendMessage, eax, WM_SETFONT, Font, TRUE
  ENDIF
ENDM

AddGroupBox MACRO h, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ, Font
  AddComponent h, NULL, ADDR ButtonClass, Text, WS_CHILD+BS_GROUPBOX+Style, x, y, width, height, 0, Font
ENDM

AddLabel MACRO h:REQ, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, Font:=<monoFont>
  AddComponent h, NULL, ADDR LabelClass, Text, WS_CHILD+Style, x, y, width, 21, NULL, Font
ENDM

AddTextField MACRO h:REQ, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, id:REQ
  AddComponent h, WS_EX_CLIENTEDGE, ADDR EditClass, Text, WS_CHILD+ES_AUTOHSCROLL+Style, x, y, width, 20, id
ENDM

; WS_HSCROLL, WS_VSCROLL, WS_MAXIMIZE
AddTextArea MACRO h:REQ, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ
  AddComponent h, WS_EX_CLIENTEDGE, ADDR EditClass, Text, WS_CHILD+ES_AUTOHSCROLL+ES_AUTOVSCROLL+ES_MULTILINE+Style, x, y, width, height, 0
ENDM

AppendText MACRO h:REQ, pBuffer:REQ, pText:REQ, scroll:=<TRUE>
  invoke lstrcat, pBuffer, pText
  invoke SetWindowText, h, pBuffer
  IFIDN <scroll>, <TRUE>
    ScrollToBottom h
  ENDIF
ENDM

ScrollToBottom MACRO hTextArea:REQ
  invoke SendMessage, hTextArea, EM_SCROLL, SB_PAGEDOWN, 0
  .WHILE ax > 0
    invoke SendMessage, hTextArea, EM_SCROLL, SB_PAGEDOWN, 0
  .ENDW
ENDM

AddButton MACRO h:REQ, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ, id:REQ, Font
  AddComponent h, NULL, ADDR ButtonClass, Text, WS_CHILD+BS_PUSHBUTTON+Style, x, y, width, height, id, Font
ENDM

AddCheckBox MACRO h:REQ, Text:REQ, Style, x:REQ, y:REQ, width:REQ, id:REQ, Font
  AddComponent h, NULL, ADDR ButtonClass, Text, WS_CHILD+BS_AUTOCHECKBOX+Style, x, y, width, 15, id, Font
ENDM

SetCheckBox MACRO hCheck:REQ
  invoke  SendMessage, hCheck, BM_SETCHECK, BST_CHECKED, 0
ENDM

ClearCheckBox MACRO hCheck:REQ
  invoke  SendMessage, hCheck, BM_SETCHECK, BST_UNCHECKED, 0
ENDM

RotateCheckBox MACRO hCheck:REQ
  invoke    SendMessage, hCheck, BM_GETCHECK, 0, 0
  .IF eax == BST_CHECKED
    invoke  SendMessage, hCheck, BM_SETCHECK, BST_UNCHECKED, 0
  .ELSE
    invoke  SendMessage, hCheck, BM_SETCHECK, BST_CHECKED, 0
  .ENDIF
ENDM

AddRadioButton MACRO h:REQ, Text:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, id:REQ
  AddComponent h, NULL, ADDR ButtonClass, Text, WS_CHILD+BS_AUTORADIOBUTTON+Style, x, y, width, 21, id
ENDM

AddScrollbar MACRO h:REQ, Style:REQ, x:REQ, y:REQ, width:=<17>, height:=<16>, id:=<0>
  invoke CreateWindowEx, 0, OFFSET ScrollbarClass, NULL, WS_CHILD+Style, x, y, width, height, hWnd, NULL, hInstance, NULL
  IFDIF <h>,<NULL>
    mov h, eax
  ENDIF
ENDM

AddComboBox MACRO h:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, id:REQ
  AddComponent h, WS_EX_CLIENTEDGE, ADDR ComboClass, NULL, WS_CHILD+WS_VSCROLL+CBS_DROPDOWNLIST+CBS_HASSTRINGS+Style, x, y, width, 150, id
ENDM

GetComboData MACRO h:REQ
  invoke    SendMessage, h, CB_GETCURSEL, 0, 0
  invoke    SendMessage, h, CB_GETITEMDATA, eax, 0
ENDM

GetComboString MACRO h:REQ
  LOCAL string
  .DATA?
    string BYTE 64 DUP(?)
  .CODE
  invoke    SendMessage, h, CB_GETCURSEL, 0, 0
  invoke    SendMessage, h, CB_GETLBTEXT, eax, OFFSET string
  mov       eax, OFFSET string
ENDM

SetComboSelect MACRO h:REQ, pString:REQ
  invoke    SendMessage, h, CB_FINDSTRINGEXACT, -1, pString
  invoke    SendMessage, h, CB_SETCURSEL, eax, 0
ENDM

AddComboItem MACRO hCombo:REQ, string:REQ, data:REQ
  invoke    SendMessage, hCombo, CB_ADDSTRING, 0, string
  invoke    SendMessage, hCombo, CB_SETITEMDATA, eax, data
ENDM

RemoveComboItems MACRO hCombo:REQ
  invoke    SendMessage, hCombo, CB_RESETCONTENT, 0, 0
ENDM

AddProgressBar MACRO h:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ
  AddComponent h, WS_EX_CLIENTEDGE, ADDR ProgressClass, NULL, WS_CHILD+Style, x, y, width, height, 0
ENDM

SetProgressRange MACRO h:REQ, min:REQ, max:REQ, step
  invoke SendMessage, h, PBM_SETRANGE32, min, max
  IFDIF <step>,<>
    invoke SendMessage, h, PBM_SETSTEP, step, 0
  ENDIF
ENDM

AdvanceProgress MACRO h:REQ, step:REQ
  IFDIF <step>,<TRUE>
    invoke SendMessage, h, PBM_DELTAPOS, step, 0
  ELSE
    invoke SendMessage, h, PBM_STEPIT, 0, 0
  ENDIF
ENDM

SetProgressPos MACRO h:REQ, pos:REQ
  invoke SendMessage, h, PBM_SETPOS, pos, 0
ENDM

GetProgressPos MACRO h:REQ
  invoke SendMessage, h, PBM_GETPOS, 0, 0
ENDM

AddList MACRO h:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ, id:REQ
  LOCAL Column
  AddComponent h, WS_EX_CLIENTEDGE, ADDR ListClass, NULL, WS_CHILD+LVS_REPORT+LVS_NOCOLUMNHEADER+Style, x, y, width, height, id
  .DATA
    Column LVCOLUMN<LVCF_WIDTH,0,0,0,0,0>
  .CODE
  mov   esi, OFFSET Column
  add   esi, 8
  m2m   [esi], width - 20
  invoke SendMessage, h, LVM_INSERTCOLUMN, 0, OFFSET Column
ENDM

AddListItem MACRO hList:REQ, string:REQ, data:=<0>
  LOCAL Item
  .DATA
    Item LVITEM<LVIF_PARAM+LVIF_TEXT,250,0,0,0,NULL,255,0,0,0>
  .CODE
  m2m   Item.pszText, string
  m2m   Item.lParam, data
  invoke    SendMessage, hList, LVM_INSERTITEM, 0, OFFSET Item
ENDM

GetListData MACRO hList:REQ
  LOCAL Item
  .DATA
    Item LVITEM<LVIF_PARAM+LVIF_TEXT,0,0,0,0,NULL,255,0,0,0>
  .CODE
  invoke    SendMessage, hList, LVM_GETNEXTITEM, -1, LVNI_SELECTED
  mov       Item.iItem, eax
  or        Item.imask, LVIF_PARAM
  invoke    SendMessage, hList, LVM_GETITEM, 0, ADDR Item
  mov       eax, Item.lParam
ENDM

RemoveListItems MACRO hList:REQ
  invoke SendMessage, hList, LVM_DELETEALLITEMS, 0, 0
ENDM

AddTabControl MACRO h:REQ, Style:REQ, x:REQ, y:REQ, width:REQ, height:REQ, id:REQ
  AddComponent h, NULL, ADDR TabClass, NULL, WS_CHILD+Style, x, y, width, height, id
ENDM

AddControlTab MACRO hTabCtrl:REQ, Text:REQ, Index:REQ
  LOCAL string
  LOCAL Tab
  .DATA
    string BYTE Text,0
    Tab TC_ITEM <TCIF_TEXT,0,0,OFFSET string,0,0,0>
  .CODE
  invoke SendMessage, hTabCtrl, TCM_INSERTITEM, Index, OFFSET Tab
ENDM

GetFont MACRO Variable, Size:=<14>, Face:=<OFFSET monoFontFace>
  invoke  CreateFont,Size,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH+FF_DONTCARE,Face
  mov     Variable, eax
ENDM

.CODE
LabelProc proc hLbl:HWND, uMsg:UINT, wParam:WPARAM, lParam:LPARAM
  LOCAL paintStruct:PAINTSTRUCT
  LOCAL monoFont:DWORD
  LOCAL text[MAX_PATH]:BYTE
  LOCAL rect:RECT
  
  .switch uMsg
    .case WM_CREATE
      invoke    SetWindowLong, hLbl, GWL_USERDATA, 00000000h
      invoke    CallWindowProc, defStaticProc, hLbl, uMsg, wParam, lParam
      ret
      
    .case WM_PAINT
      invoke    GetWindowRect, hLbl, ADDR rect      
      mov       ebx, $invoke( BeginPaint, hLbl, ADDR paintStruct )

      invoke    SelectObject, ebx, $invoke( SendMessage, hLbl, WM_GETFONT, 0, 0 )
      invoke    SetTextColor, ebx, $invoke( GetWindowLong, hLbl, GWL_USERDATA )
      invoke    SetBkColor, ebx, $invoke( GetSysColor, COLOR_3DFACE )      
      invoke    GetWindowText, hLbl, ADDR text, MAX_PATH
      invoke    TextOut, ebx, 0, 0, ADDR text, $invoke( lstrlen, ADDR text )

      invoke    EndPaint, hLbl, ADDR paintStruct
      xor       eax, eax
      ret
      
    .case WM_SETTEXTCOLOR
      invoke    SetWindowLong, hLbl, GWL_USERDATA, lParam
      invoke    RedrawWindow, hLbl, NULL, NULL, RDW_INVALIDATE+RDW_UPDATENOW
      xor       eax, eax
      ret
      
    .default
      invoke    CallWindowProc, defStaticProc, hLbl, uMsg, wParam, lParam
      ret
  .endswitch
LabelProc endp

CreateLabelClass proc hInst:DWORD
  LOCAL lblClass:WNDCLASSEX

  mov       lblClass.cbSize, SIZEOF WNDCLASSEX 
  invoke    GetClassInfoEx, NULL, OFFSET StaticClass, ADDR lblClass
  .IF eax == 0
    ;call    ErrorHandler
    ret
  .ENDIF
  m2m       defStaticProc, lblClass.lpfnWndProc
  mov       lblClass.lpfnWndProc, OFFSET LabelProc
  m2m       lblClass.hInstance, hInst
  mov       lblClass.lpszClassName, OFFSET LabelClass
  add       lblClass.cbWndExtra, 4
  invoke    RegisterClassEx, ADDR lblClass
  .IF eax == 0
    ;call    ErrorHandler
    ret
  .ENDIF
  
  ret
CreateLabelClass endp