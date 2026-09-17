/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.paleventnotifier;

import vendor.qti.hardware.paleventnotifier.PalCallbackEventType;
import vendor.qti.hardware.paleventnotifier.PalTranslationTextEvent;

@VintfStability
parcelable PalCallbackEvent {
    /** event id of the event raised on the stream. */
    PalCallbackEventType event_id;
    /** payload to be got from callback. */
    byte[] payload;

}
