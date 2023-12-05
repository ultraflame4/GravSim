//
// Created by powew on 5/12/2023.
//

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "GravSim/QuadTree.hh"


namespace QuadTree {

    template<typename T>
    class Node {
    public:
        std::unique_ptr<Node<T>> children[4];
        int depth;
        std::vector<T> items;

        Node<T> *addchild(int index, int _depth, bool force = false) {
            std::unique_ptr<Node<T>>& child = children[index];
            if (force) {
                child.reset();
            }
            if (child == nullptr) {
                child.reset(new Node<T>());
                child->depth = _depth;
            }
            return child.get();
        }

        void clearItems(){
            items.clear();
            for (const auto& child: children) {
                if (child != nullptr) {
                    child->clearItems();
                }
            }
        }

    };

    template<typename T>
    class QuadTreeManager {
    public:
        std::unique_ptr<Node<T>> rootNode;
        float physicalSize;
        glm::vec2 center;

        explicit QuadTreeManager(float physicalSize = 100.f, glm::vec2 center = glm::vec2(0, 0)) {
            rootNode.reset(new Node<T>());
            rootNode->depth=0;
        }

        void clearItems(){
            rootNode->clearItems();
        }

        Node<T> *CreateNodeFromPosition(glm::vec2 position, int maxDepth) {
            glm::vec2 quad_center = center;
            float halfSize = physicalSize / 2;
            // pointer to the current quad
            QuadTree::Node<T> *parent = rootNode.get();
            // pointer used when indexing children of search quad
            QuadTree::Node<T> *current = rootNode.get();

            int depth = rootNode->depth;
            while (current != nullptr && depth <= maxDepth) {
                parent = current;
                depth++;

                if (position.y > quad_center.y) {
                    // Top left
                    if (position.x < quad_center.x) {
                        current = parent->addchild(0, depth);
                        quad_center.x -= halfSize;
                    }
                        // top right
                    else {
                        current = parent->addchild(1, depth);
                        quad_center.x += halfSize;
                    }
                    quad_center.y += halfSize;
                } else {
                    // bottom left
                    if (position.x < quad_center.x) {
                        current = parent->addchild(2, depth);
                        quad_center.x -= halfSize;
                    }
                        // bottom right
                    else {
                        current = parent->addchild(3, depth);
                        quad_center.x += halfSize;
                    }
                    quad_center.y -= halfSize;
                }
                halfSize = halfSize / 2;
            }
            return parent;
        }
    };
}