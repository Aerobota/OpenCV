#include <cassert>
#include "VLCInstance.h"
#include <QtDebug>



Instance*   Instance::m_singleton = NULL;

Instance::Instance()
{
    char const *argv[] =
    {
        //"-vvvvv",
        "--no-video-title-show",
        "--no-skip-frames",
        "--no-audio",
        //"--plugin-path", VLC_TREE "/modules",
        "--ignore-config", //Don't use VLC's config files
        "--rtsp-caching=100"
        //"--{rtsp,http,sout-mux}-caching"
    };
    int argc = sizeof( argv ) / sizeof( *argv );

    m_internalPtr = libvlc_new( argc, argv);
    //CheckVlcppException(m_ex);
}

Instance::Instance( int argc, const char** argv )
{
    m_internalPtr = libvlc_new( argc, argv);
    //CheckVlcppException(m_ex);
}
