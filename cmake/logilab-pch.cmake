# When LOGILAB_USE_PRECOMPILED_HEADER is OFF the following macro,
# related to pre-compiled headers (PCH) are defined as dummy ones
#  * set_precompiled_header(target ...)
#  * use_precompiled_header(target ...)

if(LOGILAB_USE_PRECOMPILED_HEADER)
  include(PrecompiledHeader)
else(LOGILAB_USE_PRECOMPILED_HEADER)
  macro(set_precompiled_header target) # DUMMY
  endmacro(set_precompiled_header)
  macro(use_precompiled_header target) # DUMMY
  endmacro(use_precompiled_header)
endif(LOGILAB_USE_PRECOMPILED_HEADER)
