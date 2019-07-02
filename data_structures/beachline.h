#ifndef BEACHLINE_H
#define BEACHLINE_H

#include "dcel.h"
#include "../mathVoronoi/parabola.h"
#include "../mathVoronoi/circle.h"
#include "./data_structures/event.h"
#include <queue>

//#include <cg3/geometry/2d/point2d.h> INCLUDED BY PARABOLA.H, CIRCLE.H AND HALF_EDGE.H

namespace Voronoi {
    /**
     * @brief The Node struct, base class of the two node types
     */
    struct Node {
        Node *parent;
        Node *left;
        Node *right;

        int height;

        Node() : parent(nullptr), left(nullptr), right(nullptr) {}
        Node(int height) : parent(nullptr), left(nullptr), right(nullptr), height(height) {}
        Node(Node* parent, Node* left, Node* right, int height) :
            parent(parent), left(left), right(right), height(height) {}
    };

    /**
     * @brief The InternalNode struct, the type for internal nodes
     */
    struct InternalNode : Node {
        size_t edge;
        std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint;

        InternalNode(Node* parent, Node* left, Node* right, int height, std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint) :
            Node(parent, left, right, height), edge(std::numeric_limits<size_t>::max()), breakpoint(breakpoint) {}
        InternalNode(Node* parent, Node* left, Node* right, int height, size_t edge, std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint) :
            Node(parent, left, right, height), edge(edge), breakpoint(breakpoint) {}
    };

    /**
     * @brief The Leaf struct, the type for leaves
     */
    struct Leaf : Node {
        Leaf* prev;
        Leaf* next;

        const cg3::Point2Dd* site;
        CircleEvent* circleEvent;

        Leaf(const cg3::Point2Dd* site) : Node(0), prev(nullptr), next(nullptr), site(site), circleEvent(nullptr) {}
        Leaf(Node* parent, Leaf* prev, Leaf* next, const cg3::Point2Dd* site) :
            Node(parent, nullptr, nullptr, 0), prev(prev), next(next), site(site), circleEvent(nullptr) {}
        Leaf(Node* parent, Leaf* prev, Leaf* next, const cg3::Point2Dd* site, CircleEvent* circleEvent) :
            Node(parent, nullptr, nullptr, 0), prev(prev), next(next), site(site), circleEvent(circleEvent) {}
    };

    /**
     * @brief The Beachline class, implemented as a balanced BST
     * @class Beachline
     */
    class Beachline {
        public:
            Beachline(double *sweepline) : root(nullptr), sweepline(sweepline) {}
            Beachline(const Beachline&);
            Beachline(Beachline&&);
            Beachline& operator=(Beachline);
            Beachline& operator=(Beachline&&);
            virtual ~Beachline();

            CircleEvent* addPoint(const cg3::Point2Dd& p, Leaf*& newPoint, std::vector<Voronoi::HalfEdge>& edges);
            Leaf* removePoint(const CircleEvent* cE, DCEL& dcel);
            void clear();

            Node* getRoot() const;
            bool isLeaf(const Node* node) const;
            bool isRight(const Node* node) const;
            int balance(const Node* node) const;
            bool isBalanced(const Node* node) const;
            void deleteNode(Node* node);
            void postorder(Node* node) const;
            void printLevelOrder(Node* node) const;
            Leaf* min(Node*) const;
            Leaf* max(Node*) const;
            Leaf* prev(Node*) const;
            Leaf* next(Node*) const;
        private:
            Node* root;
            double* sweepline;

            void swap(Beachline&);

            Node* findArc(const double x,
                          std::vector<std::pair<int,int>>& _balance, std::vector<int>& path, int& diff, int& last) const;
            double getValue(Node* _node) const;
            CircleEvent* makeSubtree(Node*& node, const cg3::Point2Dd& p, std::vector<Voronoi::HalfEdge>& edges);
            void handleRotation(Node* arc,
                                std::vector<std::pair<int,int>>& _balance, std::vector<int>& path, int diff, int last);
            void rebalanceCE(Node* node);
            void whichRotation(Node* node, const int firstPath, const int secondPath);
            void findPlus1AndRotate(Node* node, const std::vector<std::pair<int,int>>& _balance, const std::vector<int>& path);

            void rotateLeft(Node* node);
            void rotateRight(Node* node);
            void rotateRightLeft(Node* node);
            void rotateLeftRight(Node* node);

            void updateHeight(Node* node);
            void resetHeight(Node* node);

            Node* copyBeachline(Node* const &node);
            void updateAttributes(Node*&);
    };

    /**
     * @brief Beachline::getRoot
     * @return root of the Beachline
     */
    inline Node* Beachline::getRoot() const {
        return root;
    }

    /**
     * @brief Beachline::isLeaf
     * @param node
     * @return if the node is a leaf
     */
    inline bool Beachline::isLeaf(const Node* node) const {
        return !node->left && !node->right;
    }

    /**
     * @brief Beachline::isRight
     * @param node
     * @return true if the node is a right child, false if left
     */
    inline bool Beachline::isRight(const Node *node) const {
        if(node->parent)
            return node->parent->right == node;
        assert(node->parent == nullptr);
        throw std::invalid_argument("passed parameter is root");
    }

    /**
     * @brief Beachline::balance
     * @param node
     * @return the balance of the node
     */
    inline int Beachline::balance(const Node* node) const {
        if(node->left && node->right)
            return node->right->height - node->left->height;
        else if(node->right)
            return 1;
        else if(node->left)
            return -1;

        throw std::invalid_argument("passed parameter is a leaf");
    }

    /**
     * @brief Beachline::isBalanced check if subtree of node is balanced
     * @param node
     * @return
     */
    inline bool Beachline::isBalanced(const Node* node) const {
        if(!node)
            return true;

        return (isLeaf(node) ? true : std::abs(balance(node)) < 2) && isBalanced(node->left) && isBalanced(node->right);
    }

    /**
     * @brief Beachline::clear clears the Beachline
     */
    inline void Beachline::clear() {
        deleteNode(root);
        sweepline = nullptr;
    }

}

#endif // BEACHLINE_H
