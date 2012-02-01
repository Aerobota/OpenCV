#ifndef VLCINSTANCE_H
#define VLCINSTANCE_H

#include "vlc/vlc.h"
#include "VLCpp.hpp"

class   Instance : public Internal< libvlc_instance_t >
{
private:
    Instance();
    Instance( int argc, const char** argv );

private:
         static Instance*    m_singleton;

public:

    static Instance*    getInstance()
    {
        if (NULL == m_singleton)
            m_singleton =  new Instance();

        return m_singleton;
    }

    static void kill()
    {
        if ( m_singleton != NULL )
        {
            delete m_singleton;
            m_singleton = NULL;
        }
    }
};

#endif // VLCINSTANCE_H
