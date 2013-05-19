#ifndef __ORG_APACHE_ZOOKEEPER_HPP__
#define __ORG_APACHE_ZOOKEEPER_HPP__

#include <jvm.hpp>

#include <java/io.hpp>
#include <java/lang.hpp>
#include <java/net.hpp>

// Package 'org.apache.zookeeper.persistence'.

namespace org {
namespace apache {
namespace zookeeper {
namespace persistence {

class FileTxnSnapLog : public java::lang::Object
{
public:
  FileTxnSnapLog(const java::io::File& dataDir,
                 const java::io::File& snapDir)
  {
    static Jvm::Constructor constructor = Jvm::get()->findConstructor(
        Jvm::Class::named(
            "org/apache/zookeeper/server/persistence/FileTxnSnapLog")
        .constructor()
        .parameter(Jvm::Class::named("java/io/File"))
        .parameter(Jvm::Class::named("java/io/File")));

    object = Jvm::get()->invoke(
        constructor, (jobject) dataDir, (jobject) snapDir);
  }
};

} // namespace persistence {
} // namespace zookeeper {
} // namespace apache {
} // namespace org {


// Package 'org.apache.zookeeper.server'.

namespace org {
namespace apache {
namespace zookeeper {
namespace server {

class ZooKeeperServer : public java::lang::Object
{
public:
  class DataTreeBuilder : public java::lang::Object
  {
  protected:
    DataTreeBuilder() {} // Interface, necessary for subclasses.
  };

  class BasicDataTreeBuilder : public DataTreeBuilder
  {
  public:
    BasicDataTreeBuilder()
    {
      static Jvm::Constructor constructor = Jvm::get()->findConstructor(
          Jvm::Class::named(
              "org/apache/zookeeper/server/ZooKeeperServer$BasicDataTreeBuilder")
          .constructor());

      object = Jvm::get()->invoke(constructor);
    }
  };

  ZooKeeperServer(const persistence::FileTxnSnapLog& txnLogFactory,
                  const DataTreeBuilder& treeBuilder)
  {
    static Jvm::Constructor constructor = Jvm::get()->findConstructor(
        Jvm::Class::named("org/apache/zookeeper/server/ZooKeeperServer")
        .constructor()
        .parameter(
            Jvm::Class::named(
                "org/apache/zookeeper/server/persistence/FileTxnSnapLog"))
        .parameter(
            Jvm::Class::named(
                "org/apache/zookeeper/server/ZooKeeperServer$DataTreeBuilder")));

    object = Jvm::get()->invoke(
        constructor, (jobject) txnLogFactory, (jobject) treeBuilder);
  }

  int getClientPort()
  {
    static Jvm::Method method = Jvm::get()->findMethod(
        Jvm::Class::named("org/apache/zookeeper/server/ZooKeeperServer")
        .method("getClientPort")
        .returns(Jvm::Class::INT));

    return Jvm::get()->invoke<int>(object, method);
  }

  void closeSession(int64_t sessionId)
  {
    static Jvm::Method method = Jvm::get()->findMethod(
        Jvm::Class::named("org/apache/zookeeper/server/ZooKeeperServer")
        .method("closeSession")
        .parameter(Jvm::Class::LONG)
        .returns(Jvm::Class::VOID));

    Jvm::get()->invoke<void>(object, method, sessionId);
  }
};


class NIOServerCnxn : public java::lang::Object
{
public:
  class Factory : public java::lang::Object // TODO(benh): Extends Thread.
  {
  public:
    Factory(const java::net::InetSocketAddress& addr)
    {
      static Jvm::Constructor constructor = Jvm::get()->findConstructor(
          Jvm::Class::named(
              "org/apache/zookeeper/server/NIOServerCnxn$Factory")
          .constructor()
          .parameter(Jvm::Class::named("java/net/InetSocketAddress")));

      object = Jvm::get()->invoke(constructor, (jobject) addr);
    }

    void startup(const ZooKeeperServer& zks)
    {
      static Jvm::Method method = Jvm::get()->findMethod(
          Jvm::Class::named(
              "org/apache/zookeeper/server/NIOServerCnxn$Factory")
          .method("startup")
          .parameter(Jvm::Class::named(
                         "org/apache/zookeeper/server/ZooKeeperServer"))
          .returns(Jvm::Class::VOID));

      Jvm::get()->invoke<void>(object, method, (jobject) zks);
    }

    bool isAlive()
    {
      static Jvm::Method method = Jvm::get()->findMethod(
          Jvm::Class::named(
              "org/apache/zookeeper/server/NIOServerCnxn$Factory")
          .method("isAlive")
          .returns(Jvm::Class::BOOLEAN));

      return Jvm::get()->invoke<bool>(object, method);
    }

    void shutdown()
    {
      static Jvm::Method method = Jvm::get()->findMethod(
          Jvm::Class::named(
              "org/apache/zookeeper/server/NIOServerCnxn$Factory")
          .method("shutdown")
          .returns(Jvm::Class::VOID));

      Jvm::get()->invoke<void>(object, method);
    }
  };

private:
  NIOServerCnxn() {} // No default constructors.
};

} // namespace server {
} // namespace zookeeper {
} // namespace apache {
} // namespace org {

#endif // __ORG_APACHE_ZOOKEEPER_HPP__
