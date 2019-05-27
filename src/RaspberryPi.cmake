if(EXISTS "/opt/vc/include/bcm_host.h")
  set(RASPBERRY_PI 1 CACHE BOOL "")
  add_definitions(-D RASPBERRY_PI)

  find_path(BCM_INCLUDR_DIR bcm_host.h
    PATHS /opt/vc/include
  )

  find_library(BCM_EGL_LIBRARY
    NAMES "brcmGLESv2"
    PATHS /opt/vc/lib
  )

  find_library(BCM_LIBRARY
    NAMES bcm_host
    PATHS /opt/vc/lib
  )

  find_path(OPENGL_INCLUDE_DIR GLES/gl.h
    PATHS /opt/vc/include
    NO_DEFAULT_PATH
  )

  find_library(OPENGL_gl_LIBRARY
    NAMES brcmEGL
    PATHS /opt/vc/lib
    NO_DEFAULT_PATH
  )

  find_library(OPENGL_egl_LIBRARY
    NAMES brcmEGL
    PATHS /opt/vc/lib
    NO_DEFAULT_PATH
  )

endif()