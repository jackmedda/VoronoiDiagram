#ifndef BEACHLINE_H
#define BEACHLINE_H

class Event;
class HalfEdge;

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

        Node() : parent(nullptr), left(nullptr), right(nullptr) {}
        Node(Node* parent, Node* left, Node* right) :
            parent(parent), left(left), right(right) {}
    };

    /**
     * @brief The InternalNode struct, the type for internal nodes
     */
    struct InternalNode : Node {
        HalfEdge* edge;
        std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint;
        double xBreakpoint;

        InternalNode(Node* parent, Node* left, Node* right, std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*> breakpoint) :
            Node(parent, left, right), edge(nullptr), breakpoint(breakpoint), xBreakpoint(nan("")) {}
        InternalNode(Node* parent, Node* left, Node* right, std::pair<cg3::Point2Dd*, cg3::Point2Dd*> breakpoint, double xBreakpoint) :
            Node(parent, left, right), edge(nullptr), breakpoint(breakpoint), xBreakpoint(xBreakpoint) {}
    };

    /**
     * @brief The Leaf struct, the type for leaves
     */
    struct Leaf : Node {
        const cg3::Point2Dd* site;
        Event* circleEvent;

        Leaf(const cg3::Point2Dd* site) : Node(), site(site), circleEvent(nullptr) {}
        Leaf(Node* parent, Node* left, Node* right, const cg3::Point2Dd* site) :
            Node(parent, left, right), site(site), circleEvent(nullptr) {}
    };

    /**
     * @brief The Beachline class, implemented as a balanced BST
     * @class Beachline
     */
    class Beachline {
        public:
            Beachline(double *sweepline) : root(nullptr), sweepline(sweepline) {}
            ~Beachline() { deleteNode(root); }

            Node* getRoot() const;
            bool isLeaf(Node* node) const;
            void deleteNode(Node* node);
            Leaf* findArc(const double x) const;
            double getValue(Node* _node) const;
            void makeSubtree(Node* node, const cg3::Point2Dd& p);
            void addPoint(const cg3::Point2Dd& p);

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

}

#endif // BEACHLINE_H
