#include "threadbuf.h"
#include "bb_comp.hpp"
#include "bb_index.hpp"
#include "bit.hpp"
#include "main.hpp"
#include "hash.hpp"
#include "pos.hpp"
#include "thread.hpp"
#include "var.hpp"
#include "CordovaPluginScanc.h"

namespace scanios
{
  static std::string CMD_EXIT = "scan:exit";

  auto readstdout = [](void *scan) {
    std::streambuf* out = std::cout.rdbuf();

    threadbuf lichbuf(8, 8096);
    std::ostream lichout(&lichbuf);
    std::cout.rdbuf(lichout.rdbuf());
    std::istream lichin(&lichbuf);

    std::string o = "";

    while(o != CMD_EXIT) {
      std::string line;
      std::getline(lichin, line);
      if(line != CMD_EXIT) {
        const char* coutput = line.c_str();
        ScanSendOutput(scan, coutput);
      } else {
        o = CMD_EXIT;
      }
    };

    // Restore output standard
    std::cout.rdbuf(out);

    lichbuf.close();
  };

  std::thread reader;

  void init(void *scan, std::string variant, std::string resourcePath) {
    reader = std::thread(readstdout, scan);
      
    bit::init();
    hash::init();
    pos::init();
    var::init();

    bb::index_init();
    bb::comp_init();

    ml::rand_init(); // after hash keys

    // use fixed hash size
    const int hashSize = 32;
    std::string ttSize = std::to_string(floor(log2(hashSize * 1024 * 1024 / 16)));
    var::set("tt-size", ttSize);

    // store resources directory as datapath
    var::set("datapath", resourcePath);

    // set variant 
    var::set("variant", variant);
    var::update();

    bit::init(); // depends on the variant

    // start input loop on a new thread
    listen_input();
  }

  void cmd(std::string cmd) {
    hub_command(cmd);
  }

  void exit() {
    sync_cout << CMD_EXIT << sync_endl;
    reader.join();
  }
}
