#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
  // TODO: initialize member variable(s) (if necessary)
{
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  return stack.empty();
}

void ValueStack::push( const std::string &value )
{
  stack.push_back(value); // push to top of stack (end of vector)
}

std::string ValueStack::get_top() const
{
  // throw exception if empty
  if(stack.empty()){
    throw OperationException("Stack is empty.");
  }

  return stack.back(); // top of stack = back of vector
}

void ValueStack::pop()
{
  // throw exception if empty
  if(stack.empty()){
    throw OperationException("Stack is empty.");
  }

  stack.pop_back(); // pop off top of stack (end of vector)
}
