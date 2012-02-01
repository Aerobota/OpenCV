#ifndef VLCPP_HPP
#define VLCPP_HPP

#include <cassert>
#include <stdlib.h>

    template < typename T >
    class   Internal
    {
        public:
            typedef     T*      internalPtr;
            T*                  getInternalPtr()
            {
                assert( m_internalPtr != NULL );
                return m_internalPtr;
            }
            operator T*() { return m_internalPtr; }
        protected:
            Internal() : m_internalPtr( NULL ) {}

            T*                  m_internalPtr;
    };

#endif // VLCPP_HPP
