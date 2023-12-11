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


    struct Square {
        glm::vec2 center;
        float size;
        bool colliding = false;

        Square(const glm::vec2 &center, float size) : center(center), size(size) {}

        float getHalfSize() const {
            return size / 2;
        }

        bool collidePoint(glm::vec2 pos) {
            float halfSize = size / 2;
            float rightX = center.x + halfSize;
            float leftX = center.x - halfSize;
            float topY = center.y + halfSize;
            float botY = center.y - halfSize;
            if (pos.x > rightX || pos.x < leftX) return false;
            if (pos.y > topY || pos.y < botY) return false;
            return true;
        }

        int getchild_index(glm::vec2 pos) {
            float halfSize = size / 2;
            // top
            if (pos.y > center.y) {
                // Top left
                if (pos.x < center.x) {
                    return 0;
                }
                // top right
                return 1;
            }
            // bottom left
            if (pos.x < center.x) {
                return 2;
            }
            // bottom right
            return 3;
        }

        Square getchild(int childIndex) const {
            Square copy(center, size);
            copy.size /= 2;
            if (childIndex % 2 == 0) {
                copy.center.x -= copy.getHalfSize();
            } else {
                copy.center.x += copy.getHalfSize();
            }

            if (childIndex <= 1) {
                copy.center.y += copy.getHalfSize();
            } else {
                copy.center.y -= copy.getHalfSize();
            }
            return copy;
        }

        Square getchild(glm::vec2 pos) {
            return getchild(getchild_index(pos));
        }

        /**
         * Gets the parent square to this square.
         * @param quad_index The quad index of this square
         * @return A square representing the parent
         */
        Square getparent(int quad_index) {
            Square copy(center, size);
            if (quad_index % 2 == 0) {
                copy.center.x += copy.getHalfSize();
            } else {
                copy.center.x -= copy.getHalfSize();
            }

            if (quad_index <= 1) {
                copy.center.y -= copy.getHalfSize();
            } else {
                copy.center.y += copy.getHalfSize();
            }
            copy.size *= 2;

            return copy;
        }

    };

    template<typename T>
    class Node {
    private:
        bool _hasChildren = true;
    public:
        std::unique_ptr<Node<T>> children[4];
        int depth = 0;
        int index;
        Node<T> *parent = nullptr;
        std::vector<T> items;

        bool IsEmpty() {
            return _hasChildren && items.size() == 0;
        }

        Node<T> *addchild(int index, int _depth, bool force = false) {
            std::unique_ptr<Node<T>> &child = children[index];
            if (force) {
                child.reset();
            }
            if (child == nullptr) {
                child.reset(new Node<T>());
                child->parent = this;
                child->index = index;
                child->depth = _depth;
            }
            _hasChildren = false;

            return child.get();
        }

        void clearItems() {
            items.clear();
            for (const auto &child: children) {
                if (child != nullptr && !child->IsEmpty()) {
                    child->clearItems();
                }
            }
            bool deleteSelf = true;
            for (auto &i: children) {
                auto child = i.get();
                if (child != nullptr) {
                    if (!child->_hasChildren) deleteSelf = false;
                }
            }

            _hasChildren = deleteSelf;
        }
    };

    template<typename T>
    struct Quad {

        Node<T> *node;
        Square square;


        Quad<T> addchild(int index, bool force = false) {
            node->addchild(index, node->depth + 1, force);
            return getchild(index);
        }

        Quad<T> addchild(glm::vec2 pos, bool force = false) {
            return addchild(square.getchild_index(pos), force);
        }

        Quad<T> getchild(int index) {
            return Quad<T>(node->children[index].get(), square.getchild(index));
        }

        Quad<T> getchild(glm::vec2 pos) {
            return getchild(square.getchild_index(pos));
        }

        Quad<T> getparent() {
            return Quad<T>(node->parent, square.getparent(node->index));
        }

        bool collidePoint(glm::vec2 pos) {
            return square.collidePoint(pos);
        }

        bool IsEmpty() {
            return node == nullptr || node->IsEmpty();
        }

        static inline Quad<T> fromRootNode(Node<T> *node, glm::vec2 center, float rootSize) {
            return Quad<T>(node, Square(center, rootSize));
        }

        static Quad<T> GetEmpty() {
            return Quad<T>(nullptr, Square(glm::vec2(0, 0), 0));
        }
    };


    inline glm::vec2 IndexToQuadCorner(int index) {
        switch (index) {
            case 0:
                return glm::vec2(-.5, .5);
            case 1:
                return glm::vec2(.5, .5);
            case 2:
                return glm::vec2(-.5, -.5);
            case 3:
                return glm::vec2(.5, -.5);
            default:
                break;
        }
        throw std::invalid_argument("Index is more than 3 ! (0-4 only)");
    }


    template<typename T>
    class QuadTreeManager {
    private:
        std::shared_ptr<spdlog::logger> logger = logging::get<QuadTreeManager>();
    public:
        std::unique_ptr<Node<T>> rootNode;
        float physicalSize;
        glm::vec2 center;

        explicit QuadTreeManager(float initialSize = 10000.f, glm::vec2 center = glm::vec2(0, 0)) {
            rootNode.reset(new Node<T>());
            rootNode->depth = 0;
            this->center = center;
            this->physicalSize = initialSize;
        }

        /**
         * DDA Line Raycast for direct descendents of the quad.
         * @param quad The quad to raycast.
         * @param head Where the ray starts. Not that this value is a reference and will change!
         * @param direction Direction of the ray
         * @return
         */
        Quad<T> RaycastQuad(Quad<T> quad, glm::vec2& head, glm::vec2 direction){
            glm::vec2 dxy = direction / quad.square.size;
            int steps = std::max(abs(dxy.x), abs(dxy.y));
            glm::vec2 xy_inc = dxy / (float) steps;
            for (int i = 0; i <= steps; ++i) {
                auto child = quad.getchild(head);
                if (!child.IsEmpty()) return child;
                head += xy_inc;
            }
            return Quad<T>::GetEmpty();
        }

        Quad<T> Raycast(glm::vec2 origin, glm::vec2 direction){
            Quad<T> current = GetRootQuad();
            Quad<T> last = Quad<T>::GetEmpty();
            glm::vec2 head = origin;
            while (!current.IsEmpty()) {
                last = current;
                current = RaycastQuad(current, head, direction);
            }
            return last;
        }

        void clearItems() {
            rootNode->clearItems();
        }

        Quad<T> GetRootQuad() {
            return Quad<T>::fromRootNode(rootNode.get(), center, physicalSize);
        }

        Quad<T> GetOrCreateQuad(glm::vec2 position, int maxDepth) {

            Quad<T> current = GetRootQuad();

            while (current.node->depth <= maxDepth) {
                current = current.addchild(position);
            }
            return current;
        }
    };
}