// RPC stubs for clients to talk to extent_server

#include "extent_client_cache.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int extent_client_cache::last_port = 12345;

extent_client_cache::extent_client_cache(std::string dst) : extent_client(dst)
{
  srand(time(NULL)^last_port);
  rlock_port = ((rand()%32000) | (0x1 << 10));
  const char *hname;
  // VERIFY(gethostname(hname, 100) == 0);
  hname = "127.0.0.1";
  std::ostringstream host;
  host << hname << ":" << rlock_port;
  clientId = host.str();
  last_port = rlock_port;
  rpcs *rlsrpc = new rpcs(rlock_port);
  rlsrpc->reg(rextent_protocol::update_handler, this, &extent_client_cache::update_handler);
  rlsrpc->reg(rextent_protocol::remove_handler, this, &extent_client_cache::remove_handler);
}

extent_protocol::status
extent_client_cache::create(uint32_t type, extent_protocol::extentid_t &id)
{
  return extent_protocol::OK;
}

extent_protocol::status 
extent_client_cache::createFileAndDir(uint32_t type, extent_protocol::extentid_t &eid, 
    const char *name, extent_protocol::extentid_t parent)
{
  cl->call(extent_protocol::create, type, clientId, parent, std::string(name), eid);
  return extent_protocol::OK;
}

extent_protocol::status 
extent_client_cache::createSymLink(extent_protocol::extentid_t &eid, 
    const char *name, const char *link, extent_protocol::extentid_t parent)
{
  cl->call(extent_protocol::createSymLink, clientId, parent, std::string(name), std::string(link), eid);
  return extent_protocol::OK;
}

extent_protocol::status
extent_client_cache::get(extent_protocol::extentid_t eid, std::string &buf)
{
  if (data_cache.find(eid) == data_cache.end()) {
    cl->call(extent_protocol::get, clientId, eid, buf);
    data_cache[eid] = buf;
  }
  else {
    buf = data_cache[eid];
  }
  return extent_protocol::OK;
}

extent_protocol::status
extent_client_cache::getattr(extent_protocol::extentid_t eid, extent_protocol::attr &a)
{
  if (attr_cache.find(eid) == attr_cache.end()) {
    cl->call(extent_protocol::getattr, clientId, eid, a);
    attr_cache[eid] = a;
  }
  else {
    a = attr_cache[eid];
  }
  return extent_protocol::OK;
}

extent_protocol::status
extent_client_cache::put(extent_protocol::extentid_t eid, std::string buf)
{
  extent_protocol::status ret = extent_protocol::OK;
  int r;
  ret = cl->call(extent_protocol::put, clientId, eid, buf, r);
  return ret;
}

extent_protocol::status
extent_client_cache::remove(extent_protocol::extentid_t eid, extent_protocol::extentid_t parent, const char *name)
{
  int r;
  cl->call(extent_protocol::remove, clientId, parent, std::string(name), eid, r);
  return extent_protocol::OK;
}

extent_protocol::status 
extent_client_cache::remove_handler(extent_protocol::extentid_t eid, int &)
{
  data_cache.erase(eid);
  attr_cache.erase(eid);
  return extent_protocol::OK;
}

extent_protocol::status 
extent_client_cache::update_handler(extent_protocol::extentid_t eid, extent_protocol::attr a,
        std::string data, int &)
{
  data_cache[eid] = data;
  attr_cache[eid] = a;
  return extent_protocol::OK;
}