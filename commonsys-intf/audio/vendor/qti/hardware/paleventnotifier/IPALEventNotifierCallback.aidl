/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.paleventnotifier;

import vendor.qti.hardware.paleventnotifier.PalCallbackConfig;
import vendor.qti.hardware.paleventnotifier.PalCallbackEvent;

@VintfStability
interface IPALEventNotifierCallback {
    oneway void onStart(in PalCallbackConfig Config);
    oneway void onStop(in PalCallbackConfig Config);
    oneway void onDeviceSwitch(in PalCallbackConfig Config);
    oneway void onEventCallback(in PalCallbackEvent event_payload);
}
