
KDE4_NO_ENABLE_FINAL(kdcraw)
INCLUDE_DIRECTORIES(../libraw
                    ../libkdcraw
                   )

SET(raw2png_SRCS raw2png.cpp)

KDE4_ADD_EXECUTABLE(raw2png NOGUI ${raw2png_SRCS})
TARGET_LINK_LIBRARIES(raw2png kdcraw)
