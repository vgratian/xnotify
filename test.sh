#!/bin/env bash

### script for testing xnotify in different languages
### optional argument: font name

# used autotranslation for most
msg=(
  'hey!!!' 'whaaasup? :)  😊'          # english
  'նամակ' 'ի՞նչ կա իմ ախպեր'            # armenian
  'やあ、' '元気かい、マイ・グッディ'   # japanese
  'Γεια σου πώς'  'είσαι καλούλη μου'   # greek
  'Привет' 'как дела, мой хороший'      # russian
  'Ahoj' 'jak se máš, můj miláčku'      # czech
)

test -e config.h || make config.h || exit 1

if [ -n "$1" ]; then
    echo "using font family [$1]"
	lineno=$(grep -n '^#define FONT_FAMILY "' config.h | tail -1 | cut -d: -f1 )
    echo "lineno = $lineno"
    test "$lineno" -gt 0 2>/dev/null || exit 1
	sed -i "$lineno d" config.h
	sed -i "$lineno i #define FONT_FAMILY \"$1\"" config.h
	make || exit 1
elif [ -z "xnotify" ]; then
    make || exit 1
fi

let i=0

while [ $i -lt "${#msg[@]}" ]; do
    printf "%2d: %-25s %s\n" $i "${msg[$i]}" "${msg[(($i+1))]}"
    ./xnotify "${msg[$i]}" "${msg[(($i+1))]}"
    ((i+=2))
    sleep 1
done
