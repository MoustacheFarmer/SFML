////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/System/Err.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <cassert>
#include <cstdlib>


namespace
{
    // The shared display and its reference counter
    Display* sharedDisplay = NULL;
    unsigned int referenceCount = 0;
}

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
Display* OpenDisplay()
{
    if (referenceCount == 0)
    {
        sharedDisplay = XOpenDisplay(NULL);

        // Opening display failed: The best we can do at the moment is to output a meaningful error message
        // and cause an abnormal program termination
        if (!sharedDisplay)
        {
            err() << "Failed to open X11 display; make sure the DISPLAY environment variable is set correctly" << std::endl;
            std::abort();
        }
    }

    referenceCount++;
    return sharedDisplay;
}


////////////////////////////////////////////////////////////
xcb_connection_t* OpenConnection()
{
    return XGetXCBConnection(OpenDisplay());
}


////////////////////////////////////////////////////////////
void CloseDisplay(Display* display)
{
    assert(display == sharedDisplay);

    referenceCount--;
    if (referenceCount == 0)
        XCloseDisplay(display);
}

////////////////////////////////////////////////////////////
void CloseConnection(xcb_connection_t* connection)
{
    assert(connection == XGetXCBConnection(sharedDisplay));
    return CloseDisplay(sharedDisplay);
}

////////////////////////////////////////////////////////////
xcb_screen_t* XCBScreenOfDisplay(xcb_connection_t* connection, int screen_nbr)
{
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));

    for (; iter.rem; --screen_nbr, xcb_screen_next (&iter))
    {
        if (screen_nbr == 0)
            return iter.data;
    }

    return NULL;
}

} // namespace priv

} // namespace sf
