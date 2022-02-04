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


#ifndef _CUBEPL_SYNTAX_HIGHLIGHTER_CPP
#define _CUBEPL_SYNTAX_HIGHLIGHTER_CPP


#include <QTextCharFormat>
#include <iostream>


#include "CubePLSyntaxHighlighter.h"

using namespace std;
using namespace metric_editor;

CubePLSyntaxHighlighter::CubePLSyntaxHighlighter( QTextDocument* parent )
    : QSyntaxHighlighter( parent )
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

    variablesFormat.setForeground( Qt::blue );
    variablesFormat.setFontWeight( QFont::Bold );
    QStringList variablesPatterns;
    variablesPatterns << "\\$\\{[\"\\w0-9:_-#]+\\}"
    ;

    keywordFormat.setForeground( Qt::darkBlue );
    keywordFormat.setFontWeight( QFont::Bold );
    QStringList keywordPatterns;
    keywordPatterns << "\\bwhile\\b"
                    << "\\bif\\b" << "\\belse\\b" << "\\belseif\\b"
                    << "\\breturn\\b"
                    << "\\bmetric::\\b"
                    << "\\bmetric::fixed\\b"
                    << "\\bcube::metric::prederived\\b"
                    << "\\bcube::metric::prederived\\b"
                    << "\\bcube::metric::postderived\\b"
                    << "\\bcube::metric::init\\b"
                    << "\\bcube::metric::set\\b"
                    << "\\bcube::metric::get\\b"
                    << "\\bsizeof\\b"
                    << "\\bdefined\\b"
                    << "\\blocal\\b"
                    << "\\bglobal\\b"
                    << "\\barg1\\b"
                    << "\\barg2\\b"
    ;

    operatorFormat.setForeground( Qt::green );
    operatorFormat.setFontWeight( QFont::Normal );
    QStringList operatorPatterns;
    operatorPatterns << "="
                     << "=="
                     << "!="
                     << "=~"
                     << "<="
                     << ">="
                     << "<"
                     << ">"
                     << "\\+"
                     << "\\-"
                     << "\\*"
                     << "\\/"
                     << "\\^"
                     << "\\bnot\\b"
                     << "\\bxor\\b"
                     << "\\bor\\b"
                     << "\\band\\b"
                     << "\\beq\\b"
                     << "\\bseq\\b"
                     << "&&"
                     << "\\|\\|"
                     << "\\<<"
                     << "\\>>"
    ;



    foreach( const QString &pattern, keywordPatterns )
    {
        rule.pattern = REGULAR_EXPRESSION( pattern );
        rule.format  = keywordFormat;
        highlightingRules.append( rule );
    }

    foreach( const QString &pattern, operatorPatterns )
    {
        rule.pattern = REGULAR_EXPRESSION( pattern );
#ifdef HAS_QREGULAR_EXPRESSION
        rule.pattern.setPatternOptions( QRegularExpression::CaseInsensitiveOption );
#else
        rule.pattern.setCaseSensitivity( Qt::CaseInsensitive );
#endif
        rule.format = operatorFormat;
        highlightingRules.append( rule );
    }

    foreach( const QString &pattern, variablesPatterns )
    {
        rule.pattern = REGULAR_EXPRESSION( pattern );
        rule.format  = variablesFormat;
        highlightingRules.append( rule );
    }
    // Format for error
    errorFormat.setFontWeight( QFont::Bold );
    errorFormat.setForeground( Qt::red );
}


#ifdef HAS_QREGULAR_EXPRESSION
/**
 * @brief SyntaxHighlighter::formatKeyword formats all occurences of exp within text with the given format
 * @param text text to search for exp
 * @param exp regular expression to highlight
 * @param format each match of the regular expression is formatted with the given format
 * @see EditorPlugin
 */
void
CubePLSyntaxHighlighter::formatKeyword( const QString& text, const QRegularExpression& exp, const QTextCharFormat& format )
{
    QRegularExpressionMatchIterator it = exp.globalMatch( text );

    while ( it.hasNext() )
    {
        QRegularExpressionMatch match = it.next();
        int                     start = match.capturedStart();
        int                     end   = match.capturedEnd();
        setFormat( start, end - start, format );
    }
}
#else
void
CubePLSyntaxHighlighter::formatKeyword( const QString& text, const QRegExp& exp, const QTextCharFormat& format )
{
    REGULAR_EXPRESSION expression( exp );
    int                index = expression.indexIn( text );
    while ( index >= 0 )
    {
        int length = expression.matchedLength();
        setFormat( index, length, format );
        index = expression.indexIn( text, index + length );
    }
}
#endif

void
CubePLSyntaxHighlighter::highlightBlock( const QString& text )
{
    foreach( const HighlightingRule &rule, highlightingRules )
    {
        formatKeyword( text, rule.pattern, rule.format );
    }
    // set format for the error at the end.
    setFormat( err_column, text.length() - err_column, errorFormat );
}

#endif
