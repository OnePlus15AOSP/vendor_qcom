/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#define LOG_TAG  "LIBVMMEM"
#define PRI_INFO " I "
#define PRI_ERROR " E "
#define LOG(pr, fmt, arg...) syslog (LOG_NOTICE, pr LOG_TAG fmt, ##arg)

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "vmmem.h"
#include <syslog.h>

#if __has_include (<linux/qti_virtio_mem.h>)
#include <linux/qti_virtio_mem.h>
#endif

#if __has_include (<linux/mem-buf.h>)
#include <linux/mem-buf.h>
#else
struct acl_entry {
	__u32 vmid;
	__u32 perms;
};
#endif

#ifndef TEMP_FAILURE_RETRY
/* Used to retry syscalls that can return EINTR. */
#define TEMP_FAILURE_RETRY(exp) ({     \
typeof (exp) _rc;                      \
do {                                   \
    _rc = (exp);                       \
} while (_rc == -1 && errno == EINTR); \
_rc; })
#endif

#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

static constexpr char kMembufPath[] = "/dev/membuf";
static constexpr char kMembufVmPath[] = "/dev/mem_buf_vm/";
#if defined(QTI_VIRTIO_MEM_IOC_HINT_CREATE) && !defined(NO_MEMORY_HINT)
static constexpr char kVirtioMemPath[] = "/dev/qti_virtio_mem";
#endif

VmMem::VM::~VM()
{
    if (fd > 0)
        close(fd);
}

#ifdef MEM_BUF_IOC_EXCLUSIVE_OWNER
int VmMem::IsExclusiveOwnerDmabuf(int fd, bool &is_exclusive_owner)
{
    int mem_buf_fd;
    struct mem_buf_exclusive_owner_ioctl_arg get_ownership = { };
    int ret;

    mem_buf_fd = TEMP_FAILURE_RETRY(open(kMembufPath, O_RDONLY | O_CLOEXEC));
    if (mem_buf_fd < 0){
        LOG(PRI_ERROR, "Unable to open %s errno: %d", kMembufPath, errno);
        return mem_buf_fd;
    }

    get_ownership.dma_buf_fd = fd;

    ret = ioctl(mem_buf_fd, MEM_BUF_IOC_EXCLUSIVE_OWNER, &get_ownership);
    if (ret) {
        LOG(PRI_ERROR, "ioctl(MEM_BUF_IOC_EXCLUSIVE_OWNER) failed, errno: %d", errno);
	goto put_mem_buf_fd;
    }

    is_exclusive_owner = get_ownership.is_exclusive_owner ? true : false;

put_mem_buf_fd:
    close(mem_buf_fd);
    return ret;
}
#else
int VmMem::IsExclusiveOwnerDmabuf(int fd, bool &is_exclusive_owner)
{
    (void)fd;
    (void)is_exclusive_owner;

    LOG(PRI_ERROR, "IsExclusiveOwnerDmabuf() NOT SUPPORTED.");

    return -ENOTTY;
}
#endif

VmHandle VmMem::FindVmByName(const std::string& name)
{
    if (!have_dma_buf_heaps)
        return 0;

    for (auto i: handle_map) {
        auto vm = i.second;

        if (vm->name == name)
            return vm->handle;
    }

    std::string path = kMembufVmPath + name;
    int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_RDONLY | O_CLOEXEC));
    if (fd < 0) {
        LOG(PRI_ERROR, "Unable to open %s, errno: %d", path.c_str(), errno);
        return -errno;
    }

    std::shared_ptr<VM> tmp = std::make_shared<VM>();
    if (tmp == nullptr) {
        LOG(PRI_ERROR,  "Unable to get VmHandle");
        close(fd);
        return -1;
    }

    tmp->name = name;
    tmp->id = fd;
    tmp->fd = fd;
    tmp->handle = fd;
    handle_map.insert(std::make_pair(tmp->handle, tmp));
    return tmp->handle;
}

int VmMem::PopulateAcl(struct acl_entry *acl, uint32_t nr_acl_entries,
    const VmPerm& vmperms)
{
    if (vmperms.size() > nr_acl_entries) {
        LOG(PRI_ERROR, "Invalid number of acl_entries %zu", (vmperms.size()));
        return -EINVAL;
    }

    int j = 0;
    for (auto i : vmperms)
    {
        VmHandle handle = i.first;
        uint32_t perms = i.second;

        auto iter = handle_map.find(handle);
        if (iter == handle_map.end()) {
            LOG(PRI_ERROR, "Invalid VmHandle %d", handle);
            return -EINVAL;
        }
        auto vm = iter->second;
        acl[j].vmid = vm->id;
        acl[j].perms = perms;
        j++;
    }
    return 0;
}

int VmMem::LendDmabuf(int fd, const VmPerm& perms,
            int64_t *memparcel_hdl)
{
    /*
     * When ion is enabled, this function should be a no-op.
     * Secure memory is created via ion APIs instead.
     */
    if (!have_dma_buf_heaps)
        return 0;

    return LendDmabufIoctl(fd, perms, memparcel_hdl);
}

#ifdef MEM_BUF_IOC_LEND
int VmMem::LendDmabufIoctl(int fd, const VmPerm& dst_perms,
            int64_t *memparcel_hdl)
{
    struct mem_buf_lend_ioctl_arg arg = {};
    struct acl_entry acl[MEM_BUF_MAX_NR_ACL_ENTS];
    int ret;
    __u64 __memparcel_hdl;

    ret = PopulateAcl(acl, MEM_BUF_MAX_NR_ACL_ENTS, dst_perms);
    if (ret)
        return ret;

    arg.dma_buf_fd = fd;
    arg.nr_acl_entries = dst_perms.size();
    arg.acl_list = (__u64)&acl;
    arg.memparcel_hdl = (__u64)&__memparcel_hdl;

    ret = ioctl(mem_buf_fd, MEM_BUF_IOC_LEND, &arg);
    if (ret) {
        LOG(PRI_ERROR, "ioctl(MEM_BUF_IOC_LEND) failed, errno: %d", errno);
        return ret;
    }

    if (memparcel_hdl != nullptr)
        *memparcel_hdl = arg.memparcel_hdl;
    return 0;
}
#else
int VmMem::LendDmabufIoctl(int fd, const VmPerm& dst_perms,
            int64_t *memparcel_hdl)
{
    (void)fd;
    (void)dst_perms;
    (void)memparcel_hdl;

    LOG(PRI_ERROR, "LendDmabufIoctl() NOT SUPPORTED.");

    return -ENOTTY;
}
#endif

int VmMem::ShareDmabuf(int fd, const VmPerm& perms,
            int64_t *memparcel_hdl)
{
    /*
     * When ion is enabled, this function should be a no-op.
     * Secure memory is created via ion APIs instead.
     */
    if (!have_dma_buf_heaps)
        return 0;

    return ShareDmabufIoctl(fd, perms, memparcel_hdl);
}

#ifdef MEM_BUF_IOC_SHARE
int VmMem::ShareDmabufIoctl(int fd, const VmPerm& dst_perms,
            int64_t *memparcel_hdl)
{
    struct mem_buf_share_ioctl_arg arg = {};
    struct acl_entry acl[MEM_BUF_MAX_NR_ACL_ENTS];
    int ret;
    __u64 __memparcel_hdl;

    ret = PopulateAcl(acl, MEM_BUF_MAX_NR_ACL_ENTS, dst_perms);
    if (ret)
        return ret;

    arg.dma_buf_fd = fd;
    arg.nr_acl_entries = dst_perms.size();
    arg.acl_list = (__u64)&acl;
    arg.memparcel_hdl = (__u64)&__memparcel_hdl;

    ret = ioctl(mem_buf_fd, MEM_BUF_IOC_SHARE, &arg);
    if (ret) {
        LOG(PRI_ERROR, "ioctl(MEM_BUF_IOC_SHARE) failed, errno: %d", errno);
        return ret;
    }

    if (memparcel_hdl != nullptr)
        *memparcel_hdl = arg.memparcel_hdl;
    return 0;
}
#else
int VmMem::ShareDmabufIoctl(int fd, const VmPerm& dst_perms,
            int64_t *memparcel_hdl)
{
    (void)fd;
    (void)dst_perms;
    (void)memparcel_hdl;
    LOG(PRI_ERROR, "ShareDmabufIoctl() NOT SUPPORTED.");

    return -ENOTTY;
}
#endif

int VmMem::RetrieveDmabuf(VmHandle owner, const VmPerm& perms,
        int64_t memparcel_hdl)
{
    return RetrieveDmabufIoctl(owner, perms, memparcel_hdl);
}

#ifdef MEM_BUF_IOC_RETRIEVE
int VmMem::RetrieveDmabufIoctl(VmHandle owner, const VmPerm& perms, int64_t memparcel_hdl)
{
    struct mem_buf_retrieve_ioctl_arg arg = {};
    struct acl_entry acl[MEM_BUF_MAX_NR_ACL_ENTS];
    int ret;
    __u32 owner_fd;

    auto iter = handle_map.find(owner);
    if (iter == handle_map.end()) {
        LOG(PRI_ERROR, "Invalid owner VmHandle %d", owner);
        return -EINVAL;
    }
    owner_fd = iter->second->id;

    ret = PopulateAcl(acl, MEM_BUF_MAX_NR_ACL_ENTS, perms);
    if (ret)
        return ret;

    arg.sender_vm_fd = owner_fd;
    arg.memparcel_hdl = memparcel_hdl;
    arg.nr_acl_entries = perms.size();
    arg.acl_list = (__u64)&acl;
    arg.fd_flags = O_RDWR | O_CLOEXEC;

    ret = ioctl(mem_buf_fd, MEM_BUF_IOC_RETRIEVE, &arg);
    if (ret) {
        LOG(PRI_ERROR, "ioctl(MEM_BUF_IOC_RETRIEVE) failed, errno: %d", errno);
        return ret;
    }

    return arg.dma_buf_import_fd;
}
#else
int VmMem::RetrieveDmabufIoctl(VmHandle owner, const VmPerm& perms,
        int64_t memparcel_hdl)
{
    (void)owner;
    (void)perms;
    (void)memparcel_hdl;

    LOG(PRI_ERROR, "RetrieveDmabufIoctl() NOT SUPPORTED.");

    return -ENOTTY;
}
#endif

int VmMem::ReclaimDmabuf(int fd, int64_t memparcel_hdl)
{
    return ReclaimDmabufIoctl(fd, memparcel_hdl);
}

#ifdef MEM_BUF_IOC_RECLAIM
int VmMem::ReclaimDmabufIoctl(int fd, int64_t memparcel_hdl)
{
    struct mem_buf_reclaim_ioctl_arg arg = {};
    int ret;

    arg.memparcel_hdl = memparcel_hdl;
    arg.dma_buf_fd = fd;

    ret = ioctl(mem_buf_fd, MEM_BUF_IOC_RECLAIM, &arg);
    if (ret)
        LOG(PRI_ERROR, "ioctl(MEM_BUF_IOC_RECLAIM) failed, errno: %d", errno);

    return ret;
}
#else
int VmMem::ReclaimDmabufIoctl(int fd, int64_t memparcel_hdl)
{
    (void)fd;
    (void)memparcel_hdl;

    LOG(PRI_ERROR, "ReclaimDmabufIoctl() NOT SUPPORTED.");

    return -ENOTTY;
}
#endif

int VmMem::RemoteAllocDmabuf(uint64_t size, const VmPerm& perms,
        const std::string& src_dma_heap_name,
        const std::string& dst_dma_heap_name)
{
    return RemoteAllocDmabufIoctl(size, perms, src_dma_heap_name,
        dst_dma_heap_name);
}

#if defined(MEM_BUF_IOC_ALLOC) && defined(MEM_BUF_DMAHEAP_MEM_TYPE)
int VmMem::RemoteAllocDmabufIoctl(uint64_t size, const VmPerm& dst_perms,
        const std::string& src_dma_heap_name,
        const std::string& dst_dma_heap_name)
{
    struct mem_buf_alloc_ioctl_arg arg = {};
    struct acl_entry acl[MEM_BUF_MAX_NR_ACL_ENTS];
    int ret;
    struct mem_buf_dmaheap_data src_heap = {};
    struct mem_buf_dmaheap_data dst_heap = {};

    ret = PopulateAcl(acl, MEM_BUF_MAX_NR_ACL_ENTS, dst_perms);
    if (ret)
        return ret;

    src_heap.heap_name = (__u64)src_dma_heap_name.c_str();
    dst_heap.heap_name = (__u64)dst_dma_heap_name.c_str();

    arg.size = size;
    arg.acl_list = (__u64)&acl;
    arg.nr_acl_entries = dst_perms.size();
    arg.src_mem_type = MEM_BUF_DMAHEAP_MEM_TYPE;
    arg.src_data = (__u64)&src_heap;
    arg.dst_mem_type = MEM_BUF_DMAHEAP_MEM_TYPE;
    arg.dst_data = (__u64)&dst_heap;

    ret = ioctl(mem_buf_fd, MEM_BUF_IOC_ALLOC, &arg);
    if (ret)
        return ret;

    return arg.mem_buf_fd;
}
#else
int VmMem::RemoteAllocDmabufIoctl(uint64_t size, const VmPerm& perms,
        const std::string& src_dma_heap_name,
        const std::string& dst_dma_heap_name)
{
    (void)size;
    (void)perms;
    (void)src_dma_heap_name;
    (void)dst_dma_heap_name;

    LOG(PRI_ERROR, "RemoteAllocDmabuf() NOT SUPPORTED.");

    return -ENOTTY;
}
#endif

int VmMem::MemorySizeHint(int64_t size, const std::string& name)
{
    return MemorySizeHintIoctl(size, name);
}

#if defined(QTI_VIRTIO_MEM_IOC_HINT_CREATE) && !defined(NO_MEMORY_HINT)
int VmMem::MemorySizeHintIoctl(int64_t size, const std::string& name)
{
    struct qti_virtio_mem_ioc_hint_create_arg arg = {};
    int virtio_mem_fd, ret;

    virtio_mem_fd = TEMP_FAILURE_RETRY(open(kVirtioMemPath, O_RDONLY | O_CLOEXEC));
    if (virtio_mem_fd < 0) {
        LOG(PRI_ERROR, "Unable to open %s : %s",  kVirtioMemPath, strerror(errno));
        return virtio_mem_fd;
    }

    arg.size = size;
    strlcpy(arg.name, name.c_str(), sizeof(arg.name));

    ret = ioctl(virtio_mem_fd, QTI_VIRTIO_MEM_IOC_HINT_CREATE, &arg);
    if (ret) {
        LOG(PRI_ERROR, "MemorySizeHint() Failed.");
        close(virtio_mem_fd);
        return ret;
    }

    close(virtio_mem_fd);
    return arg.fd;
}
#else
int VmMem::MemorySizeHintIoctl(int64_t size, const std::string& name)
{
    (void)size;
    (void)name;

    LOG(PRI_ERROR, "MemorySizeHint() NOT SUPPORTED.");
    return -ENOTSUP;
}
#endif

VmMem::VmMem()
{
    mem_buf_fd = -1;
};

VmMem::~VmMem()
{
    if (mem_buf_fd > 0)
        close(mem_buf_fd);
};

std::unique_ptr<VmMem> VmMem::CreateVmMem() {
    auto instance = std::unique_ptr<VmMem>(new VmMem());
    auto err = std::unique_ptr<VmMem>(nullptr);

    /*
     * We expect 5.10 or newer kernels to use dma-buf-heaps.
     */
#ifdef MEM_BUF_IOC_LEND
    instance->have_dma_buf_heaps = true;
#else
    instance->have_dma_buf_heaps = false;
#endif

    if (instance->have_dma_buf_heaps) {
            int fd = TEMP_FAILURE_RETRY(open(kMembufPath, O_RDONLY | O_CLOEXEC));
            if (fd < 0){
                LOG(PRI_ERROR, "Unable to open %s errno: %d", kMembufPath, errno);
                return err;
            }
            instance->mem_buf_fd = fd;
    } else {
            instance->mem_buf_fd = -1;
    }

    LOG(PRI_INFO, "Using LibvmMem with %s"
		,(instance->have_dma_buf_heaps ? "DMABUFHEAPS ": "ION "));

    return instance;
}
