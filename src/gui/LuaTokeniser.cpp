// Copyright 2023 Kushview, LLC <info@kushview.net>
// SPDX-License-Identifier: GPL3-or-later

#include "gui/LuaTokeniser.h"
using namespace juce;

namespace element {

struct LuaTokeniserFunctions
{
    static bool isControlStatement (String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char* const keywords2Char[] = { "if", "in", "do", nullptr };

        static const char* const keywords3Char[] = { "end", "for", "not", "and", nullptr };

        static const char* const keywords4Char[] = { "then", "else", nullptr };

        static const char* const keywords5Char[] = { "until", "while", "break", nullptr };

        static const char* const keywords6Char[] = { "repeat", "return", "elseif", nullptr };

        static const char* const keywordsOther[] = { "function", "require", "@interface", "@end", "@synthesize", "@dynamic", "@public", "@private", "@property", "@protected", "@class", nullptr };

        const char* const* k;

        switch (tokenLength)
        {
            case 2:
                k = keywords2Char;
                break;
            case 3:
                k = keywords3Char;
                break;
            case 4:
                k = keywords4Char;
                break;
            case 5:
                k = keywords5Char;
                break;
            case 6:
                k = keywords6Char;
                break;

            default:
                if (tokenLength < 2 || tokenLength > 16)
                    return false;

                k = keywordsOther;
                break;
        }

        for (int i = 0; k[i] != nullptr; ++i)
            if (token.compare (CharPointer_ASCII (k[i])) == 0)
                return true;

        return false;
    }

    static bool isReservedKeyword (String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char* const keywords2Char[] = { nullptr };

        static const char* const keywords3Char[] = { "nil", nullptr };

        static const char* const keywords4Char[] = { "then", "true", "else", "self", nullptr };

        static const char* const keywords5Char[] = { "false", "local", "until", "while", "break", nullptr };

        static const char* const keywords6Char[] = { "repeat", "return", "elseif", nullptr };

        static const char* const keywordsOther[] = { "function", "@interface", "@end", "@synthesize", "@dynamic", "@public", "@private", "@property", "@protected", "@class", nullptr };

        const char* const* k;

        switch (tokenLength)
        {
            case 2:
                k = keywords2Char;
                break;
            case 3:
                k = keywords3Char;
                break;
            case 4:
                k = keywords4Char;
                break;
            case 5:
                k = keywords5Char;
                break;
            case 6:
                k = keywords6Char;
                break;

            default:
                if (tokenLength < 2 || tokenLength > 16)
                    return false;

                k = keywordsOther;
                break;
        }

        for (int i = 0; k[i] != nullptr; ++i)
            if (token.compare (CharPointer_ASCII (k[i])) == 0)
                return true;

        return false;
    }

    template <typename Iterator>
    static int parseIdentifier (Iterator& source) noexcept
    {
        int tokenLength = 0;
        String::CharPointerType::CharType possibleIdentifier[100]= {0 };
        String::CharPointerType possible (possibleIdentifier);

        while (CppTokeniserFunctions::isIdentifierBody (source.peekNextChar()))
        {
            auto c = source.nextChar();

            if (tokenLength < 20)
                possible.write (c);

            ++tokenLength;
        }

        if (tokenLength > 1 && tokenLength <= 16)
        {
            possible.writeNull();

            if (isControlStatement (String::CharPointerType (possibleIdentifier), tokenLength))
                return LuaTokeniser::tokenType_preprocessor;
            else if (isReservedKeyword (String::CharPointerType (possibleIdentifier), tokenLength))
                return LuaTokeniser::tokenType_keyword;
        }

        return LuaTokeniser::tokenType_identifier;
    }

    template <typename Iterator>
    static void skipComment (Iterator& source) noexcept
    {
        int dashes = 0, brackets = 0;
        bool lastWasDash = false, lastWasBracket = false;
        for (;;)
        {
            auto c = source.nextChar();

            if (c == 0 || (dashes == 2 && brackets == 2))
                break;

            if (c == '-')
            {
                dashes = lastWasDash ? 2 : 1;
                brackets = 0;
                lastWasDash = true;
                lastWasBracket = false;
            }
            else if (c == ']' && dashes == 2)
            {
                brackets = lastWasBracket ? 2 : 1;
                lastWasDash = false;
                lastWasBracket = true;
            }
            else
            {
                lastWasDash = lastWasBracket = false;
                dashes = brackets = 0;
            }
        }
    }

    template <typename Iterator>
    static int readNextToken (Iterator& source)
    {
        source.skipWhitespace();

        auto firstChar = source.peekNextChar();

        switch (firstChar)
        {
            case 0:
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.': {
                auto result = CppTokeniserFunctions::parseNumber (source);

                if (result == LuaTokeniser::tokenType_error)
                {
                    source.skip();

                    if (firstChar == '.')
                        return LuaTokeniser::tokenType_punctuation;
                }

                return result;
            }

            case ',':
            case ';':
            case ':':
                source.skip();
                return LuaTokeniser::tokenType_punctuation;

            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
                source.skip();
                return LuaTokeniser::tokenType_bracket;

            case '"':
            case '\'':
                CppTokeniserFunctions::skipQuotedString (source);
                return LuaTokeniser::tokenType_string;

            case '+':
                source.skip();
                CppTokeniserFunctions::skipIfNextCharMatches (source, '+', '=');
                return LuaTokeniser::tokenType_operator;

            case '-': {
                source.skip();
                auto result = CppTokeniserFunctions::parseNumber (source);

                if (source.peekNextChar() == '-')
                {
                    source.skip();
                    if (source.peekNextChar() == '[')
                    {
                        source.skip();
                        if (source.peekNextChar() == '[')
                            skipComment (source);
                        else
                            source.skipToEndOfLine();
                    }
                    else
                    {
                        source.skipToEndOfLine();
                    }

                    return LuaTokeniser::tokenType_comment;
                }

                if (result == LuaTokeniser::tokenType_error)
                {
                    CppTokeniserFunctions::skipIfNextCharMatches (source, '-', '=');
                    return LuaTokeniser::tokenType_operator;
                }

                return result;
            }

            case '*':
            case '%':
            case '=':
            case '!':
                source.skip();
                CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return LuaTokeniser::tokenType_operator;

            case '?':
            case '~':
                source.skip();
                return LuaTokeniser::tokenType_operator;

            case '<':
            case '>':
            case '|':
            case '&':
            case '^':
                source.skip();
                CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
                CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return LuaTokeniser::tokenType_operator;

            default:
                if (CppTokeniserFunctions::isIdentifierStart (firstChar))
                    return parseIdentifier (source);

                source.skip();
                break;
        }

        return LuaTokeniser::tokenType_error;
    }
};

//==============================================================================
LuaTokeniser::LuaTokeniser() {}
LuaTokeniser::~LuaTokeniser() {}

int LuaTokeniser::readNextToken (CodeDocument::Iterator& source)
{
    return LuaTokeniserFunctions::readNextToken (source);
}

CodeEditorComponent::ColourScheme LuaTokeniser::getDefaultColourScheme()
{
    static const CodeEditorComponent::ColourScheme::TokenType types[] = {
        { "Error", Colour (0xffcc0000) },
        { "Comment", Colour (0xff6a9955) },
        { "Keyword", Colour (0xff569cd6) },
        { "Operator", Colour (0xffb3b3b3) },
        { "Identifier", Colour (0xffc5c5c5) },
        { "Integer", Colour (0xffb5cea8) },
        { "Float", Colour (0xffb5cea8) },
        { "String", Colour (0xffce9178) },
        { "Bracket", Colour (0xffd4d4d4) },
        { "Punctuation", Colour (0xffb3b3b3) },
        { "Preprocessor Text", Colour (0xffc586c0) } // used for control statements
    };

    CodeEditorComponent::ColourScheme cs;

    for (auto& t : types)
        cs.set (t.name, Colour (t.colour));

    return cs;
}

} // namespace element
