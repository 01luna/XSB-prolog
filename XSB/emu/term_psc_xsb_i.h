/* File:      term_psc_xsb_i.h
** Author(s): Xu, Warren, Sagonas, Swift
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1986, 1993-1999
** Copyright (C) ECRC, Germany, 1990
** 
** XSB is free software; you can redistribute it and/or modify it under the
** terms of the GNU Library General Public License as published by the Free
** Software Foundation; either version 2 of the License, or (at your option)
** any later version.
** 
** XSB is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
** more details.
** 
** You should have received a copy of the GNU Library General Public License
** along with XSB; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** $Id: term_psc_xsb_i.h,v 1.2 2005-01-14 18:31:35 ruim Exp $
** 
*/



static inline Psc term_psc(Cell term)
{
  int value;
  Psc psc;
  Pair sym;

  if (isconstr(term))
    return get_str_psc(term);
  else {
    if (isstring(term)) {
      psc = (Psc)flags[CURRENT_MODULE];
      sym = insert(string_val(term), 0, psc, &value);
      return pair_psc(sym);
    }
    else return NULL;
  }
}
