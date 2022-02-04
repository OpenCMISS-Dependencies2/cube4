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




#ifndef _CUBEPL_SYNTAX_HIGHLIGHTER_H
#define _CUBEPL_SYNTAX_HIGHLIGHTER_H


#include <QSyntaxHighlighter>
#include <QTextEdit>
#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#define REGULAR_EXPRESSION QRegularExpression
#else
#include <QRegExp>
#define REGULAR_EXPRESSION QRegExp
#endif

/*-------------------------------------------------------------------------*/
/**
 * @file  CubePLSyntaxHighlighter.h
 * @brief Declaration of the class CubePLSyntaxHighlighter
 *
 * This header file provides the declaration of the class HelpBrowser.
 */
/*-------------------------------------------------------------------------*/


namespace metric_editor
{
class CubePLSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    CubePLSyntaxHighlighter( QTextDocument* parent = 0 );


    void
    setErrorPlace( int err )
    {
        err_column = err;
    };

protected:

    void
    formatKeyword( const QString&            text,
                   const REGULAR_EXPRESSION& expression,
                   const QTextCharFormat&    format );

    /// Highlighting block
    void
    highlightBlock( const QString& block );

    int err_column;

    struct HighlightingRule
    {
        REGULAR_EXPRESSION pattern;
        QTextCharFormat    format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat errorFormat;
    QTextCharFormat operatorFormat;
    QTextCharFormat variablesFormat;
};
}
#endif
