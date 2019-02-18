/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#pragma once

#include "parser/diffeq/diffeq_parser.hpp"

/** Flex expects the declaration of yylex to be defined in the macro YY_DECL
 * and C++ parser class expects it to be declared. */
#ifndef YY_DECL
#define YY_DECL diffeq::Parser::symbol_type diffeq::Lexer::next_token()
#endif

/** For creating multiple (different) lexer classes, we can use '-P' flag
 * (or prefix option) to rename each yyFlexLexer to some other name like
 * ‘xxFlexLexer’. And then include <FlexLexer.h> in other sources once per
 * lexer class, first renaming yyFlexLexer as shown below. */
#ifndef __FLEX_LEXER_H
#define yyFlexLexer DiffEqFlexLexer
#include "FlexLexer.h"
#endif

namespace diffeq {

/**
 * \class Lexer
 * \brief Represent Lexer/Scanner class for differential equation parsing
 *
 * Lexer defined to add some extra function to the scanner class from flex.
 * At the moment we are using basic functionality but it could be easily
 * extended for further development.
 */
class Lexer: public DiffEqFlexLexer {
  public:
    /// for tracking location of the tokens
    location loc;

    /** The streams in and out default to cin and cout, but that assignment
     * is only made when initializing in yylex(). */
    Lexer(std::istream* in = nullptr, std::ostream* out = nullptr)
        : DiffEqFlexLexer(in, out) {}

    ~Lexer() override = default;
    ;

    /** Main lexing function generated by flex according to the macro declaration
     * YY_DECL above. The generated bison parser then calls this virtual function
     * to fetch new tokens. Note that yylex() has different declaration and hence
     * it can't be used for new lexer. */
    virtual Parser::symbol_type next_token();
};

}  // namespace diffeq
