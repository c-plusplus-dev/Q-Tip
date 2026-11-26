//
// Created by Ayaan on 2026-08-23.
//

#ifndef QTIP_CONFIG_H
#define QTIP_CONFIG_H

#define QTIP_CODE_BEGIN namespace QTip {
#define QTIP_CODE_END }

QTIP_CODE_BEGIN

#ifndef QTIP_ALLOW_BACKGROUND_GAMEPAD_INPUT
#define QTIP_ALLOW_BACKGROUND_GAMEPAD_INPUT false
#endif

inline const bool QTipConfig_GamepadBackgroundEvent =
        QTIP_ALLOW_BACKGROUND_GAMEPAD_INPUT;

QTIP_CODE_END

#endif //QTIP_CONFIG_H
