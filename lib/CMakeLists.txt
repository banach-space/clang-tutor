# THE LIST OF PLUGINS AND THE CORRESPONDING SOURCE FILES
# ======================================================
add_library(
  HelloWorld
  SHARED
  HelloWorld.cpp
  )

# CONFIGURE THE PLUGIN LIBRARIES
# ==============================
target_link_libraries(
  HelloWorld
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
  )
