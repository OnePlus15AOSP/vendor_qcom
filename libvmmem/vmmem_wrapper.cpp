/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "vmmem.h"

extern "C" {
VmMem *CreateVmMem()
{
	std::unique_ptr<VmMem> instance = VmMem::CreateVmMem();
	if (!instance)
		return nullptr;

	return instance.release();
}

void FreeVmMem(VmMem *instance)
{
	delete instance;
}

int IsExclusiveOwnerDmabuf(int fd, bool *is_exclusive_owner)
{
	return VmMem::IsExclusiveOwnerDmabuf(fd, *is_exclusive_owner);
}

VmHandle FindVmByName(VmMem *instance, char *cstr)
{
	std::string name(cstr);
	return instance->FindVmByName(name);
}

int LendDmabufHandle(VmMem *instance, int dma_buf_fd, VmHandle *handles,
		uint32_t *perms, int nr, int64_t *memparcel_hdl)
{
	VmPerm vmperms;

	for (int i = 0; i < nr; i++) {
		vmperms.push_back( {handles[i], perms[i]} );
	}
	return instance->LendDmabuf(dma_buf_fd, vmperms, memparcel_hdl);
}

int LendDmabuf(VmMem *instance, int dma_buf_fd, VmHandle *handles,
		uint32_t *perms, int nr)
{
	return LendDmabufHandle(instance, dma_buf_fd, handles, perms,
			nr, nullptr);
}

int ShareDmabufHandle(VmMem *instance, int dma_buf_fd, VmHandle *handles,
	uint32_t *perms, int nr, int64_t *memparcel_hdl)
{
	VmPerm vmperms;

	for (int i = 0; i < nr; i++) {
		vmperms.push_back( {handles[i], perms[i]} );
	}
	return instance->ShareDmabuf(dma_buf_fd, vmperms, memparcel_hdl);
}

int ShareDmabuf(VmMem *instance, int dma_buf_fd, VmHandle *handles,
	uint32_t *perms, int nr)
{
	return ShareDmabufHandle(instance, dma_buf_fd, handles, perms,
		nr, nullptr);
}

int RetrieveDmabuf(VmMem *instance, VmHandle owner, VmHandle *handles,
	uint32_t *perms, int nr, int64_t memparcel_hdl)
{
	VmPerm vmperms;

	for (int i = 0; i < nr; i++) {
		vmperms.push_back( {handles[i], perms[i]} );
	}

	return instance->RetrieveDmabuf(owner, vmperms, memparcel_hdl);
}

int ReclaimDmabuf(VmMem *instance, int dma_buf_fd, int64_t memparcel_hdl)
{
	return instance->ReclaimDmabuf(dma_buf_fd, memparcel_hdl);
}

int RemoteAllocDmabuf(VmMem *instance, uint64_t size, VmHandle *handles,
	uint32_t *perms, int nr,
	char *c_src_dma_heap_name, char *c_dst_dma_heap_name)
{
	std::string src_dma_heap_name(c_src_dma_heap_name);
	std::string dst_dma_heap_name(c_dst_dma_heap_name);

	VmPerm vmperms;
	for (int i = 0; i < nr; i++) {
		vmperms.push_back( {handles[i], perms[i]} );
	}

	return instance->RemoteAllocDmabuf(size, vmperms,
		src_dma_heap_name, dst_dma_heap_name);
}

int MemorySizeHint(VmMem *instance, int64_t size, char *c_name)
{
	std::string name(c_name);

	return instance->MemorySizeHint(size, name);
}

} /* end extern "C" */
