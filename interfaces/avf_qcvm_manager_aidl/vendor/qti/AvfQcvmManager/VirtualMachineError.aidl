/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
*/
package vendor.qti.AvfQcvmManager;

@Backing(type="int")
@VintfStability
enum VirtualMachineError {

    /** Failed to boot the VM */
    FAILED_START = -1,

    /** Failed to Force Stop the VM */
    FAILED_STOP = -2,

    /** Failed to Request the VM to Stop when Vote is not 0*/
    VM_IN_USE = -3,

    /** Failed to request stop when Vote is 0 */
    FAILED_TO_REQUEST_STOP = -4,

}
