/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.camera.aon;
import vendor.qti.hardware.camera.aon.CameraRectangle;

/**
 * The information passed from the event of FaceDetectPro service type
 */
@VintfStability
parcelable PersonInfoType {
    /**
     * Confidence value of this person info 
     */
    int confidence;
    /**
     * Bounding box of person info
     * It is relative to the frame dimension height (exposed in FaceInfoPro)
     * It can be negative (e.g. a facial part can be estimated to be outside of the
     * frame boundary)
     */
    CameraRectangle rect;
}
