//
// Created by powew on 5/12/2023.
//

#include "GravSim/QuadTree.hh"


template<typename T>
const QuadTree::Node<T> &QuadTree::QuadTreeManager<T>::GetNodeFromPosition(glm::vec2 position) {
    glm::vec2 quad_center = center;
    float halfSize = physicalSize / 2;
    // pointer to the current quad
    QuadTree::Node<T> *parent = &rootNode;
    // pointer used when indexing children of search quad
    QuadTree::Node<T> *current = &rootNode;

    while (current != nullptr) {
        parent = current;
        if (position.y > quad_center.y) {
            // Top left
            if (position.x < quad_center.x) {
                current = parent->children[0].get();
                quad_center.x -= halfSize;
            }
                // top right
            else {
                current = parent->children[1].get();
                quad_center.x += halfSize;
            }
            quad_center.y += halfSize;
        } else {
            // bottom left
            if (position.x < quad_center.x) {
                current = parent->children[2].get();
                quad_center.x -= halfSize;
            }
                // bottom right
            else {
                current = parent->children[3].get();
                quad_center.x += halfSize;
            }
            quad_center.y -= halfSize;
        }
        halfSize = halfSize / 2;
    }
    return parent;
}

template<typename T>
QuadTree::QuadTreeManager<T>::QuadTreeManager(float physicalSize, glm::vec2 center):physicalSize(physicalSize),
                                                                                    center(center) {
}