// RUN:  ../bin/ct-code-style-checker -main-tu-only=false %s 2>&1 -- | FileCheck -implicit-check-not warning: -check-prefixes=WITH_INCLUDED_FILES %s
// RUN:  ../bin/ct-code-style-checker -main-tu-only=true %s 2>&1 -- | FileCheck -implicit-check-not warning: -allow-empty %s

#include "Inputs/ct-code-style-checker-header-file.h"

// WITH_INCLUDED_FILES: warning:
// WITH_INCLUDED_FILES: warning:
// WITH_INCLUDED_FILES: warning:
