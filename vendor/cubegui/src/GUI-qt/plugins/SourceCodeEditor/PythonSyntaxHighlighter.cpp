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



#include "config.h"
#include <iostream>

using namespace std;

#include <QtGui>
#include "PythonSyntaxHighlighter.h"

PythonSyntaxHighlighter::PythonSyntaxHighlighter( QTextDocument* parent )
    : SyntaxHighlighter( parent )
{
    HighlightingRule rule;

    setCurrentBlockState( 0 );

    // quotation
    quotationFormat.setForeground( Qt::darkRed );
    quotationFormat.setFontItalic( true );
    rule.pattern = REGULAR_EXPRESSION( "\".*\"" );
    rule.format  = quotationFormat;
    highlightingRules.append( rule );

    // beginning of a function
    functionFormat.setFontItalic( true );
    functionFormat.setForeground( Qt::blue );
    rule.pattern = REGULAR_EXPRESSION( "\\b[A-Za-z0-9_]+(?=\\()" );
    rule.format  = functionFormat;
    highlightingRules.append( rule );

    // all python keywords
    keywordFormat.setForeground( Qt::darkBlue );
    keywordFormat.setFontWeight( QFont::Bold );

    QStringList keywordPatterns;
    keywordPatterns << "\\band\\b" << "\\bdel\\b" << "\\bfrom\\b" << "\\bnot\\b" << "\\bwhile\\b"
                    << "\\bas\\b" << "\\belif\\b" << "\\bglobal\\b" << "\\bor\\b" << "\\bwith\\b"
                    << "\\bassert\\b" << "\\belse\\b" << "\\bif\\b" << "\\bpass\\b" << "\\byield\\b"
                    << "\\bbreak\\b" << "\\bexcept\\b" << "\\bimport\\b" << "\\bprint\\b"
                    << "\\bclass\\b" << "\\bexec\\b" << "\\bin\\b" << "\\braise\\b"
                    << "\\bcontinue\\b" << "\\bfinally\\b" << "\\bis\\b" << "\\breturn\\b"
                    << "\\bdef\\b" << "\\bfor\\b" << "\\blambda\\b" << "\\btry\\b";

    foreach( const QString &pattern, keywordPatterns )
    {
#ifdef HAS_QREGULAR_EXPRESSION
        rule.pattern = QRegularExpression( pattern );
        rule.pattern.setPatternOptions( QRegularExpression::CaseInsensitiveOption );
#else
        rule.pattern = QRegExp( pattern, Qt::CaseInsensitive    );
#endif
        rule.format = keywordFormat;
        highlightingRules.append( rule );
    }

    // comments
    singleLineCommentFormat.setForeground( Qt::lightGray );
    singleLineCommentExpression = REGULAR_EXPRESSION( "#[^\n]*" );
}

void
PythonSyntaxHighlighter::highlightBlock( const QString& text )
{
    foreach( const HighlightingRule &rule, highlightingRules )
    {
        formatKeyword( text, rule.pattern, rule.format );
    }
    formatKeyword( text, singleLineCommentExpression, singleLineCommentFormat );
}
