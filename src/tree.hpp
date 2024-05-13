#include <string>
#include <map>
#include <memory>

namespace myCalc {

    /**
     * Abstract Node class, can always be evaluated to a double value.
    */
    class Node {
    public:
        virtual ~Node(){ };

        // Calculate and return the value represented by the node.
        virtual double evaluate() = 0 ;

        static std::map<char, int> operatorPriorities;
        typedef double (*Operator)(double, double);
        static std::map<char, Operator> operatorFunctions;

    protected:
        Node(){};  // Should never be able to make a node directly, so constructor is protected.

    private:
        Node * ParseString(std::string_view instr);
    };

    /**
     * Value Node, essentially just stores a double value.
    */
    class ValueNode : public Node {
        public:
            // Construct a ValueNode containing the given value.
            ValueNode(double value);

            double evaluate() override {
                return value;
            };

        protected:
            double value;
    };

    class OperationNode : public Node {
        public:
            ~OperationNode() {}

            /**
             * Create a new Operations Node
             * 
             * @param operation The operation type the node will perform on the values of the child nodes.
             * @param leftNode Left child node.
             * @param rightNode Right child node.
            */
            OperationNode(double (&operation)(double, double), Node *left, Node *right);

            double evaluate() override;

            /**-------------------
             * Operation Functions
             * -------------------
            */

            static double add(double left, double right);
            static double subtract(double left, double right);
            static double multiply(double left, double right);
            static double divide(double left, double right);
            static double power(double left, double right);

        private:
            double (*operation)(double, double);   // Pointer to operation function the node will perform on children.
            std::unique_ptr<Node> left;     // Left child node of this node.
            std::unique_ptr<Node> right;    // Right child node of this node.
    };
}