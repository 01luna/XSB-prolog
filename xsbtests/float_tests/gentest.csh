#=============================================================================
#set $argv[1] = EMULATOR
#set $argv[2] = FILE 
#set $argv[3] = COMMAND
#=============================================================================
echo "--------------------------------------------------------------------"
echo "Testing $argv[2]"
$argv[1] -m 3000 -i << EOF
[$argv[2]].
tell(temp).
$argv[3]
told.
EOF

#-----------------------
# Do NOT sort the files.
#-----------------------
if (-e ${argv[2]}_new) then
	/bin/rm -f ${argv[2]}_new
endif
\mv temp ${argv[2]}_new

#-----------------------
# print out differences.
#-----------------------
set d = `diff ${argv[2]}_new ${argv[2]}_old`
if ( $#d == 0 ) then 
	echo "$argv[2] tested"
	\rm -f ${argv[2]}_new
else
	echo "$argv[2] different\!\!\!"
	diff ${argv[2]}_new ${argv[2]}_old
endif
