
FILE(GLOB icmfiles *.icm)

INSTALL(FILES  ${icmfiles}
        DESTINATION ${DATA_INSTALL_DIR}/libkdcraw/profiles )
