//
//  blockdevice.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 09.10.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// DO NOT EDIT THIS FILE!!!

#include <cstdlib>
#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "macros.h"

#include "blockdevice.h"

#undef DEBUG

BlockDevice::BlockDevice(u_int32_t blockSize) {
    assert(blockSize % 512 == 0);
    this->blockSize = blockSize;
}

void BlockDevice::resize(u_int32_t blockSize) {
    assert(blockSize % 512 == 0);
    this->blockSize = blockSize;
}

int BlockDevice::create(const char *path) {

    int ret = 0;

    // Open Container file
    contFile = ::open(path, O_EXCL | O_RDWR | O_CREAT, 0666);
    if (contFile < 0) {
        if (errno == EEXIST) {
            // file already exists, we must open & truncate
            LOG("WARNING: container file already exists, truncating")
            contFile = ::open(path, O_EXCL | O_RDWR | O_TRUNC);
        }

        if (contFile < 0) {
            LOG("ERROR: unable to create container file");
            ret = -errno;
        }
    }

    this->size = 0;

    return ret;
}

int BlockDevice::open(const char *path) {

    int ret = 0;

    // Open Container file
    contFile = ::open(path, O_EXCL | O_RDWR);
    if (contFile < 0) {
        if (errno == ENOENT)
                LOG("ERROR: container file does not exists");
        else
                LogF("ERROR: unknown error %d", errno);

        ret = -errno;

    } else {

        // read file stats
        struct stat st;
        if (fstat(contFile, &st) < 0) {
            LOG("ERROR: fstat returned -1");
            ret = -errno;
        } else {

            // get file size
            if (st.st_size > INT32_MAX) {
                LOG("ERROR: file to large");
                ret = -EFBIG;
            } else
                this->size = (uint32_t) st.st_size;
        }
    }

    return ret;
}


int BlockDevice::close() {

    int ret = 0;

    if (::close(this->contFile) < 0)
        ret = -errno;

    return ret;
}

// this method returns 0 if successful, -errno otherwise
int BlockDevice::read(u_int32_t blockNo, char *buffer) {
#ifdef DEBUG
    fprintf(stderr, "BlockDevice: Reading block %d\n", blockNo);
#endif
    off_t pos = (blockNo) * this->blockSize;
    if (lseek(this->contFile, pos, SEEK_SET) != pos)
        return -errno;

    int size = (this->blockSize);
    if (::read(this->contFile, buffer, size) != size)
        return -errno;

    return 0;
}

// this method returns 0 if successful, -errno otherwise
int BlockDevice::write(u_int32_t blockNo, char *buffer) {
#ifdef DEBUG
    fprintf(stderr, "BlockDevice: Writing block %d\n", blockNo);
#endif
    off_t pos = (blockNo) * this->blockSize;
    if (lseek(this->contFile, pos, SEEK_SET) != pos)
        return -errno;

    int __size = (this->blockSize);
    if (::write(this->contFile, buffer, __size) != __size)
        return -errno;

    return 0;
}

uint32_t BlockDevice::getSize() {

    // update size from file stats
    struct stat st;
    if (fstat(contFile, &st) < 0) {
        LOG("ERROR: fstat returned -1");
    }

    if (st.st_size > UINT32_MAX)
            LOG("ERROR: file to large");
    this->size = (uint32_t) st.st_size;

    return this->size;
}


