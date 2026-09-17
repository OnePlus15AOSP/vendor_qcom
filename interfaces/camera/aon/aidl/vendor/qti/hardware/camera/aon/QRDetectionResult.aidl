/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

package vendor.qti.hardware.camera.aon;
import vendor.qti.hardware.camera.aon.QRDetectionResultType;
import vendor.qti.hardware.camera.aon.CameraRectangle;

/**
 * The information per QRDetectionResult passed from the event of QRC service type
 */
@VintfStability
parcelable QRDetectionResult {
    int confidence;
    /**
     * The frame dimension where ROI Coordinates will be based on
     */
    int dimWidth;
    int dimHeight;

    /**
     * The detected QRC result's bounding box and coordinate
     */
    CameraRectangle rect;

    /**
     * The detected result type can be QRCode, BarCode or TikTok 
     */
    QRDetectionResultType resultType; 
}
