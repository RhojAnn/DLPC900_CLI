#define ASI_EXPORTS
#include <ASICamera2.h>
#include <windows.h>

#ifdef _WIN32
    #define ASI_API __declspec(dllexport)
#else
    #define ASI_API
#endif

extern "C" {
// TODO: Document functions aka multi-line comments them

// Check and counts any connected camera
ASI_API int num_of_camera_connected(){
    int numCameras = ASIGetNumOfConnectedCameras();
    return numCameras;
}

ASI_API int cam_set_pos(int cameraID, int startX, int startY) {
    ASI_ERROR_CODE res = ASISetStartPos(cameraID, startX, startY);
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Sets ROI format
ASI_API int cam_set_ROI(int cameraID, int roiWidth, int roiHeight, int roiBin, ASI_IMG_TYPE imgType) {
    ASI_ERROR_CODE res = ASISetROIFormat(cameraID, roiWidth, roiHeight, roiBin, imgType);
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

/**
 * Notes：(1) when setting to auto adjust(bAuto=ASI_TRUE)，the lValue should be the current value 
(2) Automatic Exposure and Automatic Gain are only effective in Video mode (that is, when you get 
the image by calling ASIGetVideoData), but not in Snap mode (that is, when you get the image by 
using ASIGetDataAfterExp) 
 */

 /*
return:
ASI_SUCCESS : Operation is successful
ASI_ERROR_CAMERA_CLOSED : camera didn't open
ASI_ERROR_INVALID_ID  :no camera of this ID is connected or ID value is out of boundary
ASI_ERROR_INVALID_CONTROL_TYPE, //invalid Control type
ASI_ERROR_GENERAL_ERROR,//general error, eg: value is out of valid range; operate to camera hareware failed
*/

// Sets exposure controls
ASI_API int cam_set_exposure(int cameraID, long lValue, int bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_EXPOSURE, lValue, static_cast<ASI_BOOL>(bAuto));
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Sets gain controls
ASI_API int cam_set_gain(int cameraID, long lValue, int bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_GAIN, lValue, static_cast<ASI_BOOL>(bAuto));
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Sets offset controls
ASI_API int cam_set_offset(int cameraID, long lValue, int bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_OFFSET, lValue, static_cast<ASI_BOOL>(bAuto));
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Gets current ROI format
ASI_API int cam_get_pos(int cameraID, int* startX, int* startY) {
    if (!startX || !startY) return -1;
    ASI_ERROR_CODE res = ASIGetStartPos(cameraID, startX, startY);
    if (ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Gets current ROI start position
ASI_API int cam_get_ROI(int cameraID, int* roiWidth, int* roiHeight, int* roiBin, int* imgType) {
    if (!roiWidth || !roiHeight || !roiBin || !imgType) return -1;
    ASI_IMG_TYPE type;
    ASI_ERROR_CODE res = ASIGetROIFormat(cameraID, roiWidth, roiHeight, roiBin, &type);
    *imgType = static_cast<int>(type);
    if (ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Gets current exposure value 
ASI_API int cam_get_exposure(int cameraID, long* lValue, int* bAuto) {
    if (!lValue || !bAuto) return -1;
    ASI_BOOL autoVal;
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_EXPOSURE, lValue, &autoVal);
    *bAuto = static_cast<int>(autoVal);
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Gets current gain value
ASI_API int cam_get_gain(int cameraID, long* lValue, int* bAuto) {
    if (!lValue || !bAuto) return -1;
    ASI_BOOL autoVal;
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_GAIN, lValue, &autoVal);
    *bAuto = static_cast<int>(autoVal);
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Gets current offset value
ASI_API int cam_get_offset(int cameraID, long* lValue, int* bAuto) {
    if (!lValue || !bAuto) return -1;
    ASI_BOOL autoVal;
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_OFFSET, lValue, &autoVal);
    *bAuto = static_cast<int>(autoVal);
    if(ASI_ERROR_CODE::ASI_SUCCESS != res) return -1;
    return 0;
}

// Gets min/max exposure range
ASI_API int cam_get_exposure_range(int cameraID, long* minVal, long* maxVal) {
    if (!minVal || !maxVal) return -1;
    ASI_CONTROL_CAPS caps;
    int numControls = 0;
    ASIGetNumOfControls(cameraID, &numControls);
    
    for (int i = 0; i < numControls; ++i) {
        ASI_ERROR_CODE res = ASIGetControlCaps(cameraID, i, &caps);
        if (res == ASI_SUCCESS && caps.ControlType == ASI_EXPOSURE) {
            *minVal = caps.MinValue;
            *maxVal = caps.MaxValue;
            return 0;
        }
    }
    return -1;
}

// Gets min/max gain range (isMax=true for max, isMax=false for min)
ASI_API int cam_get_gain_range(int cameraID, long* minVal, long* maxVal) {
    if (!minVal || !maxVal) return -1;
    ASI_CONTROL_CAPS caps;
    int numControls = 0;
    ASIGetNumOfControls(cameraID, &numControls);
    
    for (int i = 0; i < numControls; ++i) {
        ASI_ERROR_CODE res = ASIGetControlCaps(cameraID, i, &caps);
        if (res == ASI_SUCCESS && caps.ControlType == ASI_GAIN) {
            *minVal = caps.MinValue;
            *maxVal = caps.MaxValue;
            return 0;
        }
    }
    return -1;
}

// Gets max width and height of the camera sensor
ASI_API int cam_get_dimension_range(int cameraID, int* minWidth, int* maxWidth, int* minHeight, int* maxHeight) {
    if (!maxWidth || !maxHeight || !minWidth || !minHeight) return -1;
    ASI_CAMERA_INFO info;
    ASI_ERROR_CODE res = ASIGetCameraPropertyByID(cameraID, &info);
    if (res != ASI_SUCCESS) return -2;
    *minWidth = 1;
    *minHeight = 1;
    *maxWidth = (int)info.MaxWidth;
    *maxHeight = (int)info.MaxHeight;
    return 0;
}

// ============== Video Mode ==============

// Starts video capture
ASI_API int cam_start_video(int cameraID) {
    ASI_ERROR_CODE res = ASIStartVideoCapture(cameraID);
    if (res != ASI_SUCCESS) return -1;
    return 0;
}

// Stops video capture
ASI_API int cam_stop_video(int cameraID) {
    ASI_ERROR_CODE res = ASIStopVideoCapture(cameraID);
    if (res != ASI_SUCCESS) return -1;
    return 0;
}

// Gets frame from camera
ASI_API int cam_get_frame(int cameraID, unsigned char* buffer, int bufferSize, int waitMs) {
    if (!buffer || bufferSize <= 0) return -100;
    ASI_ERROR_CODE res = ASIGetVideoData(cameraID, buffer, bufferSize, waitMs);
    if (ASI_ERROR_CODE::ASI_SUCCESS != res) return -1; 
    return 0;
}

// ============== Snap Mode ==============

// Starts snap exposure
ASI_API int cam_start_exposure(int cameraID, int isDark) {
    ASI_ERROR_CODE res = ASIStartExposure(cameraID, static_cast<ASI_BOOL>(isDark));
    if (res != ASI_SUCCESS) return -1;
    return 0;
}

// Gets exposure status: 0=idle, 1=working, 2=success, 3=failed
ASI_API int cam_get_exposure_status(int cameraID, int* status) {
    if (!status) return -1;
    ASI_EXPOSURE_STATUS expStatus;
    ASI_ERROR_CODE res = ASIGetExpStatus(cameraID, &expStatus);
    if (res != ASI_SUCCESS) return -1;
    *status = static_cast<int>(expStatus);
    return 0;
}

// Stops/cancels exposure
ASI_API int cam_stop_exposure(int cameraID) {
    ASI_ERROR_CODE res = ASIStopExposure(cameraID);
    if (res != ASI_SUCCESS) return -1;
    return 0;
}

// Gets image data after successful exposure
ASI_API int cam_get_data_after_exp(int cameraID, unsigned char* buffer, int bufferSize) {
    if (!buffer || bufferSize <= 0) return -1;
    ASI_ERROR_CODE res = ASIGetDataAfterExp(cameraID, buffer, bufferSize);
    if (res != ASI_SUCCESS) return -1;
    return 0;
}

// Combined snap: starts exposure, waits, gets data
ASI_API int cam_snap(int cameraID, unsigned char* buffer, int bufferSize, int isDark, int timeoutMs) {
    if (!buffer || bufferSize <= 0) return -1;
    
    // Stop video capture if running
    ASIStopVideoCapture(cameraID);
    
    // Start exposure
    ASI_ERROR_CODE startRes = ASIStartExposure(cameraID, static_cast<ASI_BOOL>(isDark));
    if (startRes != ASI_SUCCESS) return -2;
    
    // Poll for completion
    ASI_EXPOSURE_STATUS status;
    int elapsed = 0;
    int pollInterval = 10;
    
    while (true) {
        ASI_ERROR_CODE statusRes = ASIGetExpStatus(cameraID, &status);
        if (statusRes != ASI_SUCCESS) {
            ASIStopExposure(cameraID);
            return -3;
        }
        
        if (status == ASI_EXP_SUCCESS) break;
        if (status == ASI_EXP_FAILED) {
            ASIStopExposure(cameraID);
            return -4;
        }
        
        #ifdef _WIN32
            Sleep(pollInterval);
        #else
            usleep(pollInterval * 1000);
        #endif
        
        elapsed += pollInterval;
        if (timeoutMs > 0 && elapsed >= timeoutMs) {
            ASIStopExposure(cameraID);
            return -5;
        }
    }
    
    // Get image data
    ASI_ERROR_CODE getRes = ASIGetDataAfterExp(cameraID, buffer, bufferSize);
    if (getRes != ASI_SUCCESS) return -6;
    
    return 0;
}

// Initializes the camera and sets ROI
ASI_API int cam_init_camera(int* cameraID, int roiWidth, int roiHeight, int roiBin, int imgType) {
    if (!cameraID) return -1;

    if (num_of_camera_connected() <= 0) return -2;
    
    ASI_CAMERA_INFO info;
    ASI_ERROR_CODE propResult = ASIGetCameraProperty(&info, 0);
    if (propResult != ASI_SUCCESS) return -3;
    *cameraID = info.CameraID;

    ASI_ERROR_CODE openResult = ASIOpenCamera(*cameraID);
    if (openResult != ASI_SUCCESS) return -4;

    ASI_ERROR_CODE initResult = ASIInitCamera(*cameraID);
    if (initResult != ASI_SUCCESS) return -5;

    ASI_ERROR_CODE roiResult = ASISetROIFormat(*cameraID, roiWidth, roiHeight, roiBin, static_cast<ASI_IMG_TYPE>(imgType));
    if (roiResult != ASI_SUCCESS) return -6;

    int centerX = (info.MaxWidth - roiWidth) / 2;
    int centerY = (info.MaxHeight - roiHeight) / 2;
    ASI_ERROR_CODE posResult = ASISetStartPos(*cameraID, centerX, centerY);
    if (posResult != ASI_SUCCESS) return -7;
    
    return 0;
}

// Stops the camera and closes it
ASI_API int cam_stop_camera(int cameraID) {
    ASIStopVideoCapture(cameraID);
    ASIStopExposure(cameraID);

    ASI_ERROR_CODE closeResult = ASICloseCamera(cameraID);
    if (closeResult != ASI_SUCCESS) return -1;
    return 0;
}


} // extern "C"
