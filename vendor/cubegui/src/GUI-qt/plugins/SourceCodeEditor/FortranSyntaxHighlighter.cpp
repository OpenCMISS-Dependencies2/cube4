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
#include <QtGui>
#include "FortranSyntaxHighlighter.h"

using namespace std;

FortranSyntaxHighlighter::FortranSyntaxHighlighter( QTextDocument* parent )
    : SyntaxHighlighter( parent )
{
    HighlightingRule rule;


    setCurrentBlockState( 0 );
    quotationFormat.setForeground( Qt::darkRed );
    quotationFormat.setFontItalic( true );
    rule.pattern = REGULAR_EXPRESSION( "\".*\"" );
    rule.format  = quotationFormat;
    highlightingRules.append( rule );

    functionFormat.setFontItalic( true );
    functionFormat.setForeground( Qt::blue );
    rule.pattern = REGULAR_EXPRESSION( "\\b[A-Za-z0-9_]+(?=\\()" );
    rule.format  = functionFormat;
    highlightingRules.append( rule );


    keywordFormat.setForeground( Qt::darkBlue );
    keywordFormat.setFontWeight( QFont::Bold );
    parallelFormat.setForeground( Qt::red );
    parallelFormat.setFontWeight( QFont::Normal );
    preprocessorFormat.setForeground( Qt::darkGray );
    preprocessorFormat.setFontWeight( QFont::Normal );
    preprocessorFormat.setFontItalic( true );
    QStringList keywordPatterns;
    QStringList parallelPatterns;
    QStringList preprocessorPatterns;
    keywordPatterns << "\\bcharacter\\b"
                    << "\\bdouble\\b"
                    << "\\binteger\\b"
                    << "\\bparameter\\b"
                    << "\\bexternal\\b"
                    << "\\bprecision\\b"
                    << "\\blogical\\b"
                    << "\\bcommon\\b"
                    << "\\bfor\\b"
                    << "\\bif\\b" << "\\bthen\\b" << "\\belse\\b" << "\\belseif\\b"
                    << "\\bendif\\b"  << "\\bswitch\\b"  << "\\bdo\\b"
                    << "\\bend\\s*do\\b" << "\\bend\\b"
                    << "\\bcase\\b" << "\\bcall\\b"  << "\\bsubroutine\\b"
                    << "\\bcontinue\\b"  << "\\bgoto\\b"
    ;
    parallelPatterns << "\\bmpi_[^(,\\s]*\\b"
    ;
    preprocessorPatterns << "^\\s*include\\s+\"[^\n]*\""
                         << "^\\s*include\\s+<[^\n]*>"
                         << "^\\s*include\\s+'[^\n]*'"
                         << "^\\s*define\\b"
    ;

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
    foreach( const QString &pattern, parallelPatterns )
    {
        rule.pattern = REGULAR_EXPRESSION( pattern );
        rule.format  = parallelFormat;
        highlightingRules.append( rule );
    }
    foreach( const QString &pattern, preprocessorPatterns )
    {
        rule.pattern = REGULAR_EXPRESSION( pattern );
        rule.format  = preprocessorFormat;
        highlightingRules.append( rule );
    }


    singleLineCommentFormat.setForeground( Qt::lightGray );
    singleLineCommentExpression1 = REGULAR_EXPRESSION( "^\\s*[cC]\\W+[^\n]*" );
    singleLineCommentExpression2 = REGULAR_EXPRESSION( "^\\s*![^$][^\n]*" );

    OpenMPFormat.setForeground( Qt::red );
    OpenMPFormatExpression = REGULAR_EXPRESSION( "\\s*!\\$OMP[^\n]*" );
}

void
FortranSyntaxHighlighter::highlightBlock( const QString& text )
{
    foreach( const HighlightingRule &rule, highlightingRules )
    {
        formatKeyword( text, rule.pattern, rule.format );
    }

    formatKeyword( text, singleLineCommentExpression1, singleLineCommentFormat );
    formatKeyword( text, singleLineCommentExpression2, singleLineCommentFormat );
    formatKeyword( text, OpenMPFormatExpression, OpenMPFormat );
}
