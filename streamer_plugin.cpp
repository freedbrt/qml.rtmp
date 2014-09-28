#include "streamer_plugin.h"
#include "rtmpsender.h"
#include <rtmpreader.h>

#include <qqml.h>

void StreamerPlugin::registerTypes(const char *uri)
{
    // @uri com.rtmp.qmlcomponents
    qmlRegisterType<RTMPSender>(uri, 1, 0, "RTMPSender");
    qmlRegisterType<RTMPReader>(uri, 1, 0, "RTMPReader");
}
