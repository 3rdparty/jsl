#ifndef __JVM_HPP__
#define __JVM_HPP__

#include <jni.h>

#include <string>
#include <vector>

#include <stout/try.hpp>

// Forward declaration.
namespace java { namespace lang { class Object; } }


// Encapsulates JNI specific components, in particular the all
// important 'environment' (see below).
struct JNI
{
  enum Version
  {
    v_1_1 = JNI_VERSION_1_1,
    v_1_2 = JNI_VERSION_1_2,
    v_1_4 = JNI_VERSION_1_4,
    v_1_6 = JNI_VERSION_1_6
  };

  // Each thread that wants to interact with the JVM needs a JNI
  // environment which must be obtained by "attaching" to the JVM. We
  // use the following RAII class to provide the environment and also
  // make sure a thread is attached and properly detached. Note that
  // nested constructions are no-ops and use the same environment (and
  // without detaching too early).
  class Env
  {
  public:
    Env(bool daemon = true);
    ~Env();

    JNIEnv* operator -> () const { return env; }

    operator JNIEnv* () const { return env; }

  private:
    JNIEnv* env;
    bool detach; // A nested use of Env should not detach the thread.
  };
};


// Facilitates embedding a JVM and calling into it.
class Jvm
{
public:
  // Forward declarations.
  class ConstructorFinder;
  class MethodFinder;
  class Constructor;
  class MethodSignature;
  class Method;

  // Attempts to injects an embedded JVM. Returns the singleton Jvm
  // instance or an error if the JVM has already been created or
  // injected with a different JavaVM than what was passed.
  Try<Jvm*> inject(
      JavaVM* jvm,
      JNI::Version version,
      bool exceptions);

  // Starts a new embedded JVM with the given -D options. Each option
  // supplied should be of the standard form: '-Dproperty=value'.
  // Returns the singleton Jvm instance or an error if the JVM had
  // already been created. Note that you can only create one JVM
  // instance per process since destructing a JVM has issues, see:
  // http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=4712793.  In
  // addition, most JVM's use signals and couldn't possibly play
  // nicely with one another.
  // TODO(John Sirois): Consider elevating classpath as a top-level
  // JVM configuration parameter since it will likely always need to
  // be specified. Ditto for and non '-X' options.
  static Try<Jvm*> create(
      const std::vector<std::string>& options = std::vector<std::string>(),
      JNI::Version version = JNI::v_1_6,
      bool exceptions = false);

  // Returns true if the JVM has already been created.
  static bool created();

  // Returns the singleton JVM instance, creating it with no options
  // and a default version if necessary.
  static Jvm* get();

  // An opaque class descriptor that can be used to find constructors,
  // methods and fields.
  class Class
  {
  public:
    // Class descriptors for native types (e.g., int, short). Note
    // that this includes 'void' which is not 'java.lang.Void'.
    static const Class VOID;
    static const Class BOOLEAN;
    static const Class BYTE;
    static const Class CHAR;
    static const Class SHORT;
    static const Class INT;
    static const Class LONG;
    static const Class FLAOT;
    static const Class DOUBLE;
    static const Class STRING;

    // A factory for new Java reference type class descriptors given
    // the fully-qualified class name (e.g., 'java/io/File').
    static const Class named(const std::string& name);

    Class(const Class& that);

    // Returns the class of an array of the current class.
    const Class arrayOf() const;

    // Creates a builder that can be used to locate a constructor of
    // this class with Jvm::findConstructor.
    ConstructorFinder constructor() const;

    // Creates a builder that can be used to locate an instance method
    // of this class with Jvm::findMethod.
    MethodFinder method(const std::string& name) const;

  private:
    friend class Jvm;

    Class(const std::string& name, bool native = true);

    std::string signature() const;

    std::string name;
    bool native;
  };


  // A builder that is used to specify a constructor by specifying its
  // parameter list with zero or more calls to
  // ConstructorFinder::parameter.
  class ConstructorFinder
  {
  public:
    // Adds a parameter to the constructor parameter list.
    ConstructorFinder& parameter(const Class& clazz);

  private:
    friend class Class;
    friend class Jvm;

    ConstructorFinder(const Class& clazz);

    const Class clazz;
    std::vector<Class> parameters;
  };


  // An opaque constructor descriptor that can be used to create new
  // instances of a class using Jvm::invokeConstructor.
  class Constructor
  {
  public:
    Constructor(const Constructor& that);

  private:
    friend class Jvm;

    Constructor(const Class& clazz, const jmethodID id);

    const Class clazz;
    const jmethodID id;
  };


  // A builder that is used to specify an instance method by
  // specifying its parameter list with zero or more calls to
  // MethodFinder::parameter and a final call to MethodFinder::returns
  // to get an opaque specification of the method for use with
  // Jvm::findMethod.
  class MethodFinder
  {
  public:
    // Adds a parameter to the method parameter list.
    MethodFinder& parameter(const Class& type);

    // Terminates description of a method by specifying its return type.
    MethodSignature returns(const Class& type) const;

  private:
    friend class Class;

    MethodFinder(const Class& clazz, const std::string& name);

    const Class clazz;
    const std::string name;
    std::vector<Class> parameters;
  };


  // An opaque method specification for use with Jvm::findMethod.
  class MethodSignature
  {
  public:
    MethodSignature(const MethodSignature& that);

  private:
    friend class Jvm;
    friend class MethodFinder;

    MethodSignature(const Class& clazz,
                    const std::string& name,
                    const Class& returnType,
                    const std::vector<Class>& parameters);

    const Class clazz;
    const std::string name;
    const Class returnType;
    std::vector<Class> parameters;
  };


  // An opaque method descriptor that can be used to invoke instance methods
  // using Jvm::invokeMethod.
  class Method
  {
  public:
    Method(const Method& that);

  private:
    friend class Jvm;
    friend class MethodSignature;

    Method(const Class& clazz, const jmethodID id);

    const Class clazz;
    const jmethodID id;
  };


  // An opaque field descriptor that can be used to access fields using
  // methods like Jvm::getStaticField.
  class Field
  {
  public:
    Field(const Field& that);

  private:
    friend class Jvm;

    Field(const Class& clazz, const jfieldID id);

    const Class clazz;
    const jfieldID id;
  };

  // Helper for providing access to static variables in a class. This
  // delays the actual read of the variable in the JVM until the cast
  // operator is invoked to convert from StaticVariable<T> to T! Note
  // that we take the variable name as a template parameter so that we
  // can make cache the field (i.e., do 'static Field field = ...' in
  // 'operator T'). See Level in jvm/org/apache/log4j.hpp for an
  // example.
  // TODO(benh): Make this work for instance variables too (i.e.,
  // StaticVariable -> Variable).
  // TODO(benh): Provide template specialization for primitive
  // types (e.g., StaticVariable<int>, StaticVariable<short>,
  // StaticVariable<std::string>).
  template <typename T, const char* name>
  class StaticVariable
  {
  public:
    StaticVariable(const Class& _clazz)
      : clazz(_clazz)
    {
      // Check that T extends Object.
      { T* t = NULL; java::lang::Object* o = t; (void) o; }
    }

    operator T () const
    {
      // Note that we actually look up the field lazily (upon first
      // invocation operator) so that we don't possibly create the JVM
      // too early.
      static Field field = Jvm::get()->findStaticField(clazz, name);
      T t;
      t.object = Jvm::get()->getStaticField<jobject>(field);
      return t;
    }

  private:
    const Class clazz;
  };

  jstring string(const std::string& s);

  Constructor findConstructor(const ConstructorFinder& finder);
  Method findMethod(const MethodSignature& signature);
  Method findStaticMethod(const MethodSignature& signature);
  Field findStaticField(const Class& clazz, const std::string& name);

  // TODO(John Sirois): Add "type checking" to variadic method
  // calls. Possibly a way to do this with typelists, type
  // concatenation and unwinding builder inheritance.

  jobject invoke(const Constructor& ctor, ...);

  template <typename T>
  T invoke(const jobject receiver, const Method& method, ...);

  template <typename T>
  T invokeStatic(const Method& method, ...);

  template <typename T>
  T getStaticField(const Field& field);

  // Checks the exception state of an environment.
  void check(JNIEnv* env);

private:
  friend class JNI::Env; // For attaching and detatching.
  friend class java::lang::Object; // For managing global references.
  friend void deleter(); // For deleting the instance 'atexit'.

  Jvm(JavaVM* jvm, JNI::Version version, bool exceptions);
  ~Jvm();

private:
  jobject newGlobalRef(const jobject object);
  void deleteGlobalRef(const jobject object);

  jclass findClass(const Class& clazz);

  jmethodID findMethod(const Jvm::Class& clazz,
                       const std::string& name,
                       const Jvm::Class& returnType,
                       const std::vector<Jvm::Class>& argTypes,
                       bool isStatic);

  template <typename T>
  T invokeV(const jobject receiver, const jmethodID id, va_list args);

  template <typename T>
  T invokeStaticV(const Class& receiver, const jmethodID id, va_list args);

  // Singleton instance.
  static Jvm* instance;

  JavaVM* jvm;
  const JNI::Version version;
  const bool exceptions;
};


template <>
void Jvm::invoke<void>(const jobject receiver, const Method& method, ...);


template <typename T>
T Jvm::invoke(const jobject receiver, const Method& method, ...)
{
  va_list args;
  va_start(args, method);
  const T result = invokeV<T>(receiver, method.id, args);
  va_end(args);
  return result;
}


template <>
void Jvm::invokeStatic<void>(const Method& method, ...);


template <typename T>
T Jvm::invokeStatic(const Method& method, ...)
{
  va_list args;
  va_start(args, method);
  const T result = invokeStaticV<T>(method.clazz, method.id, args);
  va_end(args);
  return result;
}

#endif // __JVM_HPP__
