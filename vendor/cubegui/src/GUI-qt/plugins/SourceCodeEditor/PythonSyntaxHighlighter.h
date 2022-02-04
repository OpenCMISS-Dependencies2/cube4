/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef _PYTHON_SYNTAX_HIGHLIGHTER_H
#define _PYTHON_SYNTAX_HIGHLIGHTER_H


#include <QSyntaxHighlighter>
#include <QTextEdit>
#include "SyntaxHighlighter.h"

/*-------------------------------------------------------------------------*/
/**
 * @file  PythonSyntaxHighlighter.h
 * @brief Declaration of the class PythonSyntaxHighlighter
 *
 * This header file provides the declaration of the class Source Code Editor.
 */
/*-------------------------------------------------------------------------*/


class PythonSyntaxHighlighter : public SyntaxHighlighter
{
public:
    PythonSyntaxHighlighter( QTextDocument* parent = 0 );

protected:
    void
    highlightBlock( const QString& text );

private:
    struct HighlightingRule
    {
        REGULAR_EXPRESSION pattern;
        QTextCharFormat    format;
    };
    QVector<HighlightingRule> highlightingRules;

    REGULAR_EXPRESSION singleLineCommentExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif
