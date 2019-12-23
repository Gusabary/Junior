// extent client interface.

#ifndef extent_client_cache_h
#define extent_client_cache_h

#include <string>
#include <map>
#include "extent_protocol.h"
#include "extent_server.h"
#include "extent_client.h"

class extent_client_cache : public extent_client {
 private:
  // rpcc *cl;
  int rlock_port;
  std::string hostname;
  std::string clientId;
  // cache for get
  std::map<extent_protocol::extentid_t, std::string> data_cache;
  // cache for getattr
  std::map<extent_protocol::extentid_t, extent_protocol::attr> attr_cache;
  int option;
  extent_protocol::extentid_t optionId;

public:
  static int last_port;
  extent_client_cache(std::string dst);

  // eid is inode offset (inum)
  extent_protocol::status create(uint32_t type, extent_protocol::extentid_t &eid);
  extent_protocol::status createFileAndDir(uint32_t type, extent_protocol::extentid_t &eid, 
            const char *name, extent_protocol::extentid_t parent);
  extent_protocol::status createSymLink(extent_protocol::extentid_t &eid, 
            const char *name, const char *link, extent_protocol::extentid_t parent);     

  // eid is inum
  extent_protocol::status get(extent_protocol::extentid_t eid, std::string &buf);

  // eid is inum
  extent_protocol::status getattr(extent_protocol::extentid_t eid, extent_protocol::attr &a);

  // eid is inum
  extent_protocol::status put(extent_protocol::extentid_t eid, std::string buf);

  // eid is inum
  extent_protocol::status remove(extent_protocol::extentid_t eid, extent_protocol::extentid_t parent, const char *name);

  extent_protocol::status remove_handler(extent_protocol::extentid_t eid, int &);
  
  extent_protocol::status update_handler(extent_protocol::extentid_t eid, extent_protocol::attr a,
        std::string data, int &);
};

#endif 

