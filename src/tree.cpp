#include "tree.hpp"
#include <cctype>
#include <charconv>
#include <iostream>
#include <exception>
#include <stack>
#include <cmath>

namespace myCalc
{

    std::map<char, int> Node::operatorPriorities{
        {'(', 1},
        {')', 1},
        {'+', 2},
        {'-', 2},
        {'*', 3},
        {'/', 3},
        {'^', 4}
    };

    std::map<char, Node::Operator> Node::operatorFunctions{
        {'+', &OperationNode::add},
        {'-', &OperationNode::subtract},
        {'*', &OperationNode::multiply},
        {'/', &OperationNode::divide},
        {'^', &OperationNode::power}
    };

    Node * Node::ParseString(std::string_view instr)
    {

        std::stack<char> charStack;
        std::stack<Node *> nodeStack;
        std::string_view instr_remain = instr.substr(0);  // Currently unparsed part of the input string

        while (instr_remain.size() > 0)
        {
            int imove = 1; // Number of characters scanned this iteration. Assume at least 1, it's better to always move forwards than get stuck.
            if (std::isdigit(instr_remain[0]))
            { // Parsing Numbers 

                // from_chars converts a string to a number as far as possible,
                // and puts the value in "value", and gives the end pointer in res.ptr.
                double value = 0;
                const auto &res = std::from_chars(instr_remain.data(),
                                                  instr_remain.data() + instr_remain.size(),
                                                  value);

                // Handle errors from from_chars.
                if (res.ec != std::errc())
                {
                    auto const ec = std::make_error_code(res.ec);
                    std::cerr << "err: " << ec.message() << '\n';
                    throw 1; // I don't know if there's a better way to throw an exception from an errc.
                }

                nodeStack.push(new ValueNode(value));

                imove = res.ptr - instr_remain.data(); // End of number pointer - start of string pointer = end of number index
            }
            else
            { // Parsing Operators

                auto result = Node::operatorPriorities.find(instr_remain[0]);

                // Test if operator is invalid using priority map
                if (result == Node::operatorPriorities.end())
                { // Invalid
                    throw std::invalid_argument("received an invalid operator: " + instr_remain[0]);
                }

                int priority = result->second;

                while (!charStack.empty() && Node::operatorPriorities[charStack.top()] >= priority)
                    { // Current queued operator priority is less than last operator priority - collapse the stack

                        if (instr_remain[0] == '(')
                        { // If it's an open bracket, don't do anything just push to stack.
                            break;
                        }

                        char op = charStack.top();
                        charStack.pop();

                        if (Node::operatorPriorities[op] > 1) 
                        {  // Don't put brackets in as actual node operators.

                            // Last in first out stack means we get the right side before left.
                            Node *rightNode = nodeStack.top();
                            nodeStack.pop();
                            Node *leftNode = nodeStack.top();
                            nodeStack.pop();

                            nodeStack.push(new OperationNode(*Node::operatorFunctions[op], leftNode, rightNode));
                        }
                    }
                charStack.push(instr_remain[0]);
            }
            instr_remain = instr_remain.substr(imove);
        }

        while (nodeStack.size() > 1)
        { // Collapse remaining nodes
            Node * rightNode = nodeStack.top();
            nodeStack.pop();
            Node * leftNode = nodeStack.top();
            nodeStack.pop();

            char op = charStack.top();
            charStack.pop();

            if (Node::operatorPriorities[op] > 1)
            { // Don't put brackets in as actual node operators.
                nodeStack.push(new OperationNode(*Node::operatorFunctions[op], leftNode, rightNode));
            }
        }

        return nodeStack.top();  // There's only 1 node left - the top of the tree.
    }

    ValueNode::ValueNode(double val) {
        value = val;
    }

    OperationNode::OperationNode(double (&op)(double left, double right), Node *leftNode, Node *rightNode)
    {
        operation = &op;  // Since ops are functions not objects, we don't have to be so memory-careful (I hope?)
        left = std::unique_ptr<Node>(leftNode);  // & addressof operator gets address of reference for unique_ptr
        right = std::unique_ptr<Node>(rightNode);
    }

    double OperationNode::evaluate() {
        return operation(left->evaluate(), right->evaluate());
    }

    double OperationNode::add(double left, double right) {
        return left + right;
    }
    double OperationNode::subtract(double left, double right) {
        return left - right;
    }
    double OperationNode::multiply(double left, double right) {
        return left * right;
    }
    double OperationNode::divide(double left, double right) {
        return left / right;
    }
    double OperationNode::power(double left, double right) {
        return std::pow(left, right);
    }
}