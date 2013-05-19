#ifndef __JAVA_NET_HPP__
#define __JAVA_NET_HPP__

#include <jvm.hpp>

#include <java/lang.hpp>

namespace java {
namespace net {

class SocketAddress : public java::lang::Object
{
protected:
  SocketAddress() {} // Abstract class, necessary for subclasses.
};


class InetSocketAddress : public SocketAddress
{
public:
  InetSocketAddress(int port)
  {
    static Jvm::Constructor constructor = Jvm::get()->findConstructor(
        Jvm::Class::named("java/net/InetSocketAddress")
        .constructor()
        .parameter(Jvm::Class::INT));

    object = Jvm::get()->invoke(constructor, port);
  }
};

} // namespace net {
} // namespace java {

#endif // __JAVA_NET_HPP__
