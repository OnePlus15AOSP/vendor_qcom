/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
*/
package vendor.qti.AvfQcvmManager;

@VintfStability
parcelable VmInfo {
    /** Name of the VM */
    String name;

    /** Early VM enabled */
    boolean early_vm;

    /** Enable the VM to be started */
    boolean enabled;

    /** The VM can be force stopped from within the VM */
    boolean force_stop;

    /** The number of assigned vCPUs  */
    int num_vcpus;

    /** CMA memory assigned to boot the VM */
    int cma_size;

    /** Shared memory between VM and Host assigned to boot the VM */
    int swiotlb_size;

    /** Total memory assigned to boot the VM with (CMA + SWIOTLB + Scattered)  */
    int total_memory;

    /** A static ID that is assigned to each QC VM */
    int vm_id;

    /** The Mink UID used for the shutdown service */
    int mink_uid;

}
