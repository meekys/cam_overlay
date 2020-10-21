if(EXISTS "/opt/vc/include/bcm_host.h")
  set(RASPBERRY_PI 1 CACHE BOOL "")
  add_definitions(-D RASPBERRY_PI)
  message( "Found Raspberry Pi" )

  find_path(BCM_INCLUDE_DIR bcm_host.h
    PATHS /opt/vc/include
    NO_DEFAULT_PATH
  )
  
  find_library(BCM_EGL_LIBRARY
    NAMES brcmGLESv2 libbrcmGLESv2
    PATHS /opt/vc/lib
  )

  find_library(BCM_LIBRARY 
    NAMES bcm_host libbcm_host
    PATHS /opt/vc/lib
  )

endif()