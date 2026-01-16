#include <thread>
#include <ASICamera2.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <commdlg.h>

// Library functions for ASI camera - these are the same as asi_view.cpp but without main()

ASI_ERROR_CODE is_camera_connected(){
    int numCameras = ASIGetNumOfConnectedCameras();
    std::cout << "ASIGetNumOfConnectedCameras returned: " << numCameras << std::endl;
    if (numCameras <= 0) {
        std::cout << "No ZWO ASI cameras detected." << std::endl;
        return ASI_ERROR_INVALID_INDEX;
    }
    return ASI_SUCCESS;
}

ASI_ERROR_CODE cam_set_pos(int cameraID, int startX, int startY) {
    ASI_ERROR_CODE res = ASISetStartPos(cameraID, startX, startY);
    std::cout << "ASISetStartPos (" << startX << "," << startY << ") result: " << res << std::endl;
    return res;
}

ASI_ERROR_CODE cam_get_pos(int cameraID, int& startX, int& startY) {
    ASI_ERROR_CODE res = ASIGetStartPos(cameraID, &startX, &startY);
    std::cout << "ASIGetStartPos: X=" << startX << ", Y=" << startY << " (result: " << res << ")" << std::endl;
    return res;
}

ASI_ERROR_CODE cam_set_ROI(int cameraID, int roiWidth, int roiHeight, int roiBin, ASI_IMG_TYPE imgType) {
    ASI_ERROR_CODE res = ASISetROIFormat(cameraID, roiWidth, roiHeight, roiBin, imgType);
    std::cout << "ASISetROIFormat (" << roiWidth << "x" << roiHeight << ") result: " << res << std::endl;
    return res;
}

ASI_ERROR_CODE cam_get_ROI(int cameraID, int& roiWidth, int& roiHeight, int& roiBin, ASI_IMG_TYPE& imgType) {
    ASI_ERROR_CODE res = ASIGetROIFormat(cameraID, &roiWidth, &roiHeight, &roiBin, &imgType);
    std::cout << "ASIGetROIFormat: " << roiWidth << "x" << roiHeight << ", Bin=" << roiBin << ", ImgType=" << imgType << " (result: " << res << ")" << std::endl;
    return res;
}

ASI_ERROR_CODE cam_init_camera(int& cameraID, int& roiWidth, int& roiHeight, int& roiBin, ASI_IMG_TYPE& imgType) {
    is_camera_connected();
    
    ASI_CAMERA_INFO info;
    ASI_ERROR_CODE propResult = ASIGetCameraProperty(&info, 0);
    std::cout << "ASIGetCameraProperty result: " << propResult << ", CameraID: " << info.CameraID << std::endl;
    cameraID = info.CameraID;

    ASI_ERROR_CODE openResult = ASIOpenCamera(cameraID);
    std::cout << "ASIOpenCamera result: " << openResult << std::endl;
    if (openResult != ASI_SUCCESS) {
        std::cout << "Failed to open camera." << std::endl;
        return openResult;
    }

    ASI_ERROR_CODE initResult = ASIInitCamera(cameraID);
    std::cout << "ASIInitCamera result: " << initResult << std::endl;
    if (initResult != ASI_SUCCESS) {
        std::cout << "Failed to initialize camera." << std::endl;
        return initResult;
    }

    int numControls = 0;
    ASI_ERROR_CODE controlsResult = ASIGetNumOfControls(cameraID, &numControls);
    std::cout << "ASIGetNumOfControls result: " << controlsResult << ", numControls: " << numControls << std::endl;

    ASI_CONTROL_CAPS caps;
    for (int i = 0; i < numControls; ++i) {
        int capsResult = ASIGetControlCaps(cameraID, i, &caps);
        std::cout << "ASIGetControlCaps index " << i << ": result=" << capsResult
                  << ", Name=" << caps.Name << ", Min=" << caps.MinValue << ", Max=" << caps.MaxValue << std::endl;
    }

    int roiResult = cam_set_ROI(cameraID, roiWidth, roiHeight, roiBin, imgType);
    if (roiResult != ASI_SUCCESS) {
        std::cout << "Failed to set ROI." << std::endl;
        return initResult;
    }

    int centerX = (8288 - roiWidth) / 2;
    int centerY = (5644 - roiHeight) / 2;

    int posResult = cam_set_pos(cameraID, centerX, centerY);
    if (posResult != ASI_SUCCESS) {
        std::cout << "Failed to set position." << std::endl;
        return initResult;
    }
    return ASI_SUCCESS;
}

ASI_ERROR_CODE cam_stop_camera(int cameraID) {
    ASI_ERROR_CODE stopResult = ASIStopVideoCapture(cameraID);
    std::cout << "ASIStopVideoCapture result: " << stopResult << std::endl;
    if(stopResult != ASI_SUCCESS) return static_cast<ASI_ERROR_CODE>(stopResult);
        
    ASI_ERROR_CODE closeResult = ASICloseCamera(cameraID);
    std::cout << "ASICloseCamera result: " << closeResult << std::endl;
    return closeResult;
}

// Gets frame from camera
int cam_get_frame(int cameraID, unsigned char* buffer, int bufferSize, int waitMs) {
    return ASIGetVideoData(cameraID, buffer, bufferSize, waitMs);
}

ASI_ERROR_CODE cam_set_exposure(int cameraID, long lValue, ASI_BOOL bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_EXPOSURE, lValue, bAuto);
    std::cout << "ASISetControlValue (EXPOSURE) result: " << res << std::endl;
    return res;
}

ASI_ERROR_CODE cam_set_gain(int cameraID, long lValue, ASI_BOOL bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_GAIN, lValue, bAuto);
    std::cout << "ASISetControlValue (GAIN) result: " << res << std::endl;
    return res;
}

ASI_ERROR_CODE cam_set_offset(int cameraID, long lValue, ASI_BOOL bAuto) {
    ASI_ERROR_CODE res = ASISetControlValue(cameraID, ASI_OFFSET, lValue, bAuto);
    std::cout << "ASISetControlValue (OFFSET) result: " << res << std::endl;
    return res;
}

ASI_ERROR_CODE cam_get_exposure(int cameraID, long& lValue, ASI_BOOL& bAuto) {
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_EXPOSURE, &lValue, &bAuto);
    std::cout << "ASIGetControlValue (EXPOSURE): Value=" << lValue << ", Auto=" << bAuto << " (result: " << res << ")" << std::endl;
    return res;
}

ASI_ERROR_CODE cam_get_gain(int cameraID, long& lValue, ASI_BOOL& bAuto) {
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_GAIN, &lValue, &bAuto);
    std::cout << "ASIGetControlValue (GAIN): Value=" << lValue << ", Auto=" << bAuto << " (result: " << res << ")" << std::endl;
    return res;
}

ASI_ERROR_CODE cam_get_offset(int cameraID, long& lValue, ASI_BOOL& bAuto) {
    ASI_ERROR_CODE res = ASIGetControlValue(cameraID, ASI_OFFSET, &lValue, &bAuto);
    std::cout << "ASIGetControlValue (OFFSET): Value=" << lValue << ", Auto=" << bAuto << " (result: " << res << ")" << std::endl;
    return res;
}

long cam_get_exposure_range(int cameraID, bool isMax) {
    ASI_CONTROL_CAPS caps;
    int numControls = 0;
    ASIGetNumOfControls(cameraID, &numControls);
    
    for (int i = 0; i < numControls; ++i) {
        ASI_ERROR_CODE res = ASIGetControlCaps(cameraID, i, &caps);
        if (res == ASI_SUCCESS && caps.ControlType == ASI_EXPOSURE) {
            long value = isMax ? caps.MaxValue : caps.MinValue;
            std::cout << "Exposure " << (isMax ? "Max" : "Min") << ": " << value << std::endl;
            return value;
        }
    }
    std::cout << "Failed to get exposure range." << std::endl;
    return -1;
}

long cam_get_gain_range(int cameraID, bool isMax) {
    ASI_CONTROL_CAPS caps;
    int numControls = 0;
    ASIGetNumOfControls(cameraID, &numControls);
    
    for (int i = 0; i < numControls; ++i) {
        ASI_ERROR_CODE res = ASIGetControlCaps(cameraID, i, &caps);
        if (res == ASI_SUCCESS && caps.ControlType == ASI_GAIN) {
            long value = isMax ? caps.MaxValue : caps.MinValue;
            std::cout << "Gain " << (isMax ? "Max" : "Min") << ": " << value << std::endl;
            return value;
        }
    }
    std::cout << "Failed to get gain range." << std::endl;
    return -1;
}
