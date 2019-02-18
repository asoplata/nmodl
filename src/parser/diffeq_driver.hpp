/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#pragma once

#include <string>

#include "parser/diffeq_context.hpp"


namespace diffeq {
/**
 * \class Driver
 * \brief Class that binds all pieces together for parsing differential equations
 *
 * Driver class bind components required for lexing, parsing and ast
 * generation for differential equations.
 */

/// flex generated scanner class (extends base lexer class of flex)
class Lexer;

/// parser class generated by bison
class Parser;

class Driver {
  private:
    std::string solve_equation(std::string& state,
                               int order,
                               std::string& rhs,
                               std::string& method,
                               bool& cnexp_possible,
                               bool debug = false);

    /// parse given equation into lhs, rhs and find it's order and state variable
    void parse_equation(const std::string& equation,
                        std::string& state,
                        std::string& rhs,
                        int& order);

  public:
    Driver() = default;

    /// solve equation using provided method
    std::string solve(const std::string& equation, std::string method, bool debug = false);

    /// check if given equation can be solved using cnexp method
    bool cnexp_possible(const std::string& equation, std::string& solution);
};

}  // namespace diffeq
