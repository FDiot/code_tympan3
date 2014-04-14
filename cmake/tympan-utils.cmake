# Various utility macro

macro(install_tympan_plugin PLUGIN_NAME)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${TYMPAN_INSTALL_PLUGINS_Release} CONFIGURATIONS Release)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${TYMPAN_INSTALL_PLUGINS_Debug} CONFIGURATIONS Debug)
endmacro(install_tympan_plugin)

macro(install_tympan_component TARGET)
install(TARGETS ${TARGET} ARCHIVE DESTINATION lib
                          LIBRARY DESTINATION lib
                          RUNTIME DESTINATION . )

endmacro(install_tympan_component)
