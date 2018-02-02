/**
 * @brief
 * @author Yuki Koizumi
 */
#include "network.hpp"
#include <algorithm>

Network &Network::instance()
{
    static Network object;
    return object;
}

void Network::push_router(router_ptr r)
{
    m_router_list.push_back(r);
}

router_ptr Network::search_router(int ID)
{
    std::vector<router_ptr>::iterator it = std::find_if(m_router_list.begin(), m_router_list.end(), [ID](router_ptr ptr) {
            return ptr->getID() == ID;
        });
    return (it == m_router_list.end()) ? router_ptr() : *it;
}

int Network::search_router_position(int ID) const
{
    for(int i = 0; i < ROUTER_N; i++) {
        if(m_router_list.at(i)->getID() == ID) {
            return i;
        }
    }

    return -1;
}
