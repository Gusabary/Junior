// this is the extent server

#ifndef extent_server_cache_h
#define extent_server_cache_h

#include <string>
#include <map>
#include <set>
#include "extent_protocol.h"
#include "inode_manager.h"

class extent_server_cache {
 protected:
#if 0
  typedef struct extent {
    std::string data;
    struct extent_protocol::attr attr;
  } extent_t;
  std::map <extent_protocol::extentid_t, extent_t> extents;
#endif
  inode_manager *im;
  // store client IDs
  std::set<std::string> clients;
  // cache for get
  std::map<extent_protocol::extentid_t, std::string> data_cache;
  // cache for getattr
  std::map<extent_protocol::extentid_t, extent_protocol::attr> attr_cache;

  std::string filename(unsigned long long inum);
  void update_cache(extent_protocol::extentid_t id);
  void broadcast(extent_protocol::extentid_t id, bool isRemove=false);

public:
  extent_server_cache();

  int create(uint32_t type, std::string clientId, extent_protocol::extentid_t parent,
      std::string name, extent_protocol::extentid_t &id);
  int createSymLink(std::string clientId, extent_protocol::extentid_t parent,
      std::string name, std::string link, extent_protocol::extentid_t &id);
  int put(std::string clientId, extent_protocol::extentid_t id, std::string buf, int &);
  int get(std::string clientId, extent_protocol::extentid_t id, std::string &);
  int getattr(std::string clientId, extent_protocol::extentid_t id, extent_protocol::attr &);
  int remove(std::string clientId, extent_protocol::extentid_t parent, std::string name,
      extent_protocol::extentid_t id, int &);
};

#endif 







