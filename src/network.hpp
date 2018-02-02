/**
 * @brief
 * @author Yuki Koizumi
 */
#ifndef NETWORK_HPP_INC
#define NETWORK_HPP_INC

#include "Router.hpp"

class Network
{
public:
    ~Network() {}
    static Network &instance();

    std::vector<router_ptr>& router_list() {
        return m_router_list;
    }

    void push_router(router_ptr r);

    router_ptr router(int id) {
        return m_router_list.at(id);
    }
    router_ptr search_router(int ID);
    int search_router_position(int ID) const;

private:
    std::vector<router_ptr> m_router_list;

private:
    Network() {}
    Network(const Network&);
    Network &operator=(const Network);
};

#endif
