// the extent server implementation

#include "extent_server_cache.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "handle.h"

extent_server_cache::extent_server_cache() 
{
  im = new inode_manager();
}

std::string
extent_server_cache::filename(unsigned long long inum)
{
    std::ostringstream ost;
    ost << inum;
    return ost.str();
}

void extent_server_cache::update_cache(extent_protocol::extentid_t id)
{
  id &= 0x7fffffff;
  int size = 0;
  char *cbuf = NULL;

  std::string buf;
  im->read_file(id, &cbuf, &size);
  if (size == 0)
    buf = "";
  else {
    buf.assign(cbuf, size);
    free(cbuf);
  }

  extent_protocol::attr attr;
  memset(&attr, 0, sizeof(attr));
  im->getattr(id, attr);

  data_cache[id] = buf;
  attr_cache[id] = attr;

  broadcast(id);
}

void extent_server_cache::broadcast(extent_protocol::extentid_t id, bool isRemove)
{
  for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); it++) {
    // printf("\tbroadcast to %s\n", it->c_str());
    handle h(*it);
    if (h.safebind()) {
      int r;
      if (isRemove)
        h.safebind()->call(rextent_protocol::remove_handler, id, r);
      else
        h.safebind()->call(rextent_protocol::update_handler, id, attr_cache[id], data_cache[id], r);
    }
  }
}

int extent_server_cache::get(std::string clientId, extent_protocol::extentid_t id, std::string &data)
{
  data = data_cache[id];
  return extent_protocol::OK;
}

int extent_server_cache::getattr(std::string clientId, extent_protocol::extentid_t id, extent_protocol::attr &a)
{
  a = attr_cache[id];
  return extent_protocol::OK;
}


int extent_server_cache::create(uint32_t type, std::string clientId, extent_protocol::extentid_t parent,
      std::string name, extent_protocol::extentid_t &id)
{
  // alloc a new inode and return inum
  id = im->alloc_inode(type);
  std::string buf = data_cache[parent];
  buf.append(name + ":" + filename(id) + "/");
  im->write_file(parent, buf.c_str(), buf.size());

  update_cache(parent);
  update_cache(id);

  clients.insert(clientId);
  return extent_protocol::OK;
}

int extent_server_cache::createSymLink(std::string clientId, extent_protocol::extentid_t parent,
      std::string name, std::string link, extent_protocol::extentid_t &id)
{
  id = im->alloc_inode(extent_protocol::T_SYMLINK);
  im->write_file(id, link.c_str(), link.size());

  std::string buf = data_cache[parent];
  buf.append(std::string(name) + ":" + filename(id) + "/");
  im->write_file(parent, buf.c_str(), buf.size());

  update_cache(parent);
  update_cache(id);

  clients.insert(clientId);
  return extent_protocol::OK;
}

int extent_server_cache::put(std::string clientId, extent_protocol::extentid_t id, 
      std::string buf, int &)
{
  id &= 0x7fffffff;
  im->write_file(id, buf.c_str(), buf.size());

  update_cache(id);
  clients.insert(clientId);
  return extent_protocol::OK;
}

int extent_server_cache::remove(std::string clientId, extent_protocol::extentid_t parent, std::string name,
      extent_protocol::extentid_t id, int &)
{
  id &= 0x7fffffff;
  im->remove_file(id);

  std::string buf = data_cache[parent];
  int erase_start = buf.find(name);
  int erase_after = buf.find('/', erase_start);
  buf.erase(erase_start, erase_after - erase_start + 1);
  im->write_file(parent, buf.c_str(), buf.size());

  update_cache(parent);
  data_cache.erase(id);
  attr_cache.erase(id);
  broadcast(id, true);
  clients.insert(clientId);
  return extent_protocol::OK;
}

