#!/bin/bash

f=main.h

echo "#define hel1 \"USAGE\n\"" > ${f}
echo -n "#define hel2 \" [filepath [line_termination: rn/r/n]]\
\n      --remove-config      Remove configuration files.\
\nINPUT\
\nthis help: q(uit),up/down,mouse/touch V.scroll\
\nMovement:\
\n    [Ctrl/Alt/Shift +]arrows/home/end/del,page up,page down,backspace,enter\
\n    p.s.: Ctrl+ left/right/del breaks at white-spaces and (),[]{}\
\n    mouse/touch Click and V.scroll" >> ${f}

text="\ncommand mode: left,right,home,end,Ctrl+q\
\nCtrl+v = visual mode; Alt+v = visual line mode\
\n    c = copy\
\n    d = delete\
\n    x = cut\
\n    i = indent (I = flow indent)\
\n    u = unindent (U = flow unindent)\
\nCtrl+o = paste; Alt+O = paste at the beginning of the row\
\nCtrl+s = save file; Alt+s = save file as...\
\nCtrl+g = go to row[,column]; Alt+g = \\\"current_row,\\\" is entered\
\nCtrl+f = find text; Alt+f = refind text; Ctrl+c = word at cursor (alphanumerics and _); Alt+c = word from cursor\
\n    if found\
\n      Enter       = next\
\n      Space       = previous\
\n      Left Arrow  = [(next/prev)&] replace\
\n      Right Arrow = total\
\n      r           = reset replace text\
\n      R           = modify replace text\
\n    c = cancel\
\n    other key to return\
\nCtrl+u = undo; Alt+u = undo mode: left=undo,right=redo,other key to return\
\nCtrl+r = redo\
\nCtrl+h = titles (movement, Enter at done, Ctrl+q, other keys to search)\
\nCtrl+w = text wrapping (movement. another key to return)\
\nCtrl+e = enable/disable internal mouse/touch\
\nCtrl+n = disable/enable indentation\
\nCtrl+t = enable/disable insensitive search\
\nCtrl+a = enable/disable O language syntax; Alt+a = syntax rescan; Alt+A = change extension name (blank is all)\
\nCtrl+j = enable/disable OA split syntax; Alt+j = change delimiter; Alt+J = change view delimiter\
\n    Alt+p = change splits folder; Alt+P = change extension name for splits (blank is all)\
\nCtrl+q = quit\""

#QWERTyUiOp - p alts are taken
#ASdFGHJkl
# zxCVbNm
# ^M is 13 that comes also at Enter, ^I is 9 that comes also at Tab
# ^P at docker, something is not ok with the redraw

#typedef struct{
#char e;
#unsigned short*z;}a;
#unsigned short a_qw[]={1,2};
#a qw={'a',a_qw};//straight {1,2} will not work

echo "${text}" >> ${f}
