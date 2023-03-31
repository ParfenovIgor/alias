#ifndef VALIDATOR_H_INCLUDED
#define VALIDATOR_H_INCLUDED

#include <set>
#include "ast.h"

namespace AST {
    /* class CContext {
        private:
        std::vector < std::vector <std::pair <std::string, int> > > S_variable_stack;
        std::vector < std::vector <Type> > S_variable_type;
        std::vector <int> S_variable_index;
        std::vector < std::vector <int> > S_heap_size;
        std::vector <int> S_heap_index;
        std::vector < std::vector <std::pair <std::string, std::shared_ptr <FunctionSignature> > > > S_function_stack;
        std::vector <int> S_function_index;
        std::vector < std::set <State> > S_states;

        int size = 0;

        public:
        std::vector <std::pair <std::string, int> > variable_stack; // {name, index}
        std::vector <Type> variable_type;
        int variable_index = 0;
        std::vector <int> heap_size;
        int heap_index = 0;
        std::vector <std::pair <std::string, std::shared_ptr <FunctionSignature> > > function_stack;
        int function_index = 0;
        std::set <State> states;

        void push() {
            if (size) {
                S_variable_stack.push_back(variable_stack);
                S_variable_type.push_back(variable_type);
                S_variable_index.push_back(variable_index);
                S_heap_size.push_back(heap_size);
                S_heap_index.push_back(heap_index);
                S_function_stack.push_back(function_stack);
                S_function_index.push_back(function_index);
                S_states.push_back(states);
            }

            variable_stack.clear();
            variable_type.clear();
            variable_index = 0;
            heap_size.clear();
            heap_index = 0;
            function_stack.clear();
            function_index = 0;
            states.clear();
            State state;
            for (int i = 0; i < 10; i++) {
                state.heap.push_back({-1, 0});
            }
            states.insert(state);
            size++;
        }

        void pop() {
            variable_stack = S_variable_stack.back();
            variable_type = S_variable_type.back();
            variable_index = S_variable_index.back();
            heap_size = S_heap_size.back();
            heap_index = S_heap_index.back();
            function_stack = S_function_stack.back();
            function_index = S_function_index.back();
            states = S_states.back();

            S_variable_stack.pop_back();
            S_variable_type.pop_back();
            S_variable_index.pop_back();
            S_heap_size.pop_back();
            S_heap_index.pop_back();
            S_function_stack.pop_back();
            S_function_index.pop_back();
            S_states.pop_back();
            size--;
        }
    };

    extern CContext Context; */

    void Validate(std::shared_ptr <Node> node);

    void PrintStatesLog();
}

#endif // VALIDATOR_H_INCLUDED
