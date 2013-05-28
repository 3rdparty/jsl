JSL (Java as a Second Language) is a C++ library to make it easier to
"speak" Java without the complexity of explicit JNI.

If you just cloned this repository, you can build this library via:

```
  $ ./bootstrap
  $ ./configure
  $ make check
```

You can skip `./bootstrap` if you downloaded a distribution instead of
cloning the repository.

We depend on the following libraries:

  - boost
  - google-glog
  - stout

HOWEVER, all of these libraries are included in the `3rdparty`
directory.

---

## Motivation

JNI is a pain in the butt.

want to cache jclassid and jmethodid

can't have more than one jvm at a time

### Alternatives

jnipp: closest thing to Bilingua, heavily macro based. 

jni++: code generating utilities, didn't want a generator and still a
lot of verbosity.

## User Guide

create the JVM with your preferred options. otherwise, the first call
to Jvm::get will create the jvm for you. soon you will be able to
"inject" a JVM via Jvm::inject.

start programming in java. the memory model is that a C++ object has a
1-to-1 correspondance with a java reference (note, not to a java
object, just a reference to that java object). the c++ objects are
dumb wrappers, you can copy and assign and the references will be
properly accounted for under the covers. deleting a c++ object will
cause the associated java reference to get removed.

forgetting to delete a C++ object will cause a memory leak in the JVM.

if you can't find a java class you're looking for, you should probably
contribute! see below.

## Contributor Guide

can probably generate most/all of the code for what you want, for now
we'll grow the libraries organically as people add more to the
library. an aspect that we still need to resolve is versioning, i.e.,
java se 6 versus java se 7, etc.

### Classes

#### Inheritance

### Constructors

#### Super

### Methods

### Static Fields

### Instance Fields
