
find_library(INTL_LIBRARY NAMES intl)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(INTL DEFAULT_MSG
                                  INTL_LIBRARY)

mark_as_advanced(INTL_LIBRARY)
