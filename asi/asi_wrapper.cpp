#include <ASICamera2.h>
#include "asi_wrapper.h"
#include <iostream>

// Forward declarations of functions from asi_view.cpp
extern ASI_ERROR_CODE cam_init_camera(int& cameraID, int& roiWidth, int& roiHeight, int& roiBin, ASI_IMG_TYPE& imgType);
extern int cam_get_frame(int cameraID, unsigned char* buffer, int bufferSize, int waitMs);
extern ASI_ERROR_CODE cam_stop_camera(int cameraID);

// Wrapper functions that use C-style interfaces (pointers instead of references)
extern "C" {

    ASI_WRAPPER_API int asi_init_camera(int* cameraID, int* roiWidth, int* roiHeight, int* roiBin, int* imgType) {
        if (!cameraID || !roiWidth || !roiHeight || !roiBin || !imgType) {
            return -1; // Invalid parameters
        }
        
        ASI_IMG_TYPE imgTypeEnum = static_cast<ASI_IMG_TYPE>(*imgType);
        ASI_ERROR_CODE result = cam_init_camera(*cameraID, *roiWidth, *roiHeight, *roiBin, imgTypeEnum);
        *imgType = static_cast<int>(imgTypeEnum);
        
        std::cout << "asi_init_camera wrapper: cameraID=" << *cameraID 
                  << ", result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_get_frame(int cameraID, unsigned char* buffer, int bufferSize, int waitMs) {
        if (!buffer || bufferSize <= 0) {
            return -1; // Invalid parameters
        }
        
        int result = cam_get_frame(cameraID, buffer, bufferSize, waitMs);
        
        return result;
    }

    ASI_WRAPPER_API int asi_stop_camera(int cameraID) {
        ASI_ERROR_CODE result = cam_stop_camera(cameraID);
        
        std::cout << "asi_stop_camera wrapper: result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_start_video_capture(int cameraID) {
        ASI_ERROR_CODE result = ASIStartVideoCapture(cameraID);
        
        std::cout << "asi_start_video_capture wrapper: result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_stop_video_capture(int cameraID) {
        ASI_ERROR_CODE result = ASIStopVideoCapture(cameraID);
        
        std::cout << "asi_stop_video_capture wrapper: result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_start_exposure(int cameraID, int isDark) {
        ASI_BOOL isDarkBool = isDark ? ASI_TRUE : ASI_FALSE;
        ASI_ERROR_CODE result = ASIStartExposure(cameraID, isDarkBool);
        
        std::cout << "asi_start_exposure wrapper: result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_get_exp_status(int cameraID, int* status) {
        if (!status) {
            return -1; // Invalid parameters
        }
        
        ASI_EXPOSURE_STATUS expStatus;
        ASI_ERROR_CODE result = ASIGetExpStatus(cameraID, &expStatus);
        *status = static_cast<int>(expStatus);
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_get_data_after_exp(int cameraID, unsigned char* buffer, int bufferSize) {
        if (!buffer || bufferSize <= 0) {
            return -1; // Invalid parameters
        }
        
        ASI_ERROR_CODE result = ASIGetDataAfterExp(cameraID, buffer, bufferSize);
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_set_exposure(int cameraID, long value, int isAuto) {
        ASI_BOOL isAutoBool = isAuto ? ASI_TRUE : ASI_FALSE;
        ASI_ERROR_CODE result = ASISetControlValue(cameraID, ASI_EXPOSURE, value, isAutoBool);
        
        std::cout << "asi_set_exposure wrapper: value=" << value << ", result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

    ASI_WRAPPER_API int asi_set_gain(int cameraID, long value, int isAuto) {
        ASI_BOOL isAutoBool = isAuto ? ASI_TRUE : ASI_FALSE;
        ASI_ERROR_CODE result = ASISetControlValue(cameraID, ASI_GAIN, value, isAutoBool);
        
        std::cout << "asi_set_gain wrapper: value=" << value << ", result=" << result << std::endl;
        
        return static_cast<int>(result);
    }

}
