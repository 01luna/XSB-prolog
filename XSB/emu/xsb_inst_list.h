XSB_INST(0x00, getpvar, _getpvar, P, V, R,X);
XSB_INST(0x01, getpval, _getpval, P, V, R,X);
XSB_INST(0x02, getstrv, _getstrv, PP, V, S,X);
XSB_INST(0x03, gettval, _gettval, P, R, R,X);
XSB_INST(0x04, getcon, _getcon, PP, R, C,X);
XSB_INST(0x05, getnil, _getnil, PP, R, X,X);
XSB_INST(0x06, getstr, _getstr, PP, R, S,X);
XSB_INST(0x07, getlist, _getlist, PP, R, X,X);
XSB_INST(0x08, unipvar, _unipvar, PP, V, X,X);
XSB_INST(0x09, unipval, _unipval, PP, V, X,X);
XSB_INST(0x0a, unitvar, _unitvar, PP, R, X,X);
XSB_INST(0x0b, unitval, _unitval, PP, R, X,X);
XSB_INST(0x0c, unicon, _unicon, PPP,C, X,X);
XSB_INST(0x0d, uninil, _uninil, PPP,X, X,X);
/*  XSB_INST(0x0e, getnumcon, _getnumcon, PP, R, N,X); */
/*  XSB_INST(0x0f, putnumcon, _putnumcon, PP, R, N,X); */
XSB_INST(0x0e, getnumcon, _getnumcon, PP, R, B,X);
XSB_INST(0x0f, putnumcon, _putnumcon, PP, R, B,X);

XSB_INST(0x10, putpvar, _putpvar, P, V, R,X);
XSB_INST(0x11, putpval, _putpval, P, V, R,X);
XSB_INST(0x12, puttvar, _puttvar, P, R, R,X);
XSB_INST(0x13, putstrv, _putstrv, PP, V, S,X);
XSB_INST(0x14, putcon, _putcon, PP, R, C,X);
XSB_INST(0x15, putnil, _putnil, PP, R, X,X);
XSB_INST(0x16, putstr, _putstr, PP, R, S,X);
XSB_INST(0x17, putlist, _putlist, PP, R, X,X);
XSB_INST(0x18, bldpvar, _bldpvar, PP, V, X,X);
XSB_INST(0x19, bldpval, _bldpval, PP, V, X,X);
XSB_INST(0x1a, bldtvar, _bldtvar, PP, R, X,X);
XSB_INST(0x1b, bldtval, _bldtval, PP, R, X,X);
XSB_INST(0x1c, bldcon, _bldcon, PPP,C, X,X);
XSB_INST(0x1d, bldnil, _bldnil, PPP,X, X,X);
/*  XSB_INST(0x1e, uninumcon, _uninumcon, PPP,N, X,X); */
/*  XSB_INST(0x1f, bldnumcon, _bldnumcon, PPP,N, X,X); */
XSB_INST(0x1e, uninumcon, _uninumcon, PPP,B, X,X);
XSB_INST(0x1f, bldnumcon, _bldnumcon, PPP,B, X,X);

XSB_INST(0x20, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x21, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x22, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x23, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x24, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x25, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x26, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x27, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x28, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x29, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x2a, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x2b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x2c, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x2d, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x2e, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x2f, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0x30, getattv, _getattv, PP, R, X,X);
XSB_INST(0x31, putattv, _putattv, PP, R, X,X);
XSB_INST(0x32, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x33, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x34, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x35, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x36, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x37, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x38, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x39, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x3a, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x3b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x3c, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x3d, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x3e, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x3f, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0x40, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x41, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x42, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x43, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x44, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x45, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x46, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x47, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x48, getlist_tvar_tvar, _getlist_tvar_tvar, R, R, R,X);
XSB_INST(0x49, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x4a, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x4b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x4c, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x4d, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x4e, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x4f, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0x50, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x51, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x52, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x53, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x54, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x55, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x56, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x57, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x58, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x59, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x5a, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x5b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x5c, trie_no_cp_attv, _trie_no_cp_attv,X,X,X,X);
XSB_INST(0x5d, trie_trust_attv, _trie_trust_attv,X,X,X,X);
XSB_INST(0x5e, trie_try_attv,   _trie_try_attv,  X,X,X,X);
XSB_INST(0x5f, trie_retry_attv, _trie_retry_attv,X,X,X,X);

XSB_INST(0x60, trie_no_cp_str, _trie_no_cp_str,PPP,X,X,X);
XSB_INST(0x61, trie_trust_str, _trie_trust_str, X, X, X,X);
XSB_INST(0x62, trie_try_str, _trie_try_str, X, X, X,X);
XSB_INST(0x63, trie_retry_str, _trie_retry_str, X,X,X,X);
XSB_INST(0x64, trie_no_cp_list, _trie_no_cp_list, X, X, X,X);
XSB_INST(0x65, trie_trust_list, _trie_trust_list, X,X,X,X);
XSB_INST(0x66, trie_try_list, _trie_try_list, X, X, X,X);
XSB_INST(0x67, trie_retry_list, _trie_retry_list, X,X,X,X);
XSB_INST(0x68, trie_no_cp_var, _trie_no_cp_var, X, X, X,X);
XSB_INST(0x69, trie_trust_var, _trie_trust_var, X, X, X,X);
XSB_INST(0x6a, trie_try_var, _trie_try_var, X, X, X,X);
XSB_INST(0x6b, trie_retry_var, _trie_retry_var, X,X,X,X);
XSB_INST(0x6c, trie_no_cp_val, _trie_no_cp_val, X, X, X,X);
XSB_INST(0x6d, trie_trust_val, _trie_trust_val, X, X, X,X);
XSB_INST(0x6e, trie_try_val, _trie_try_val, X, X, X,X);
XSB_INST(0x6f, trie_retry_val, _trie_retry_val, X,X,X,X);

XSB_INST(0x70, trie_no_cp_numcon, _trie_no_cp_numcon, X, X, X,X);
XSB_INST(0x71, trie_trust_numcon, _trie_trust_numcon, X, X, X,X);
XSB_INST(0x72, trie_try_numcon, _trie_try_numcon, X, X, X,X);
XSB_INST(0x73, trie_retry_numcon, _trie_retry_numcon, X,X,X,X);
XSB_INST(0x74, trie_no_cp_numcon_succ, _trie_no_cp_numcon_succ, X, X, X,X);
XSB_INST(0x75, trie_trust_numcon_succ, _trie_trust_numcon_succ, X, X, X,X);
XSB_INST(0x76, trie_try_numcon_succ, _trie_try_numcon_succ, X, X, X,X);
XSB_INST(0x77, trie_retry_numcon_succ, _trie_retry_numcon_succ, X,X,X,X);
XSB_INST(0x78, trie_proceed, _trie_proceed, X,X,X,X);
XSB_INST(0x79, hash_opcode, _hash_opcode, X,X,X,X);
XSB_INST(0x7a, hash_handle, _hash_handle, X,X,X,X);
XSB_INST(0x7b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x7c, trie_assert_inst, _trie_assert_inst,X,X,X,X);
XSB_INST(0x7d, trie_root, _trie_root, X, X, X,X);
XSB_INST(0x7e, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x7f, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0x80, getfloat, _getfloat, PP, R, F,X);
XSB_INST(0x81, putfloat, _putfloat, PP, R, F,X);
XSB_INST(0x82, unifloat, _unifloat, PPP,F, X,X);
XSB_INST(0x83, bldfloat, _bldfloat, PPP,F, X,X);
XSB_INST(0x84, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x85, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x86, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x87, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x88, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x89, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x8a, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x8b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x8c, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x8d, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x8e, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x8f, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0x90, trie_no_cp_fail, _trie_no_cp_fail,X,X,X,X);
XSB_INST(0x91, trie_trust_fail, _trie_trust_fail,X,X,X,X);
XSB_INST(0x92, trie_try_fail, _trie_try_fail,X,X,X,X);
XSB_INST(0x93, trie_retry_fail, _trie_retry_fail,X,X,X,X);
XSB_INST(0x94, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x95, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x96, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x97, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x98, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x99, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x9a, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x9b, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x9c, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x9d, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x9e, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0x9f, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0xa0, trymeelse, _trymeelse, PP, A, L,X);
XSB_INST(0xa1, retrymeelse, _retrymeelse, PP, A, L,X);
XSB_INST(0xa2, trustmeelsefail, _trustmeelsefail, PP, A, X,X);
XSB_INST(0xa3, try, _try,  PP, A, L,X);
XSB_INST(0xa4, retry, _retry, PP, A, L,X);
XSB_INST(0xa5, trust, _trust, PP, A, L,X);
XSB_INST(0xa6, getpbreg, _getpbreg, PP, V, X,X);
XSB_INST(0xa7, gettbreg, _gettbreg, PP, R, X,X);
XSB_INST(0xa8, putpbreg, _putpbreg, PP, V, X,X);
XSB_INST(0xa9, puttbreg, _puttbreg, PP, R, X,X);
XSB_INST(0xaa, jumptbreg, _jumptbreg, PP, R, L,X);
XSB_INST(0xab, getVn, _getVn,  PP, V, X,X);
XSB_INST(0xac, test_heap, _test_heap, PP, A, N,X);
XSB_INST(0xad, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xae, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xaf, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0xb0, switchonterm, _switchonterm, PPR, L, L,X);
XSB_INST(0xb1, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xb2, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xb3, switchonbound, _switchonbound, PPR, I, I,X);
XSB_INST(0xb4, switchon3bound, _switchon3bound, RRR, I, I,X);
XSB_INST(0xb5, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xb6, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xb7, trymeorelse, _trymeorelse, PP, A, L,X);
XSB_INST(0xb8, retrymeorelse, _retrymeorelse, PP, A, L,X);
XSB_INST(0xb9, trustmeorelsefail, _trustmeorelsefail, PP, A, X,X);
XSB_INST(0xba, dyntrustmeelsefail, _dyntrustmeelsefail, PP, A, L,X); 
XSB_INST(0xbb, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xbc, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xbd, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xbe, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xbf, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0xc0, tabletrysingle, _tabletrysingle, PP, A, L,T);
XSB_INST(0xc1, tabletry, _tabletry, PP, A, L,T); 
XSB_INST(0xc2, tableretry, _tableretry, PP, A, L,X);
XSB_INST(0xc3, tabletrust, _tabletrust, PP, A, L,X); 
XSB_INST(0xc4, check_complete, _check_complete, PPP, X, X,X); 
XSB_INST(0xc5, answer_return, _answer_return, PPP, X, X,X); 
XSB_INST(0xc6, resume_compl_suspension, _resume_compl_suspension, PPP, X, X,X);
XSB_INST(0xc7, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xc8, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xc9, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xca, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xcb, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xcc, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xcd, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xce, check_interrupt, _check_interrupt, PP, A, S,X);
XSB_INST(0xcf, new_answer_dealloc, _new_answer_dealloc, P, A, A, X);

XSB_INST(0xd0, term_comp, _term_comp, R, R, R,X);
XSB_INST(0xd1, movreg, _movreg, P, R, R,X);
XSB_INST(0xd2, negate, _negate, PP, R, X,X);
XSB_INST(0xd3, and, _and,  P, R, R,X);
XSB_INST(0xd4, or, _or,  P, R, R,X);
XSB_INST(0xd5, logshiftl, _logshiftl, P, R, R,X);
XSB_INST(0xd6, logshiftr, _logshiftr, P, R, R,X);
XSB_INST(0xd7, addreg, _addreg, P, R, R,X);
XSB_INST(0xd8, subreg, _subreg, P, R, R,X);
XSB_INST(0xd9, mulreg, _mulreg, P, R, R,X);
XSB_INST(0xda, divreg, _divreg, P, R, R,X);
XSB_INST(0xdb, idivreg, _idivreg, P, R, R,X);
/*  XSB_INST(0xdc, int_test_z, _int_test_z, PP, R, N, L); */
/*  XSB_INST(0xdd, int_test_nz, _int_test_nz, PP, R, N, L); */
XSB_INST(0xdc, int_test_z, _int_test_z, PP, R, B, L);
XSB_INST(0xdd, int_test_nz, _int_test_nz, PP, R, B, L);
XSB_INST(0xde, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xdf, no_inst, _no_inst, PPP,X,X,X);

XSB_INST(0xe0, putdval, _putdval, P, V, R,X);
XSB_INST(0xe1, putuval, _putuval, P, V, R,X);
XSB_INST(0xe2, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xe3, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xe4, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xe5, call_forn, _call_forn, PPP,L, X,X);
XSB_INST(0xe6, load_pred, _load_pred, PPP,S, X,X);
XSB_INST(0xe7, allocate_gc, _allocate_gc, P, A, A,X);
XSB_INST(0xe8, call, _call, PP, A, S,X);
XSB_INST(0xe9, allocate, _allocate, PPP,X, X,X);
XSB_INST(0xea, deallocate, _deallocate, PPP,X, X,X);
XSB_INST(0xeb, proceed, _proceed, PPP,X, X,X);
XSB_INST(0xec, execute, _execute, PPP,S, X,X);
XSB_INST(0xed, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xee, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xef, calld, _calld, PP, A, L,X);

XSB_INST(0xf0, jump, _jump, PPP,L, X,X);
XSB_INST(0xf1, jumpz, _jumpz, PP, R, L,X);
XSB_INST(0xf2, jumpnz, _jumpnz, PP, R, L,X);
XSB_INST(0xf3, jumplt, _jumplt, PP, R, L,X);
XSB_INST(0xf4, jumple, _jumple, PP, R, L,X);
XSB_INST(0xf5, jumpgt, _jumpgt, PP, R, L,X);
XSB_INST(0xf6, jumpge, _jumpge, PP, R, L,X);
XSB_INST(0xf7, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xf8, fail, _fail, PPP,X, X,X);
XSB_INST(0xf9, noop, _noop, PP, A, X,X);
XSB_INST(0xfa, halt, _halt, PPP,X, X,X);
XSB_INST(0xfb, builtin, _builtin, PP, A, X,X);
XSB_INST(0xfc, unifunc, _unifunc, P,X, A,R);
XSB_INST(0xfd, no_inst, _no_inst, PPP,X,X,X);
XSB_INST(0xfe, reset, _reset, PPP,X, X,X);
XSB_INST(0xff, no_inst, _no_inst, PPP,X,X,X);
