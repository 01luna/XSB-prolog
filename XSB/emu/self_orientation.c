/* File:      self_orientation.c - find out where xsb stuff is
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
** $Id: self_orientation.c,v 1.8 1998-12-23 21:09:17 kifer Exp $
** 
*/



#include "configs/config.h"
#include "basictypes.h"

char executable[MAXPATHLEN];		/* This is set to a real name below */

char *install_dir; 			/* installation directory */
char *xsb_config_file;     		/* XSB configuration file */
char *user_home;     	     	     	/* the user $HOME dir or install dir,
					   if $HOME is null */ 

#ifdef WIN_NT
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
/* special.h must be included after sys/stat.h */
#include "configs/special.h"

extern bool is_absolute_filename(char *);
extern char *strip_names_from_path(char*, int);

static void check_create_dir(char *);

char current_dir[MAXPATHLEN];
char xsbinfo_dir[MAXPATHLEN];


void set_xsbinfo_dir () {
  struct stat fileinfo;
  char old_xinitrc[MAXPATHLEN], new_xinitrc[MAXPATHLEN],
    user_config_dir[MAXPATHLEN], user_arch_dir[MAXPATHLEN];
  int retcode;

  sprintf(xsbinfo_dir, "%s%c.xsb", user_home, SLASH);
  sprintf(old_xinitrc, "%s%c.xsbrc", user_home, SLASH);
  sprintf(new_xinitrc, "%s%cxsbrc", xsbinfo_dir, SLASH);
  sprintf(user_config_dir, "%s%cconfig", xsbinfo_dir, SLASH);
  sprintf(user_arch_dir, "%s%c%s", user_config_dir, SLASH, FULL_CONFIG_NAME);

  /* Create USER_HOME/.xsb directory, if it doesn't exist. */
  check_create_dir(xsbinfo_dir);
  check_create_dir(user_config_dir);
  check_create_dir(user_arch_dir);

  retcode = stat(old_xinitrc, &fileinfo);

  if ((retcode == 0) && (stat(new_xinitrc, &fileinfo) != 0)) {
    fprintf(stderr,
	    "*************************************************************\n");
    fprintf(stderr,
	    "****It appears that you have an old-style `.xsbrc' file!*****\n");
    fprintf(stderr, "The XSB initialization file is now %s.\n", new_xinitrc);
    fprintf(stderr, "If your `.xinitrc' defines the `library_directory' predicate.\n");
    fprintf(stderr,
	    "Please consult the XSB manual for the new conventions.\n");
    fprintf(stderr,
	    "*************************************************************\n");
  }
}


/* Check if PATH exists. Create if it doesn't. Bark if it can't create or if
   PATH exists, but isn't a directory. */
static void check_create_dir(char *path) {
  struct stat fileinfo;
  int retcode = stat(path, &fileinfo);

  if (retcode == 0 && ! S_ISDIR(fileinfo.st_mode)) {
    fprintf(stderr,
	    "*************************************************************\n");
    fprintf(stderr, "File `%s' exists, but isn't a directory.\n", path);
    fprintf(stderr, "XSB needs directory `%s' to store data.\n", path);
    fprintf(stderr,
	    "*************************************************************\n");
    exit(1);
  }

  if (retcode != 0) 
#ifdef WIN_NT
    retcode = mkdir(path);
#else
    retcode = mkdir(path, 0755);
#endif

  if (retcode != 0) {
    fprintf(stderr,
	    "*************************************************************\n");
    fprintf(stderr, "Cannot create directory `%s'.\n", path);
    fprintf(stderr, "XSB needs directory `%s' to store data.\n", path);
    fprintf(stderr,
	    "*************************************************************\n");
    exit(1);
  }
}

/* uses the global executable var */
char *xsb_executable_full_path(char *myname)
{
  struct stat fileinfo;
  char *path = getenv("PATH");
  int link_len, len, found = 0;
  char *pathcounter, save;
  static char myname_augmented[MAXPATHLEN];

  
#ifndef WIN_NT
  /* Unix */
  /* if we can read symlink, then it is a symlink */
  if ( (link_len = readlink(myname, myname_augmented, MAXPATHLEN)) > 0 ) {
    /* we can't assume that the value of the link is null-terminated */
    if ( *(myname_augmented+link_len) != '\0' )
      *(myname_augmented+link_len+1) = '\0';
  } else
    strcpy(myname_augmented, myname);
#else
  /* Windows doesn't seem to have readlink() */
  strcpy(myname_augmented, myname);
  /* if executable doesn't end with .exe, then add it */
  if (strcmp(myname + strlen(myname) - 4, ".exe") != 0)
    sprintf(myname_augmented, "%s.exe", myname);
#endif

  if (is_absolute_filename(myname_augmented))
    strcpy(executable, myname_augmented);
  else {
    getcwd(current_dir, MAXPATHLEN-1);
    sprintf(executable, "%s%c%s", current_dir, SLASH, myname_augmented);
  }

  /* found executable by prepending cwd */
  if (!stat(executable, &fileinfo)) return executable;

  /* Otherwise, search PATH environment var.
     This code is a modified "which" shell builtin */
  pathcounter = path;
  while (*pathcounter != '\0' && found == 0) {
    len = 0;
    while (*pathcounter != PATH_SEPARATOR && *pathcounter != '\0') {
      len++;
      pathcounter++;
    }

    /* save the separator ':' (or ';' on NT and replace it with \0) */
    save = *pathcounter;
    *pathcounter = '\0';

    /* Now `len' holds the length of the PATH component 
       we are currently looking at.
       `pathcounter' points to the end of this component. */
    sprintf(executable, "%s%c%s", pathcounter - len, SLASH, myname_augmented);

    /* restore the separator and addvance the pathcounter */
    *pathcounter = save;
    if (*pathcounter) pathcounter++;

#ifdef WIN_NT
    found = (0 == access(executable, 02));	/* readable */
#else
    found = (0 == access(executable, 01));	/* executable */
#endif
    if (found) return executable;
  }

  /* XSB executable isn't found after searching PATH */
  fprintf(stderr,
	  "*************************************************************\n");
  printf("PANIC!!! Can't determine the full name of the XSB executable!\n");
  printf("         Please report this bug to xsb-contact@cs.sunysb.edu\n");
  fprintf(stderr,
	  "*************************************************************\n");
  exit(1);
}

void set_install_dir() {

  /* strip 4 levels, since executable is always of this form:
     install_dir/config/<arch>/bin/xsb */
  install_dir = strip_names_from_path(executable, 4);
  if (install_dir == NULL) {
    fprintf(stderr,
	    "*************************************************************\n");
    fprintf(stderr, "PANIC!! Can't find the XSB installation directory.\n");
    fprintf(stderr, "Perhaps, you moved the XSB executable out of \n");
    fprintf(stderr, "its normal place in the XSB directory structure?\n");
    fprintf(stderr,
	    "*************************************************************\n");
    exit(1);
  }
}

void set_config_file() {
  int retcode;
  struct stat fileinfo;

  /* The config file is always 1 directory below the executable. */
  xsb_config_file = strip_names_from_path(executable, 2);
  sprintf(xsb_config_file+strlen(xsb_config_file), "%cconfiguration.P", SLASH);

  /* Perform sanity checks: xsb_config_file must be in install_dir/config
     This is probably redundant */
  if ( strncmp(install_dir, xsb_config_file, strlen(install_dir)) != 0 
       || (strstr(xsb_config_file, "config") == NULL) ) {
    fprintf(stderr,
	    "*************************************************************\n");
    fprintf(stderr, "PANIC!! The file configuration.P\n");
    fprintf(stderr,
	    "is not where it is expected: %s%cconfig%c%s\n",
	    install_dir, SLASH, SLASH, FULL_CONFIG_NAME);
    fprintf(stderr, "Perhaps you moved the XSB executable %s\n", executable);
    fprintf(stderr, "away from its usual place?\n");
    fprintf(stderr,
	    "*************************************************************\n");
    exit(1);
  }

  /* Check if configuration.P exists and is readable */
  retcode = stat(xsb_config_file, &fileinfo);
  if ( (retcode != 0) || !(S_IREAD & fileinfo.st_mode) ) {
    fprintf(stderr,
	    "*************************************************************\n");
    fprintf(stderr, "PANIC! XSB configuration file %s\n", xsb_config_file);
    fprintf(stderr, "doesn't exist or is not readable by you.\n");
    fprintf(stderr,
	    "*************************************************************\n");
    exit(1);
  }
}

void set_user_home() {
  user_home = (char *) getenv("HOME");
  if ( user_home == NULL )
    user_home = install_dir;
}
