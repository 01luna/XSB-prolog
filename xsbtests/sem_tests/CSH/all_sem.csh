#!/usr/bin/csh

############################################################
# Test for Semantica parsing
############################################################
set TEST = "sem1"
echo "Testing $TEST"

$argv[1] -i << EOF

[sem_parse].

import numbervars/3 from num_vars.

load_dyn('tests/${TEST}.data').

tell('temp').

test(String), % write('/*-------'), write(String), writeln('-------*/'), nl,
parse(String, (H:-B)), numbervars(H,0,N), writeq(H), write(' :- '), % tab(8),
numbervars(B,N,_), writeq(B), writeln('.'), % nl, 
fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif

sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"

############################################################
set TEST = "sem2"
echo "Testing $TEST"

$argv[1] -i << EOF

[sem_parse].

import numbervars/3 from num_vars.

load_dyn('tests/english.data').

load_dyn('tests/${TEST}_test.data').

tell('temp').

test(String), % write('/*-------'), write(String), writeln('-------*/'), nl,
parse(String, (H:-B)), numbervars(H,0,N), writeq(H), write(' :- '), % tab(8),
numbervars(B,N,_), writeq(B), writeln('.'), % nl, 
fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


#----------------------------------------------------------------
# Test for Semantica partial evaluation
#
# NOTE that this file depends on the results of the sem1 test.
#----------------------------------------------------------------
############################################################
set TEST = "sem3"
echo "Testing $TEST"

$argv[1] -i << EOF

[sem_pe].

assert(sem_config_mode(basic)).

load_dyn('tests/${TEST}_test.data').

load_dyn('tests/sem1_old').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, % nl, 
fail.

told.

halt.
EOF


# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's load_config/1 and its partial evaluation
############################################################
set TEST = "sem4"
echo "Testing $TEST"

#$argv[1] -i -m 20000 << EOF

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem4.conf').

load_dyn('tests/sem4.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"

# Test for Semantica's load_config/1 and its partial evaluation
############################################################
set TEST = "sem5"
echo "Testing $TEST"

$argv[1] -i -m 5000 -c 5000 << EOF

[semantica,sem_pe].

load_config('tests/sem5.conf').

load_dyn('tests/sem5.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's load_config/1, its partial evaluation and pretty printing
############################################################
set TEST = "sem6"
echo "Testing $TEST"

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem6.conf').

load_dyn('tests/sem6.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"

# Test for Semantica's load_config/1, its partial evaluation and pretty printing
############################################################
set TEST = "sem7"
echo "Testing $TEST"

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem7.conf').

load_dyn('tests/sem7.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"

# Test for Semantica's partial evaluation,
# handling of events and their ambiguity
############################################################
set TEST = "sem8"
echo "Testing $TEST"

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem8.conf').

load_lex_interp('tests/sem8.lexi').

load_dyn('tests/sem8.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"

# Test for Semantica's partial evaluation with sets
############################################################
set TEST = "sem9"
echo "Testing $TEST"

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem9.conf').

load_dyn('tests/sem9.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's switches
############################################################
set TEST = "sem10"
echo "Testing $TEST"

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem10.conf').

assert(sem_switch(debug_on)).

tell('temp').

Tree = 'S'('NP'('N'('Chris')), 'VP'('V'(walks))),
%writeln('/* ---------------------------------------'),
%write('   Default PE : '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

retract(sem_switch(debug_on)).

Tree = 'S'('NP'('N'('Chris')), 'VP'('V'(walks))),
%writeln('/* ---------------------------------------'),
%write('   No Vals PE : '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's performance in doing partial evaluation
############################################################
set TEST = "sem11"
echo "Testing $TEST"

$argv[1] -i -m 5000 -c 5000 << EOF

[semantica,sem_pe].

load_config('tests/sem11.conf').

load_dyn('tests/sem11.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's load_config/1, load_lex_interp/1,
# its partial evaluation and pretty printing
############################################################
set TEST = "sem12"
echo "Testing $TEST"

#$argv[1] -i -m 20000  << EOF
$argv[1] -i  << EOF

[semantica,sem_pe].

load_config('tests/sem12.conf').

load_lex_interp('tests/sem12.lexi').

load_dyn('tests/sem12.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's load_config/1, load_lex_interp/1,
# its partial evaluation and pretty printing
############################################################
set TEST = "sem13"
echo "Testing $TEST"

#$argv[1] -i -m 20000 << EOF

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem13.conf').

load_lex_interp('tests/sem13.lexi').

load_dyn('tests/sem13.sent').

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for parsing and simplification inside sets.
############################################################
set TEST = "sem14"
echo "Testing $TEST"

$argv[1] -i << EOF

[sem_parse, sem_pe].

import numbervars/3 from num_vars.

load_dyn('tests/sem14_test.data').

tell('temp').

test(String), parse(String, Translation), assert(Translation), fail.

%writeln('/*------------- Results of parser -----------------*/'), nl,
listing(test/1).

listing(val/2).

assert(sem_config_mode(basic)).
assert(sem_switch(debug_on)).

%writeln('/*------------- Results of simplification  ----------------*/'), nl,
simplify('S'('NP'('D'(every),'N'(man)),'VP'('V'(walks))), Answer),
writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's set-theory reductions
############################################################
set TEST = "sem15"
echo "Testing $TEST"

#$argv[1] -i -m 64000 << EOF

$argv[1] -i << EOF

import sched_strat/1 from machine.

% (sched_strat(local_sched) -> unix('cp tests/sem15_old temp'), halt; true).

[semantica,sem_pe].

load_config('tests/sem15.conf').

load_dyn('tests/sem15.sent').

assert(sem_switch(set_theory_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's set-theory reductions
############################################################
set TEST = "sem16"
echo "Testing $TEST"

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem16.conf').

load_lex_interp('tests/sem16.lexi').

load_dyn('tests/sem16.sent').

assert(sem_switch(debug_on)).

assert(sem_switch(set_theory_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's set-theory reductions
############################################################
set TEST = "sem17"
echo "Testing $TEST"

#$argv[1] -i -m 20000 << EOF

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem17.conf').

load_lex_interp('tests/sem17.lexi').

load_dyn('tests/sem17.sent').

assert(sem_switch(debug_on)).

assert(sem_switch(set_theory_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's set-theory reductions
############################################################
set TEST = "sem18"
echo "Testing $TEST"

#$argv[1] -i -m 20000 << EOF

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem18.conf').

load_lex_interp('tests/sem18.lexi').

load_dyn('tests/sem18.sent').

assert(sem_switch(debug_on)).

assert(sem_switch(set_theory_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's set-theory reductions
############################################################
set TEST = "sem19"
echo "Testing $TEST"

#$argv[1] -i -m 20000 << EOF

$argv[1] -i << EOF

[semantica,sem_pe].

load_config('tests/sem19.conf').

load_lex_interp('tests/sem19.lexi').

load_dyn('tests/sem19.sent').

assert(sem_switch(debug_on)).

assert(sem_switch(set_theory_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's set-theory reductions
############################################################
set TEST = "sem20"
echo "Testing $TEST"

#$argv[1] -i -m 25000 -c 5000 << EOF

$argv[1] -i -m 5000 -c 5000 << EOF

[semantica,sem_pe].

assert(mm(H, [H|_])), fail.

assert((mm(H, [_|T]) :- mm(H, T))), fail.  % Define member/2

load_config('tests/sem20.conf').

load_lex_interp('tests/sem20.lexi').

load_dyn('tests/sem20.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
setof(Answer, simplify(Tree,Answer), L), mm(A, L), writeq(A), nl, fail.

%writeln('%======== With SET THEORY on =================================='), nl.

abolish_all_tables.

assert(sem_switch(set_theory_on)).

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF
# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"


# Test for Semantica's identity treatment
############################################################
set TEST = "sem21"
echo "Testing $TEST"

#$argv[1] -i -m 20000 << EOF

$argv[1] -i << EOF

[semantica,sem_pe,basics].

load_config('tests/sem21.conf').

load_lex_interp('tests/sem21.lexi').

load_dyn('tests/sem21.sent').

assert(sem_switch(debug_on)).

tell('temp').

test(Num, Tree),
%writeln('/* ---------------------------------------'),
%write(Num), write('. '), writeln(Tree),
%writeln('   --------------------------------------- */'), nl,
simplify(Tree, Answer), writeq(Answer), nl, fail.

told.

halt.
EOF

# print out differences.
if (-r tests/${TEST}_new) then
	/bin/rm tests/${TEST}_new
endif
sort temp > tests/${TEST}_new

set d = `diff tests/${TEST}_new tests/${TEST}_old`
if ( $#d == 0 ) then 
	echo "$TEST tested"
else
	echo "$TEST different\!\!\!"
	diff tests/${TEST}_new tests/${TEST}_old
endif
echo "---------------------------------------------------------------------"
