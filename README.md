## jascii -- filter/count non-ASCII characters

jascii will examine any file and print out ASCII data.

This was created for Systems that does not have strings(1).
If strings(1) is present, you may want to use that instead.

To build, execute build.sh to generate a Makefile from
Makefile.template on most BSD/Linux systems and IBM AIX.
For MS-DOS, it should be rather easy to create a Makefile.

This requires [j\_lib2](https://github.com/jmcunx/j_lib2) to build.

[GNU automake](https://en.wikipedia.org/wiki/Automake)
only confuses me, so I came up with my own method which
is a real hack.  But works for me.
