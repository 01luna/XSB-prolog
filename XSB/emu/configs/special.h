/* File:      configs/special.h -- auxiliary defs that might not be everywhere
** Author(s): kifer
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1998
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
** $Id: special.h,v 1.3 1998-11-18 16:16:06 kifer Exp $
** 
*/


/* This stuff isn't defined in NT */
#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode & S_IFMT) == S_IFDIR)
#endif

#ifdef WIN_NT
#define snprintf _snprintf
#endif

/* The separator used between pathnames in PATH environment */
#ifdef WIN_NT
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif
