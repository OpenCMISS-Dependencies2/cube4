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

#include <QtGui>

#include "CPPSyntaxHighlighter.h"

CPPSyntaxHighlighter::CPPSyntaxHighlighter( QTextDocument* parent )
    : SyntaxHighlighter( parent )
{
    HighlightingRule rule;



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
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b"  << "\\bfor\\b"
                    << "\\bif\\b" << "\\belse\\b"  << "\\bswitch\\b"
                    << "\\bcase\\b"   << "\\bdefault\\b"  << "\\bbreak\\b"
                    << "\\bcontinue\\b"
    ;
    parallelPatterns << "\\bMPI_[^(,\\s]*\\b"
    ;
    preprocessorPatterns << "^\\s*#\\s*include\\s+\"[^\n]*\""
                         << "^\\s*#\\s*include\\s+<[^\n]*>"
                         << "^\\s*#\\s*define\\b"
                         << "^\\s*#\\s*ifdef\\b"
                         << "^\\s*#\\s*ifndef\\b"
                         << "^\\s*#\\s*if\\b"
                         << "^\\s*#\\s*defined\\b"
                         << "^\\s*#\\s*endif\\b"
                         << "^\\s*#\\s*else\\b"
    ;

    foreach( const QString &pattern, keywordPatterns )
    {
        rule.pattern = REGULAR_EXPRESSION( pattern );
        rule.format  = keywordFormat;
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
    singleLineCommentExpression = REGULAR_EXPRESSION( "//[^\n]*" );
    OpenMPFormat.setForeground( Qt::red );
    OpenMPFormatExpression = REGULAR_EXPRESSION( "\\s*#\\s*pragma\\s+[p]?omp[^\n]*" );
//      rule.format = singleLineCommentFormat;
//      highlightingRules.append(rule);

    multiLineCommentFormat.setForeground( Qt::lightGray );


    commentStartExpression = REGULAR_EXPRESSION( "/\\*" );
    commentEndExpression   = REGULAR_EXPRESSION( "\\*/" );
}

void
CPPSyntaxHighlighter::highlightBlock( const QString& text )
{
    foreach( const HighlightingRule &rule, highlightingRules )
    {
        formatKeyword( text, rule.pattern, rule.format );
    }
    if ( previousBlockState() != MULTILINE_COMMENT )
    {
        formatKeyword( text, singleLineCommentExpression, singleLineCommentFormat );
        formatKeyword( text, OpenMPFormatExpression, OpenMPFormat );
    }
    formatBlock( text, commentStartExpression, commentEndExpression, multiLineCommentFormat );
}
