#include "inode_manager.h"
#include <string>

// disk layer -----------------------------------------

disk::disk()
{
  bzero(blocks, sizeof(blocks));
}

void
disk::read_block(blockid_t id, char *buf)
{
  if (id < 0 || id >= BLOCK_NUM || buf == NULL)
    return;

  memcpy(buf, blocks[id], BLOCK_SIZE);
}

void
disk::write_block(blockid_t id, const char *buf)
{
  printf("\tdisk: write block: %d\n", id);
  if (id < 0 || id >= BLOCK_NUM || buf == NULL)
    return;
  memcpy(blocks[id], buf, BLOCK_SIZE);
}

// block layer -----------------------------------------

// Allocate a free disk block.
blockid_t
block_manager::alloc_block()
{
  /*
   * your code goes here.
   * note: you should mark the corresponding bit in block bitmap when alloc.
   * you need to think about which block you can start to be allocated.
   */
  
  blockid_t bid = 0;
  while (bid + IBLOCK(INODE_NUM, BLOCK_NUM) < BLOCK_NUM) {  // bid is data block offset, not real block id
    char buf[BLOCK_SIZE];
    read_block(BBLOCK(bid), buf);

    char mask = 0x80;  // 10000000

    if ((buf[bid/8] & (mask >> (bid%8))) == 0) {

      buf[bid/8] = buf[bid/8] | (mask >> (bid%8));  // alloc
      write_block(BBLOCK(bid), buf);

      return bid + IBLOCK(INODE_NUM, BLOCK_NUM);  // critical!! without which, the return value indicated block will overlay bitmap
    }
    bid++;
  }
  
  printf("\tbm: no free block\n");
  exit(-1);
}

void
block_manager::free_block(uint32_t id)
{
  /* 
   * your code goes here.
   * note: you should unmark the corresponding bit in the block bitmap when free.
   */
  
  if (id < 0 || id >= BLOCK_NUM) {
    printf("\tbm: block id out of range\n");
    return;
  }

  uint32_t bid = id - IBLOCK(INODE_NUM, BLOCK_NUM);  // get data block offset

  char buf[BLOCK_SIZE];
  read_block(BBLOCK(bid), buf);
  unsigned char mask = 0x7f;  // 01111111
  buf[bid/8] &= (mask >> (bid%8));  // free
  write_block(BBLOCK(bid), buf);
}

// The layout of disk should be like this:
// |<-sb->|<-free block bitmap->|<-inode table->|<-data->|
block_manager::block_manager()
{
  d = new disk();

  // format the disk
  sb.size = BLOCK_SIZE * BLOCK_NUM;
  sb.nblocks = BLOCK_NUM;
  sb.ninodes = INODE_NUM;

}

void
block_manager::read_block(uint32_t id, char *buf)
{
  d->read_block(id, buf);
}

void
block_manager::write_block(uint32_t id, const char *buf)
{
  d->write_block(id, buf);
}

// inode layer -----------------------------------------

inode_manager::inode_manager()
{
  bm = new block_manager();
  uint32_t root_dir = alloc_inode(extent_protocol::T_DIR);
  if (root_dir != 1) {
    printf("\tim: error! alloc first inode %d, should be 1\n", root_dir);
    exit(0);
  }
}

/* Create a new file.
 * Return its inum. */
uint32_t
inode_manager::alloc_inode(uint32_t type)
{
  /* 
   * your code goes here.
   * note: the normal inode block should begin from the 2nd inode block.
   * the 1st is used for root_dir, see inode_manager::inode_manager().
   */

  // find an empty block for the new inode
  uint32_t inum = 1;
  while (get_inode(inum) != NULL) {
    inum++;
  }

  // construct the new inode
  struct inode *ino;
  ino = (struct inode*)malloc(sizeof(struct inode));
  ino->type = type;
  ino->size = 0;
  ino->atime = (unsigned int)time(NULL);
  ino->mtime = (unsigned int)time(NULL);
  ino->ctime = (unsigned int)time(NULL);

  // put the new inode into the block found
  put_inode(inum, ino);
  //free(ino);
  return inum;
}

void
inode_manager::free_inode(uint32_t inum)
{
  /* 
   * your code goes here.
   * note: you need to check if the inode is already a freed one;
   * if not, clear it, and remember to write back to disk.
   */
  
  struct inode *ino;
  if ((ino = get_inode(inum)) == NULL) {
    // already freed
    return;
  }

  ino->type = 0;  // free

  // write back to disk
  put_inode(inum, ino);
  free(ino);

  return;
}


/* Return an inode structure by inum, NULL otherwise.
 * Caller should release the memory. */
struct inode* 
inode_manager::get_inode(uint32_t inum)
{
  // ino_disk is local, ino is malloced
  struct inode *ino, *ino_disk;
  char buf[BLOCK_SIZE];

  printf("\tim: get_inode %d\n", inum);

  if (inum < 0 || inum >= INODE_NUM) {
    printf("\tim: inum out of range\n");
    return NULL;
  }

  bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
  // printf("%s:%d\n", __FILE__, __LINE__);

  // 因为 buf 已经被转成 struct inode* 了，所以 + 的步长为 sizeof(struct inode*)，但 lab1 中 IPB 为 1，所以 + 没什么效果
  ino_disk = (struct inode*)buf + inum%IPB;
  if (ino_disk->type == 0) {
    // if an inode's type is 0, it is free
    printf("\tim: inode not exist\n");
    return NULL;
  }

  ino = (struct inode*)malloc(sizeof(struct inode));
  *ino = *ino_disk;

  return ino;
}

void
inode_manager::put_inode(uint32_t inum, struct inode *ino)
{
  char buf[BLOCK_SIZE];
  struct inode *ino_disk;

  printf("\tim: put_inode %d\n", inum);
  if (ino == NULL)
    return;

  bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
  ino_disk = (struct inode*)buf + inum%IPB;
  *ino_disk = *ino;
  bm->write_block(IBLOCK(inum, bm->sb.nblocks), buf);
}

#define MIN(a,b) ((a)<(b) ? (a) : (b))

/* Get all the data of a file by inum. 
 * Return alloced data, should be freed by caller. */
void
inode_manager::read_file(uint32_t inum, char **buf_out, int *size)
{
  /*
   * your code goes here.
   * note: read blocks related to inode number inum,
   * and copy them to buf_Out
   */
  
  struct inode *ino = get_inode(inum);
  *buf_out = (char *)malloc(ino->size);
  int pos = 0;

  if (ino->size == 0) {
    *size = 0;
    ino->atime = (unsigned int)time(NULL);
    put_inode(inum, ino);
    free(ino);
    return;
  }
  if (ino->size <= NDIRECT * BLOCK_SIZE) {
    for (int i = 0; i < (ino->size-1)/BLOCK_SIZE + 1; i++) {
      char buf[BLOCK_SIZE];
      bm->read_block(ino->blocks[i], buf);
      if (i != (ino->size-1)/BLOCK_SIZE) {
        // not last i
        memcpy(*buf_out + pos, buf, BLOCK_SIZE);
        pos += BLOCK_SIZE;
      }
      else {
        // last i
        memcpy(*buf_out + pos, buf, ((ino->size-1) % BLOCK_SIZE) + 1);
        pos += ((ino->size-1) % BLOCK_SIZE) + 1;
      }
    }
  }
  else {
    for (int i = 0; i < NDIRECT; i++) {
      char buf[BLOCK_SIZE];
      bm->read_block(ino->blocks[i], buf);
      memcpy(*buf_out + pos, buf, BLOCK_SIZE);
      pos += BLOCK_SIZE;
    }

    char indirect[BLOCK_SIZE];
    bm->read_block(ino->blocks[NDIRECT], indirect);

    for (int i = 0; i < (ino->size - NDIRECT * BLOCK_SIZE - 1)/BLOCK_SIZE + 1; i++) {
      char buf[BLOCK_SIZE];
      bm->read_block(*((uint32_t *)indirect + i), buf);
      if (i != (ino->size - NDIRECT * BLOCK_SIZE - 1)/BLOCK_SIZE) {
        // not last i
        memcpy(*buf_out + pos, buf, BLOCK_SIZE);
        pos += BLOCK_SIZE;
      }
      else {
        // last i
        memcpy(*buf_out + pos, buf, ((ino->size-1) % BLOCK_SIZE) + 1);
        pos += ((ino->size-1) % BLOCK_SIZE) + 1;
      }
    }
  }
  
  *size = pos;
  ino->atime = (unsigned int)time(NULL);
  put_inode(inum, ino);
  free(ino);
  return;
}

/* alloc/free blocks if needed */
// size as argument cannot be 0, that means you mustn't make a file empty
void
inode_manager::write_file(uint32_t inum, const char *buf, int size)
{
  /*
   * your code goes here.
   * note: write buf to blocks of inode inum.
   * you need to consider the situation when the size of buf 
   * is larger or smaller than the size of original inode
   */
	struct inode *inode = get_inode(inum);

	// get old and new block num
	unsigned int old_blocknum = (inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
	unsigned int new_blocknum = (size + BLOCK_SIZE - 1 ) / BLOCK_SIZE;
	char indirect[BLOCK_SIZE];

  // adjust size, free / alloc
	if (old_blocknum > new_blocknum)
	{
  	// old > new, need free  
		if (new_blocknum > NDIRECT)
		{
		  // NDIRECT < new < old
			bm->read_block(inode->blocks[NDIRECT], indirect); //use indirect blocks
			for (unsigned int i = new_blocknum; i < old_blocknum; i++){
				bm->free_block(*((blockid_t *)indirect + (i - NDIRECT))); //free unused blocks
			}
		}
    else if (new_blocknum <= NDIRECT && old_blocknum > NDIRECT)
    {
      // new <= NDIRECT < old
      bm->read_block(inode->blocks[NDIRECT], indirect);
      for (unsigned int i = NDIRECT; i < old_blocknum; i++){
        bm->free_block(*((blockid_t *)indirect + (i - NDIRECT)));
      }
      bm->free_block(inode->blocks[NDIRECT]);
      for (unsigned int i = new_blocknum; i < NDIRECT; i++){
        bm->free_block(inode->blocks[i]);
      }
    }
    else {
      // new < old <= NDIRECT
      for (unsigned int i = new_blocknum; i < old_blocknum; i++){
        bm->free_block(inode->blocks[i]);
      }
    }
	} 
  else
	{ 
    // old <= new, need alloc
		if (new_blocknum <= NDIRECT) {
      // old <= new <= NDIRECT
			for (unsigned int i = old_blocknum; i < new_blocknum; i++){
				inode->blocks[i] = bm->alloc_block();
			}
		}
		else if (new_blocknum > NDIRECT && old_blocknum <= NDIRECT) {
			// old <= NDIRECT < new
      for (unsigned int i = old_blocknum; i < NDIRECT; i++) {
        inode->blocks[i] = bm->alloc_block();
      }
      inode->blocks[NDIRECT] = bm->alloc_block();
      bzero(indirect, BLOCK_SIZE);
      for (unsigned int i = NDIRECT; i < new_blocknum; i++) {
        *((blockid_t *)indirect + (i - NDIRECT)) = bm->alloc_block();
      }
      bm->write_block(inode->blocks[NDIRECT], indirect);
    }
    else {
      // NDIRECT < old <= new
      bm->read_block(inode->blocks[NDIRECT], indirect);
      for (unsigned int i = old_blocknum; i < new_blocknum; i++){
        *((blockid_t *)indirect + (i - NDIRECT)) = bm->alloc_block();
      }
      bm->write_block(inode->blocks[NDIRECT], indirect);
    }
	}	

	// write to blocks
	char tail_block[BLOCK_SIZE];
	int pos = 0; 
	for (int i = 0; i < NDIRECT && pos < size; i++) {   
    // write blocksize
    if (size - pos > BLOCK_SIZE) {
      bm->write_block(inode->blocks[i], buf+pos); 
      pos += BLOCK_SIZE;
    } 
    else {
      int left_len = size - pos;
      memcpy(tail_block, buf + pos, left_len);
      bm->write_block(inode->blocks[i], tail_block);
      pos += left_len;
    }   
  }   
	
	if (pos < size) {
    bm->read_block(inode->blocks[NDIRECT], indirect);
    for (unsigned int i = 0; i < NINDIRECT && pos < size; i++) {
      blockid_t blockid = *((blockid_t *)indirect + i);
      if (size - pos > BLOCK_SIZE){
          bm->write_block(blockid, buf + pos);
          pos += BLOCK_SIZE;
      } 
      else {
        int left_len = size - pos;
        memcpy(tail_block, buf + pos, left_len);
				bm->write_block(blockid, tail_block);
        pos += left_len;
      }
    }
  }

	inode->size = size;
  inode->atime = (unsigned int)time(NULL);
	inode->mtime = (unsigned int)time(NULL); 
	inode->ctime = (unsigned int)time(NULL);

	put_inode(inum, inode);
	free(inode);
}

void
inode_manager::getattr(uint32_t inum, extent_protocol::attr &a)
{
  /*
   * your code goes here.
   * note: get the attributes of inode inum.
   * you can refer to "struct attr" in extent_protocol.h
   */
  struct inode *ino = get_inode(inum);
  if (ino == NULL) {
    // the inode is free
    a.type = 0;
    return;
  }
  a.size = ino->size;
  a.type = ino->type;
  a.atime = ino->atime;
  a.mtime = ino->mtime;
  a.ctime = ino->ctime;
  
  return;
}

void
inode_manager::remove_file(uint32_t inum)
{
  /*
   * your code goes here
   * note: you need to consider about both the data block and inode of the file
   */
  
  struct inode *ino = get_inode(inum);

  if (ino->size > 0) {
    // not empty file, free blocks
    if (ino->size <= NDIRECT * BLOCK_SIZE) {
      // without indirect block
      for (int i = 0; i < (ino->size-1)/BLOCK_SIZE + 1; i++) {
        bm->free_block(ino->blocks[i]);
      }
    }
    else {
      // need to consider indirect block
      for (int i = 0; i < NDIRECT; i++) {
        bm->free_block(ino->blocks[i]);
      }

      // get the indirect block
      char indirect[BLOCK_SIZE];
      bm->read_block(ino->blocks[NDIRECT], indirect);

      // free indirect blocks
      for (int i = 0; i < (ino->size - NDIRECT * BLOCK_SIZE - 1)/BLOCK_SIZE + 1; i++) {
        bm->free_block(*((uint32_t *)indirect + i));
      }
      bm->free_block(ino->blocks[NDIRECT]);
    }
  }

  // free inode
  free_inode(inum);
  
  return;
}
