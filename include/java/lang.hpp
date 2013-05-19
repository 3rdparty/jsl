#ifndef __JAVA_LANG_HPP__
#define __JAVA_LANG_HPP__

#include <jvm.hpp>

namespace java {
namespace lang {

// Base class for all JVM objects. This object "stores" the underlying
// global reference and performs the appropriate reference operations
// across copies and assignments.
class Object
{
public:
  Object() : object(NULL) {}

  Object(jobject _object)
    : object(Jvm::get()->newGlobalRef(_object)) {}

  Object(const Object& that)
  {
    object = Jvm::get()->newGlobalRef(that.object);
  }

  ~Object()
  {
    if (object != NULL) {
      Jvm::get()->deleteGlobalRef(object);
    }
  }

  Object& operator = (const Object& that)
  {
    if (object != NULL) {
      Jvm::get()->deleteGlobalRef(object);
    }
    object = Jvm::get()->newGlobalRef(that.object);
    return *this;
  }

  operator jobject () const
  {
    return object;
  }

protected:
  friend void Jvm::check(JNIEnv* env); // For manipulating object.

  jobject object;
};


class Throwable : public Object
{
public:
  Throwable(const std::string& message)
  {
    static Jvm::Constructor constructor = Jvm::get()->findConstructor(
        Jvm::Class::named("java/lang/Throwable")
        .constructor()
        .parameter(Jvm::Class::STRING));

    object = Jvm::get()->invoke(constructor, Jvm::get()->string(message));
  }

private:
  friend void Jvm::check(JNIEnv* env); // For constructing default instances.

  Throwable() {}
};

} // namespace lang {
} // namespace java {

#endif // __JAVA_LANG_HPP__
