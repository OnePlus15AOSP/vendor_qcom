/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
*/
package vendor.qti.AvfQcvmManager;

import vendor.qti.AvfQcvmManager.IVirtualMachineCallback;
import vendor.qti.AvfQcvmManager.VmInfo;

@VintfStability
interface IVirtualMachine {

    /** Returns the current VM Info. */
    VmInfo getVmInfo();

    /** Starts running the VM. And place a vote.
        At the end of the call, the callback will be
        used to to give the current state of the VM.
        (if not started -> onStarting, is userspace ready -> onUserspaceReady)
    */
    oneway void start(IVirtualMachineCallback callback);

    /** If the VM has the capability to Force Stop, then force stop the VM */
    oneway void stop(IVirtualMachineCallback callback);

    /** Unvote and Query the VM to Stop on its own if possible */
    oneway void request_stop(IVirtualMachineCallback callback);



}
