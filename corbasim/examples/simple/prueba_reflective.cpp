#include "simpleC.h"
#include "simple_adapted.hpp"
#include <corbasim/core/reflective_fwd.hpp>
#include <iostream>
#include <sstream>
#include <string>

void print(corbasim::core::reflective_base const * current, 
        unsigned int level = 0)
{
    if (current->is_primitive()) return;

    // TODO implement slice type
    if (current->is_repeated()) return;
    
    unsigned int count;
    if ((count = current->get_children_count()) > 0)
    {
        for (unsigned int i = 0; i < count; i++) 
        {
            corbasim::core::reflective_base const * child = 
                current->get_child(i);

            std::cout << std::string(level * 4, ' ') 
                << current->get_child_name(i) << std::endl;

            print(child, level + 1);
        }
    }
}

int main(int argc, char **argv)
{
    const corbasim::core::reflective< SimpleExample::Padre > ref;

    print(&ref);

    corbasim::core::interface_reflective_base const * iface = 
        corbasim::core::interface_reflective< SimpleExample::Test >::get_instance();
}
