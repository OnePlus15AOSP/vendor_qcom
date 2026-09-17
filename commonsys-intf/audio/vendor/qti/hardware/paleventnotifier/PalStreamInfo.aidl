/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.paleventnotifier;

@VintfStability
parcelable PalStreamInfo {
    long version;
    long size;
    long durationUs;
    boolean hasVideo;
    int txProxyType;
    int rxProxyType;
    boolean isStreaming;
    int loopbackType;
    int hapticsType;
}
