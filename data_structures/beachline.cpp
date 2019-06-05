#include "beachline.h"
#include <iostream>

namespace Voronoi {

/**
     * @brief Beachline::findArc
     * @param x of the point to be added
     * @param _balance: stores the heights of each subtree
     * @param path: stores a path to the arc with -1 means moving to left child and 1 means moving to right child
     * @param diff: stores the differences in _balance
     * @return the arc that is vertically above the point
     */
    Leaf* Beachline::findArc(const double x,
                             std::vector<std::pair<int,int>>& _balance, std::vector<int>& path, int& diff) const {
        if(!root)
            return nullptr;
        else {
            Node* node = root;
            while(!isLeaf(node)) {
                if(x < getValue(node)) {
                    node = node->left;
                    path.push_back(-1);
                    if (!isLeaf(node)) {
                        diff += node->left->height - node->right->height;
                        _balance.push_back(std::pair<int,int>(node->left->height, node->right->height));
                    }
                } else {
                    node = node->right;
                    path.push_back(1);
                    if (!isLeaf(node)) {
                        diff += node->right->height - node->left->height;
                        _balance.push_back(std::pair<int,int>(node->right->height, node->left->height));
                    }
                }
            }
            return static_cast<Leaf*>(node);
        }
    }

    /**
     * @brief Beachline::getValue
     * @param _node which value is returned
     * @return value of the node:
     *  - if Leaf returns site.x
     *  - if InternalNode returns xBreakpoint, but if it is NaN the breakpoint is computed and then xBreakpoint is set
     */
    double Beachline::getValue(Node* _node) const {
        if(isLeaf(_node)) {
            return static_cast<Leaf*>(_node)->site->x();
        } else {
            InternalNode* node = static_cast<InternalNode*>(_node);
            if (!isnan(node->xBreakpoint)) {
                return node->xBreakpoint;
            } else {
                std::vector<cg3::Point2Dd> intPoints = Voronoi::findIntersectionPoint(*node->breakpoint.first, *node->breakpoint.second, *sweepline);

                if(intPoints.size() == 2) {
                    if (node->breakpoint.first->y() < node->breakpoint.second->y()) {
                        node->xBreakpoint = intPoints[0].x();
                        return intPoints[0].x();
                    } else {
                        node->xBreakpoint = intPoints[1].x();
                        return intPoints[1].x();
                    }
                } else {
                    node->xBreakpoint = intPoints[0].x();
                    return intPoints[0].x();
                }
            }
        }
    }

    /**
     * @brief Beachline::makeSubtree creates a subtree for a new siteEvent
     * @param node
     * @param p: the new site to be added to the Beachline
     */
    void Beachline::makeSubtree(Node*& node, const cg3::Point2Dd& p, std::vector<Voronoi::HalfEdge>& edges) {
        Leaf* leaf = static_cast<Leaf*>(node);
        InternalNode* newNode = new InternalNode(node->parent,
                                                 new Leaf(nullptr, nullptr, nullptr, leaf->site),
                                                 new InternalNode(nullptr,
                                                                  new Leaf(nullptr, nullptr, nullptr, &p),
                                                                  new Leaf(nullptr, nullptr, nullptr, leaf->site),
                                                                  1,
                                                                  std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*>(&p, leaf->site)),
                                                 2,
                                                 std::pair<const cg3::Point2Dd*, const cg3::Point2Dd*>(leaf->site, &p));
        //Connecting the parent for each new Node
        newNode->left->parent = newNode;
        newNode->right->parent = newNode;
        newNode->right->left->parent = newNode->right;
        newNode->right->right->parent = newNode->right;

        //HalfEdges creation and pointers added to each breakpoint in Beachline
        edges.push_back(Voronoi::HalfEdge(nullptr, nullptr, nullptr, nullptr));
        Voronoi::HalfEdge& edge1 = edges.back();
        edges.push_back(Voronoi::HalfEdge(nullptr, &edge1, nullptr, nullptr));
        Voronoi::HalfEdge& edge2 = edges.back();
        edge1.setTwin(edge2);

        newNode->edge = &edge1;
        static_cast<InternalNode*>(newNode->right)->edge = &edge2;

        if(!isRight(node))
            node->parent->left = newNode;
        else
            node->parent->right = newNode;

        delete node;
        node = newNode;
    }

    /**
     * @brief Beachline::addPoint
     * @param point to be added to the beachline
     */
    void Beachline::addPoint(const cg3::Point2Dd &p, std::vector<Voronoi::HalfEdge>& edges) {
        if(!root) {
            root = new Leaf(&p);
        } else {
            //stores the balance of the children for each node visited to find the arc (pair.first takes the
            std::vector<std::pair<int,int>> _balance;
            //stores the path to the arc (-1 for left child, 1 for right child)
            std::vector<int> path;
            //store the differences of balance, diff += pair.first - pair.second, for each pair
            int diff = 0;
            Node* arc = findArc(p.x(), _balance, path, diff);
            makeSubtree(arc, p, edges);
            //Here heights of the tree have not been updated yet
            handleRotation(arc, _balance, path, diff);
        }
    }

    void Beachline::handleRotation(Node *&arc,
                                   std::vector<std::pair<int, int> > &_balance, std::vector<int> &path, int diff) {
        if(arc->height == 1) {
            if(isRight(arc))
                rotateLeft(arc->right);
            else
                rotateLeftRight(arc->right);
        } else {
            if(!_balance.empty()) {
                //TODO
            } else
                arc->parent->height += 1;
        }
    }

    /**
     * @brief Beachline::rotateLeft
     * @param node as center of rotation
     */
    void Beachline::rotateLeft(Node *&node) {
        node->parent->right = node->left;
        if(node->left)
            node->left->parent = node->parent;
        node->left = node->parent;
        Node* temp = node->parent->parent;
        node->parent->parent = node;
        node->parent = temp;
    }

    /**
     * @brief Beachline::rotateRight
     * @param node as center of rotation
     */
    void Beachline::rotateRight(Node *&node) {
        node->parent->left = node->right;
        node->right->parent = node->parent;
        node->right = node->parent;
        Node* temp = node->parent->parent;
        node->parent->parent = node;
        node->parent = temp;
    }

    void Beachline::rotateLeftRight(Node *&node) {
        rotateLeft(node);
        rotateRight(node);
    }

    void Beachline::rotateRightLeft(Node *&node) {
        rotateRight(node);
        rotateLeft(node);
    }

    /**
     * @brief Beachline::deleteNode deletes the subtree with node as root
     * @param node
     */
    void Beachline::deleteNode(Node *node) {
        if(node) {
            deleteNode(node->left);
            deleteNode(node->right);
            delete node;
        }
    }

    /**
     * @brief Beachline::inorder Inorder visit of the Beachline
     * @param node: root of the tree to visit
     */
    void Beachline::inorder(Node *node) const {
        if(node) {
            inorder(node->left);
            inorder(node->right);
            if(isLeaf(node))
                std::cout<<static_cast<Leaf*>(node)->site;
            else {
                InternalNode* print = static_cast<InternalNode*>(node);
                std::cout<<"<"<<print->breakpoint.first<<"> <"<<print->breakpoint.second<<">";
            }
        }
    }

}
