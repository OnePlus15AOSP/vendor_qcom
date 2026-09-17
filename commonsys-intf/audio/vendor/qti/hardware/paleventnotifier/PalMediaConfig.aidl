/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.paleventnotifier;

import vendor.qti.hardware.paleventnotifier.PalAudioFmt;
import vendor.qti.hardware.paleventnotifier.PalChannelInfo;

/**
 * Media configuraiton
 */
@VintfStability
parcelable PalMediaConfig {
    int sampleRate;
    int bitwidth;
    PalChannelInfo chInfo;
    PalAudioFmt audioFormatId;
}
