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


#ifndef _SYNTAX_HIGHLIGHTER_H
#define _SYNTAX_HIGHLIGHTER_H


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
 * @file  SyntaxHighlighter.h
 * @brief Declaration of the class CubePLSyntaxHighlighter
 *
 * This header file provides the declaration of the class Source Code Editor.
 */
/*-------------------------------------------------------------------------*/

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter( QTextDocument* parent );
    const int MULTILINE_COMMENT = 1;

protected:
    void
    formatKeyword( const QString&            text,
                   const REGULAR_EXPRESSION& expression,
                   const QTextCharFormat&    format );
    void
    formatBlock( const QString&            text,
                 const REGULAR_EXPRESSION& startExp,
                 const REGULAR_EXPRESSION& endExp,
                 const QTextCharFormat&    format );
};

#endif
