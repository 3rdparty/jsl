# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([libjsl], [0.0.1])

# Have autoconf setup some variables related to the system.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET

AC_LANG([C++])

AC_CONFIG_MACRO_DIR([m4])

# Initialize automake.
# -Wno-portability, since we require GNU Make for % patterns.
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability foreign])

# Required for linking non-POSIX libs.
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Initialize libtool (LT_OUTPUT builds ./libtool immediately, needed
# if we want to do tests with libtool during configuration).
LT_PREREQ([2.2])
LT_INIT
LT_LANG([C++])
LT_OUTPUT

# Save the configure arguments so we can pass them to any third_party
# libraries that we might run configure on (see
# third_party/Makefile.am). One downside of our strategy for shipping
# and building third_party libraries is that we can't expose options
# from nested third_party configure scripts.
CONFIGURE_ARGS="$ac_configure_args"
AC_SUBST(CONFIGURE_ARGS)

AC_CONFIG_SUBDIRS([3rdparty/stout])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([3rdparty/Makefile])

AC_ARG_VAR([JAVA_HOME], [location of Java Development Kit (JDK)])

AC_ARG_VAR([JNI_CPPFLAGS], [preprocessor flags for JNI])

AC_ARG_VAR([JNI_LDFLAGS], [linker flags for JNI])

# Do some OS specific setup.
case "${target_os}" in
  linux*)
    OS_NAME=linux # For determining JNI linker flags.
    LIBS="$LIBS -lrt" # For clock_gettime() in stout/stopwatch.hpp.
    ;;
  darwin*)
    OS_NAME=darwin # For determining JNI linker flags.
    ;;
  *)
    ;;
esac

# Checks for gcc toolchain (we rely on some atomic builtins for now).
AC_PROG_CXX([g++])
AC_PROG_CC([gcc])

# Check for pthreads (uses m4/acx_pthread.m4).
ACX_PTHREAD([], [AC_MSG_ERROR([failed to find pthreads])])

# A helper for checking whether we can compile and link using JNI with
# the current JNI_CPPFLAGS and JNI_LDFLAGS.
# TRY_LINK_JNI([ACTION-SUCCESS], [ACTION-FAILURE])
AC_DEFUN([TRY_LINK_JNI], [
  cat <<__EOF__ >conftest.cpp [
#include <jni.h>
int main(int argc, char** argv)
{
  JNIEnv* env;
  JavaVM* jvm;
  JavaVMInitArgs vmArgs;
  return JNI_CreateJavaVM(&jvm, (void**) &env, &vmArgs);
}]
__EOF__

  # Try to compile and link via libtool (the one we generate).
  ./libtool --tag=CXX --mode=link $CXX -Wall -Werror $JNI_CPPFLAGS \
    -o conftest conftest.cpp $JNI_LDFLAGS >&5

  if test $? != 0; then
    rm -f conftest # Cleanup after ourselves.
    $2 # Expand failure action.
  else
    rm -f conftest # Cleanup after ourselves.
    $1 # Expand success action.
  fi
])

# First let's try and determine JAVA_HOME if it hasn't been set. We
# do this by checking to see if the directory found at the
# 'java.home' system property for the java found on the path
# includes javac. If so, then we'll guess that this is a JDK
# installation rather than a JRE installation.
if test -z "$JAVA_HOME"; then
  AC_PATH_PROG([JAVAC], [javac], [$JAVAC])
  AC_PATH_PROG([JAVA], [java], [$JAVA])

  if test "x$JAVA" = "x" || test "x$JAVAC" = "x"; then
    AC_MSG_ERROR([can not guess JAVA_HOME (no 'java' or 'javac' found)])
  fi

  AC_MSG_CHECKING([value of Java system property 'java.home'])
  cat <<__EOF__ >conftest.java [
public class conftest {
  public static void main(String[] args) {
    System.out.print(System.getProperty("java.home"));
  }
}]
__EOF__

  # Now build and run the code.
  $JAVAC conftest.java && JAVA_DOT_HOME="`$JAVA -cp . conftest`"

  if test $? = 0 && test ! -z "$JAVA_DOT_HOME"; then
    AC_MSG_RESULT($JAVA_DOT_HOME)
  else
    JAVA_DOT_HOME=""
    AC_MSG_RESULT([not found])
  fi

  # Clean up after ourselves.
  rm -f conftest.java conftest.class

  # Check if 'java.home' looks like a JDK installation, or if
  # 'java.home'/.. looks like a JDK installation (e.g., Linux).
  if test -f $JAVA_DOT_HOME/bin/javac; then
    JAVA_HOME=$JAVA_DOT_HOME
  elif test -f `dirname $JAVA_DOT_HOME`/bin/javac; then
    JAVA_HOME=`dirname $JAVA_DOT_HOME`
  fi

  if test -z "$JAVA_HOME"; then
    AC_MSG_ERROR([could not guess JAVA_HOME])
  else
    AC_MSG_NOTICE([using JAVA_HOME=$JAVA_HOME])
  fi
fi

# Determine linker flags for Java if not set.
if test -z "$JNI_LDFLAGS"; then
  if test "$OS_NAME" = "darwin"; then
    JNI_LDFLAGS="-framework JavaVM"
  elif test "$OS_NAME" = "linux"; then
    JNI_LDFLAGS=""
    for arch in amd64 i386; do
      dir="$JAVA_HOME/jre/lib/$arch/server"
      if test -e "$dir"; then
        # Note that these are libtool specific flags.
        JNI_LDFLAGS="-L$dir -R$dir -Wl,-ljvm"
        break;
        fi
    done
  fi
fi

if test -z "$JNI_LDFLAGS"; then
  AC_MSG_ERROR([failed to determine linker flags for using the JVM \
(bad JAVA_HOME or missing support for your architecture?)])
fi

# Now try and build with JNI, looping through possible compiler
# flags as necessary (provided JNI_CPPFLAGS was not set).
AC_MSG_CHECKING([whether or not we can build with JNI])
if test -z "$JNI_CPPFLAGS"; then
  if test "$OS_NAME" = "darwin"; then
    while true; do # Loop until sucessful (via break) or exhausted options.
      m4_foreach([jni_cppflags],
                 [["-I$JAVA_HOME/include -I$JAVA_HOME/include/$OS_NAME"],
                  ["-I/System/Library/Frameworks/JavaVM.framework/Headers"]],
                 [JNI_CPPFLAGS=jni_cppflags
                  TRY_LINK_JNI([break])])
      AC_MSG_ERROR([failed to build with JNI]) # Exhausted options.
    done
  else
    while true; do # Loop until sucessful (via break) or exhausted options.
      m4_foreach([jni_cppflags],
                 [["-I$JAVA_HOME/include -I$JAVA_HOME/include/$OS_NAME"]],
                 [JNI_CPPFLAGS=jni_cppflags
                  TRY_LINK_JNI([break])])
      AC_MSG_ERROR([failed to build with JNI]) # Exhausted options.
    done
  fi
else
  TRY_LINK_JNI([], [AC_MSG_ERROR([failed to build with JNI])])
fi
AC_MSG_RESULT([yes])

AC_OUTPUT
