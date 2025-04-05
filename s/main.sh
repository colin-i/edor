#!/bin/bash

f=main.h
wr_n() {
	echo "$@" >> ${f}
}
wr () {
	echo -n "$@" >> ${f}
}
wr2_n() {
	buf="${buf}$@
"
}
wr2 () {
	buf="${buf}$@"
}

echo "#define hel1 \"USAGE\n\""  > ${f}
d2="#define hel2 \" [filepath [line_termination: rn/r/n]]\
\n      --remove-config      Remove configuration files.\
\nINPUT\
\nthis help: q(uit),up/down,mouse/touch V.scroll\
\nMovement:\
\n    [Ctrl/Alt/Shift +]arrows/home/end/del,page up,page down,backspace,enter\
\n    p.s.: Ctrl+ left/right/del breaks at white-spaces and (),[]{}\
\n    mouse/touch Click and V.scroll"
wr "${d2}"
d2s="$(echo -n "${d2}" | sed "s/\\\n/n/g" | wc -c)" #more at second sed of this kind, if here was \\\" was another command
fix_s="$(echo ${d2s}+5 | bc)"

text="\ncommand mode: left,right,home,end,Ctrl+q\
\nCtrl+v = visual mode; Alt+v = visual line mode\
\n    c = copy\
\n    d = delete\
\n    x = cut\
\n    i = indent (I = flow indent)\
\n    u = unindent (U = flow unindent)\
\nCtrl+o = paste; Alt+o = paste at the beginning of the row\
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
wr_n "${text}"


#QWERTyUiOp - p alts are taken
#ASdFGHJkl
# zxCVbNm
# ^M is 13 that comes also at Enter, ^I is 9 that comes also at Tab
# ^P at docker, something is not ok with the redraw

wr2_n "typedef struct{
	unsigned short*pos;
	unsigned short upos;
}keys_struct;"

textsed="$(echo "${text}" | sed "s/\\\n/n/g; s/\\\\\"/\"/g")"  # replace \n to n and \\\" to \"(this will go " at grep). \ at endings are 0
search_pos () {
	wr2 ${3}
	txt=$(printf "$1+\\$(printf %o $2)")
	p=$(echo "${textsed}" | grep -b -o ${txt} | cut -d':' -f1 )
	a=
	for nr in ${p}; do
		nr=$(echo ${fix_s}+${nr}$(if [ -n "$4" ]; then echo +$4; fi) | bc)
		wr2 "${a}${nr}"
		if [ -z "${a}" ]; then a=","; fi
	done
}

_find_pos () { #name=$1 letter=$2 ctrls=$3 alt=$4 bigalt=$5
	wr2 "{(unsigned short[]){"
	e=
	if [ ${3} -ne 0 ]; then
		wr "static char key_${1}=$(echo ${2}-32 | bc);"
		search_pos Ctrl ${2} "" 1
		e=,
	fi
	if [ -n "${4}" ]; then
		search_pos Alt ${2} ${e}
	fi
	wr2 "},"
	if [ -n "${5}" ]; then
		nr=$(echo ${2}-32 | bc)
		search_pos Alt ${nr}
	else
		wr2 "0"
	fi
	wr2 "}"
}
find_pos () {
	wr2 ","
	_find_pos $1 $2 $3 $4 $5
}

wr2 "static keys_struct keys[]={"
_find_pos ocomp 97 1 1 1                    #a
i=98
while [ $i -lt 123 ]; do
	case $i in
		99) find_pos findword $i 1 1;;  #c
		101) find_pos mouse $i 1;;      #e
		102) find_pos find $i 1 1;;     #f
		103) find_pos goto $i 1 1;;     #g
		104) find_pos titles $i 1;;     #h
		106) find_pos actswf $i 1 1 1;; #j
		110) find_pos indents $i 1;;    #n
		111) find_pos paste $i 1 1;;    #o
		112) find_pos actswf2 $i 0 1 1;;#p
		113) find_pos quit $i 3;;       #q
		114) find_pos redo $i 1;;       #r
		115) find_pos save $i 1 1;;     #s
		116) find_pos insens $i 1;;     #t
		117) find_pos undo $i 1 1;;     #u
		118) find_pos visual $i 1 1;;   #v
		119) find_pos wrap $i 1;;       #w
		*) wr2 ",{nullptr,0}";;
	esac
	i=$((i+1))
done
wr2_n "};"
wr_n ""

wr "${buf}"
