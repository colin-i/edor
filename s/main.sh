#!/bin/sh

level_help=0
level_names=1
level_pref_wr=2
level_pref_rd=3
level_map=4
if [ -z "${level}" ]; then level=${level_map}; fi

f=main.h
wr_n() { printf '%s' "$@" >> ${f}; echo >> ${f}; }
wr () { printf '%s' "$@" >> ${f}; }
wr2_n() { buf="${buf}$@
"; }
wr2 () { buf="${buf}$@"; }
wr3_n() { buf2="${buf2}$@
"; }
wr3 () { buf2="${buf2}$@"; }
wr4_n() { buf3="${buf3}$@
"; }
wr4 () { buf3="${buf3}$@"; }

printf '%s\n' "#define hel1 \"USAGE\n\"" > ${f}
wr "#define hel2 \""
d2=" [filepath [line_termination: rn/r/n]]\
\n      --remove-config      Remove configuration files.\
\nINPUT\
\nthis help: q(uit),up/down,mouse/touch V.scroll\
\nMovement:\
\n    [Ctrl/Alt/Shift +]arrows/home/end/del,page up,page down,backspace,enter\
\n    p.s.: Ctrl+ left/right/del breaks at white-spaces and (),[]{}\
\n    mouse/touch Click and V.scroll"
wr "${d2}"
d2s="$(echo -n "${d2}" | sed "s/\\\n/n/g" | wc -c)" #more at second sed of this kind, if here was \\\" was another command
fix_s="$(echo ${d2s}+4 | bc)"
fix_s2="$(echo ${fix_s}+1 | bc)"

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
\nCtrl+l = titles (movement, Enter at done, Ctrl+q, other keys to search)\
\nCtrl+w = text wrapping (movement. another key to return)\
\nCtrl+e = enable/disable internal mouse/touch\
\nCtrl+n = disable/enable indentation\
\nCtrl+t = enable/disable insensitive search\
\nCtrl+a = enable/disable O language syntax; Alt+a = syntax rescan; Alt+A = change rule extension name (blank is all)\
\nCtrl+j = enable/disable OA split syntax; Alt+j = change delimiter; Alt+J = change view delimiter\
\n    Alt+p = change splits folder (blank is mixless); Alt+P = change rule extension name for splits (blank is all)\
\nCtrl+y = enable/disable spaces->tabs indents at read, tabs->spaces at write; Alt+y = change rule extension name (blank is all)\
\nCtrl+b = change tab size\
\nCtrl+z = switch keys, applies to Ctrl and lower/upper Alt (example: az , +a becomes +z and +z becomes +a)\
\nCtrl+q = quit; Alt+q = quit and return EXIT_FAILURE\
\nabcdefghijklmnopqrstuvwxyz\""
wr_n "${text}"

if [ ${level} -eq ${level_help} ]; then
	exit 0
fi

#QWERTYU O    ip
#AS FG J L dk h
# Z CVBN   x  m
# ^M is 13 that comes also at Enter, ^I is 9 that comes also at Tab
# ^H at termux is 263 backspace
# ^P at docker, something is not ok with the redraw
# ^A in termux is from Ctrl+Alt+a

textsed="$(echo "${text}" | sed "s/\\\n/n/g; s/\\\\\"/\"/g")"  # replace \n to n and \\\" to \"(this will go " at grep). \ at endings are 0
#sh will not see that \n but will cut ok
search_pos () {
	wr2 ${3}
	txt=$(printf "$1+\\$(printf %o $2)")
	p=$(echo "${textsed}" | grep -b -o ${txt} | cut -d':' -f1 )
	a=
	for nr in ${p}; do
		nr=$(echo $(if [ -n "$4" ]; then echo -n ${fix_s2}; else echo -n ${fix_s}; fi)+${nr} | bc)
		wr2 "${a}${nr}"
		if [ -z "${a}" ]; then a=","; fi
		z=$((z+1))
	done
}

nothing="{nullptr,nullptr,0,0,0}"

_find_pos () { #name=$1 letter=$2 ctrls=$3 alt=$4 bigalt=$5
	if [ -z "${is_extern}" ]; then wr "static "; fi
	wr "char key_${1}=$(echo ${2}-32 | bc);"
	wr2 "{&key_${1},(unsigned short[]){"
	e=
	z=0
	if [ ${3} -ne 0 ]; then
		search_pos Ctrl ${2} "" 1
		e=,
	fi
	if [ -n "${4}" ]; then
		search_pos Alt ${2} ${e}
	fi
	wr2 "},$z,"
	if [ -n "${5}" ]; then
		nr=$(echo ${2}-32 | bc)
		search_pos Alt ${nr}
	else
		wr2 "0"
	fi
	wr2 ",${number_of_keys}}"
	wr3 $((${2}-97)); wr4 "${nothing}"
	number_of_keys=$((number_of_keys+1))
}
find_pos () {
	wr2 ","; wr3 ","; wr4 ","
	_find_pos $1 $2 $3 $4 $5
}

number_of_keys=0
_find_pos                 ocomp              97 1 1 1   #a
i=98
while [ $i -lt 123 ]; do
	case $i in
		98) find_pos tab                 $i 1;;     #b
		99) find_pos findword            $i 1 1;;   #c
		101) find_pos mouse              $i 1;;     #e
		102) find_pos find               $i 1 1;;   #f
		103) find_pos goto               $i 1 1;;   #g
		106) find_pos actswf             $i 1 1 1;; #j
		108) is_extern=x find_pos titles $i 1;;     #l
		110) find_pos indents            $i 1;;     #n
		111) find_pos paste              $i 1 1;;   #o
		112) find_pos actswf2            $i 0 1 1;; #p
		113) is_extern=x find_pos quit   $i 3 1;;   #q
		114) find_pos redo               $i 1;;     #r
		115) is_extern=x find_pos save   $i 1 1;;   #s
		116) find_pos insens             $i 1;;     #t
		117) find_pos undo               $i 1 1;;   #u
		118) find_pos visual             $i 1 1;;   #v
		119) is_extern=x find_pos wrap   $i 1;;     #w
		121) find_pos whites             $i 1 1;;   #y
		122) find_pos swkey              $i 1;;     #z
		*) wr2 ",${nothing}"; wr4 ",${nothing}";;
	esac
	i=$((i+1))
done
wr_n ""

#can define here static a[]; and defines to compile right but is a risk and better to undo them
wr_n "#define A_to_a 0x20"
if [ ${level} -ge ${level_pref_wr} ]; then
	wr_n "static char keys_row_orig[]={${buf2}};"
	wr_n "char*keys_row=keys_row_orig;"
	if [ ${level} -ge ${level_pref_rd} ]; then
		wr_n "static char keys_row_frompref[]={${buf2}};"
		if [ ${level} -ge ${level_map} ]; then
			wr_n "static key_struct keys_frompref[]={${buf3}};"  #important to define same here and not at runtime
			wr_n "static key_struct keys_orig[]={${buf}};"
			wr_n "key_struct*keys=keys_orig;"
			wr_n "static char*keys_helptext;"
			wr_n "#define key_last_index $((122-97))"
			wr_n "#define _0_to_A 0x41"
			wr_n "#define number_of_keys ${number_of_keys}"
			wr_n "char*keys_help;"
			wr_n "#define help_last_part_size $((26+1))"
		fi
	fi
fi
