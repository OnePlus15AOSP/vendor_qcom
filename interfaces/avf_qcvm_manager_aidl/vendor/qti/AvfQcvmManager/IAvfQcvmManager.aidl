/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
*/
package vendor.qti.AvfQcvmManager;

import vendor.qti.AvfQcvmManager.IVirtualMachine;
import vendor.qti.AvfQcvmManager.VmInfo;

@VintfStability
interface IAvfQcvmManager{

    /** Gets a list of available VMs from the json config and their info */
    List<VmInfo> availableVms();

    /** Returns a handle to the VirtualMachine based on the vm_name. */
    IVirtualMachine getVm(String vm_name);
}
