#ifndef IOT_DEVICE_PLATFORM_H
#define IOT_DEVICE_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif
 
/* Note: comment out the following line to build for a low-cost based OS */     
/* #define IOT_DEVICE_LO */        
    
/* Note: comment out the following line to disable logging */
#define IOT_ENABLE_LOGGING    
    
#ifdef IOT_ENABLE_LOGGING
    #define LogInfo(FORMAT, ...) { time_t t = time(NULL); (void)fprintf(stdout,"%.24s [INFO] (%s) - " FORMAT "\n", ctime(&t), __func__, ##__VA_ARGS__); }  
    #define LogError(FORMAT, ...) { time_t t = time(NULL); (void)fprintf(stderr,"%.24s [ERR] (%s) - " FORMAT "\n", ctime(&t), __func__, ##__VA_ARGS__); }      
#else    
    #define LogInfo(FORMAT, ...)
    #define LogError(FORMAT, ...)   
#endif    

#ifdef __cplusplus
}
#endif

#endif /* IOT_DEVICE_PLATFORM_H */

