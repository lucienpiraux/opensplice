/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2009 PrismTech 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#define MAX_ARGS 256

extern char **environ;
typedef char *cptr;
static cptr newargs[MAX_ARGS];
static int argNum = 0;

void addarg( char *pattern, char *val )
{
   char *winval;
   int newlen;
   int len = cygwin32_posix_to_win32_path_list_buf_size (val);
   winval = malloc(len);
   cygwin32_conv_to_win32_path(val, winval );
   newlen=strlen(pattern) + strlen(winval) -1;
   char *newarg = malloc( newlen );
   snprintf( newarg, newlen, pattern, winval );
   free(winval);
   newargs[argNum++] = newarg;
   if (argNum+1 >= MAX_ARGS)
   {
      fprintf(stderr, "Error: ospl_wincmd Max number args exceeded\n");
      fflush(stderr);
      exit (1);
   }
}


int main( int argc, char ** argv)
{
   int count;
   for ( count=1 ; count < argc; count++ )
   {
      char *arg = argv[count];
      if ( arg[0] == '-' )
      {
         if ( !strcmp( argv[1], "cl" )
              || !strcmp( argv[1], "link" )
              || !strcmp( argv[1], "lib" ))
         {
            switch ( arg[1] )
            {
               case 'L' :
               {
                  addarg( "-LIBPATH:%s", &arg[2] );
                  break;
               }
               case 'I' :
               {
                  addarg( "-I%s", &arg[2] );
                  break;
               }
               case 'l' :
               {
                  addarg( "%s.lib", &arg[2] );
                  break;
               }
               case 'o' :
               {
                  if ( arg[2] == '\0' )
                  {
                     char *narg=argv[count+1];
                     int arglen = strlen( narg );
		  
                     if ( narg[arglen-4] == '.' 
                          && narg[arglen-3] == 'o'
                          && narg[arglen-2] == 'b'
                          && narg[arglen-1] == 'j' )
                     {
                        addarg( "-Fo%s", narg );
                        count++;
                     }
                     else
                     {
                        addarg( "-OUT:%s", narg );
                        count++;
                     }
                  }
                  else
                  {
                     addarg( "%s", arg );
                  }
                  break;
               }
               default:
               {
                  addarg( "%s", argv[count] );
               }
            }
         }
         else
         {
            addarg( "%s", argv[count] );
         }
      }
      else
      {
         addarg( "%s", argv[count] );
      }
   }
   newargs[argNum] = NULL;
   if ( execvp( argv[1], newargs, environ ) == -1 )
   {
      fprintf(stderr, "ERROR: exec failed %d\n", errno);
      fflush(stderr);

   }
}
