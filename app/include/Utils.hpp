//
// Created by filip on 8/21/25.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>

#include <spdlog/spdlog.h>

#define VECTOR3 glm::vec3
#define VECTOR4 glm::vec4

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define offsetof(type, member) __builtin_offsetof(type, member)
#define typeof_member(T, m)	typeof(((T*)0)->m)

#define container_of(ptr, type, member) ({				        \
    void *__mptr = (void *)(ptr);					            \
    ((type *)(__mptr - offsetof(type, member))); })


struct list_head{
    struct list_head *prev, *next;
};

static inline void list_init(struct list_head* ptr){
    ptr->prev = ptr;
    ptr->next = ptr;
}

static inline void list_del(struct list_head* ptr){
    ptr->next->prev = ptr->prev;
    ptr->prev->next = ptr->next;
    ptr->next = NULL;
    ptr->prev = NULL;
}

static inline void list_add(struct list_head* list, struct list_head* add){
    list->next = add;
    add->prev = list;
}

#define extract_from_list(_list, structure, member)     \
        container_of(_list, structure, member)

#define extract_next_from_list(_list, structure, member)    \
        extract_from_list((_list)->next, structure, member)

#define list_for_each(_list, head)  \
    for(_list = (head); _list != NULL; _list = _list->next)


#endif //UTILS_HPP
