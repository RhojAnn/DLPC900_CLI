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

}
