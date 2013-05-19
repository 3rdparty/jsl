#ifndef __JAVA_IO_HPP__
#define __JAVA_IO_HPP__

#include <jvm.hpp>

#include <java/lang.hpp>

namespace java {
namespace io {

class File : public java::lang::Object
{
public:
  File(const std::string& pathname)
  {
    static Jvm::Constructor constructor = Jvm::get()->findConstructor(
        Jvm::Class::named("java/io/File")
        .constructor()
        .parameter(Jvm::Class::STRING));

    object = Jvm::get()->invoke(constructor, Jvm::get()->string(pathname));
  }

  void deleteOnExit()
  {
    static Jvm::Method method = Jvm::get()->findMethod(
        Jvm::Class::named("java/io/File")
        .method("deleteOnExit")
        .returns(Jvm::Class::VOID));

    Jvm::get()->invoke<void>(object, method);
  }

  bool exists()
  {
    static Jvm::Method method = Jvm::get()->findMethod(
        Jvm::Class::named("java/io/File")
        .method("exists")
        .returns(Jvm::Class::BOOLEAN));

    Jvm::get()->invoke<bool>(object, method);
  }
};

} // namespace io {
} // namespace java {

#endif // __JAVA_IO_HPP__
