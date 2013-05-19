#include <glog/logging.h>

#include <string>

#include <stout/os.hpp>
#include <stout/try.hpp>

#include <java/io.hpp>


int main(int argc, char** argv)
{
  FLAGS_logtostderr = true; // Log to stderr instead of files by default.
  google::InitGoogleLogging(argv[0]);

  Try<std::string> directory = os::mkdtemp();
  CHECK(directory.isSome());

  java::io::File file(directory.get());

  file.deleteOnExit();

  return file.exists() ? 0 : -1;
}
