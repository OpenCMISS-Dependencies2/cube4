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



#ifndef _FORTRAN_SYNTAX_HIGHLIGHTER_H
#define _FORTRAN_SYNTAX_HIGHLIGHTER_H


#include <QSyntaxHighlighter>
#include <QTextEdit>
#include "SyntaxHighlighter.h"

/*-------------------------------------------------------------------------*/
/**
 * @file  FortranSyntaxHighlighter.h
 * @brief Declaration of the class FortranSyntaxHighlighter
 *
 * This header file provides the declaration of the class Source Code Editor.
 */
/*-------------------------------------------------------------------------*/


class FortranSyntaxHighlighter : public SyntaxHighlighter
{
public:
    FortranSyntaxHighlighter( QTextDocument* parent = 0 );

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

    REGULAR_EXPRESSION commentStartExpression;
    REGULAR_EXPRESSION commentEndExpression;
    REGULAR_EXPRESSION singleLineCommentExpression1;
    REGULAR_EXPRESSION singleLineCommentExpression2;
    REGULAR_EXPRESSION OpenMPFormatExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat parallelFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat OpenMPFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif
