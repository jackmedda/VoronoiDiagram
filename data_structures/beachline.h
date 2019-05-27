#ifndef BEACHLINE_H
#define BEACHLINE_H

class Event;

#include "half_edge.h"

#include <math.h>

#include <cg3/geometry/2d/point2d.h>

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
        HalfEdge* edge;
        std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint;
        double xBreakpoint;

        InternalNode(Node* parent, Node* left, Node* right, int height, std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint) :
            Node(parent, left, right, height), edge(nullptr), breakpoint(breakpoint), xBreakpoint(nan("")) {}
        InternalNode(Node* parent, Node* left, Node* right, int height, std::pair<cg3::Point2Dd*, cg3::Point2Dd*> breakpoint, double xBreakpoint) :
            Node(parent, left, right, height), edge(nullptr), breakpoint(breakpoint), xBreakpoint(xBreakpoint) {}
    };

    /**
     * @brief The Leaf struct, the type for leaves
     */
    struct Leaf : Node {
        const cg3::Point2Dd* site;
        Event* circleEvent;

        Leaf(const cg3::Point2Dd* site) : Node(0), site(site), circleEvent(nullptr) {}
        Leaf(Node* parent, Node* left, Node* right, const cg3::Point2Dd* site) :
            Node(parent, left, right, 0), site(site), circleEvent(nullptr) {}
    };

    /**
     * @brief The Beachline class, implemented as a balanced BST
     * @class Beachline
     */
    class Beachline {
        public:
            Beachline(double *sweepline) : root(nullptr), sweepline(sweepline) {}
            ~Beachline() { deleteNode(root); }

            Leaf* findArc(const double x) const;
            double getValue(Node* _node) const;
            void makeSubtree(Node*& node, const cg3::Point2Dd& p, std::vector<Voronoi::HalfEdge>& edges);
            void addPoint(const cg3::Point2Dd& p, std::vector<Voronoi::HalfEdge>& edges);

            Node* getRoot() const;
            bool isLeaf(Node* node) const;
            void deleteNode(Node* node);
            bool isBalanced(const Node* node) const;
            void updateHeight(Node* node, const int plus);

        private:
            Node* root;
            double* sweepline;
    };

    inline Node* Beachline::getRoot() const {
        return root;
    }

    inline bool Beachline::isLeaf(Node* node) const {
        return node->left && node->right;
    }

    /**
     * @brief Beachline::isBalanced
     * @return true if the Beachline is balanced, false if not
     */
    inline bool Beachline::isBalanced(const Node* node) const {
        return node->left->height - node->right->height <= 1 &&
                node->left->height - node->right->height >= -1 ? true : false;
    }

}

#endif // BEACHLINE_H
