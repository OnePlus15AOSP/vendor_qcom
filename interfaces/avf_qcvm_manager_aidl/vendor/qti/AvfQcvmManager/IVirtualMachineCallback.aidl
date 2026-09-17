/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
*/
package vendor.qti.AvfQcvmManager;

import vendor.qti.AvfQcvmManager.VirtualMachineError;

@VintfStability
oneway interface IVirtualMachineCallback {

    /** Called when the state of the VM changes. */
    void onStarting();

    /** Called when the state of the VM is fully stopped. */
    void onStopped();

    /** Called when the VM userspace is ready */
    void onUserspaceReady();

    /** Called when the state of the VM is in progress to shutdown. */
    void onShutdownInitiated();

    /** Called when the state of the VM becomes crashed */
    void onCrashed();

    /** Called when the HAL encounters an error when handling the VM */
    void onError(VirtualMachineError error);

}
