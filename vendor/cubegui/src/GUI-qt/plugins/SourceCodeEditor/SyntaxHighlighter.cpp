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

#include "SyntaxHighlighter.h"

SyntaxHighlighter::SyntaxHighlighter( QTextDocument* parent )
    : QSyntaxHighlighter( parent )
{
}

#ifdef HAS_QREGULAR_EXPRESSION

/**
 * @brief SyntaxHighlighter::formatKeyword formats all occurences of exp within text with the given format
 * @param text text to search for exp
 * @param exp regular expression to highlight
 * @param format each match of the regular expression is formatted with the given format
 */
void
SyntaxHighlighter::formatKeyword( const QString& text, const QRegularExpression& exp, const QTextCharFormat& format )
{
    QRegularExpressionMatchIterator it = exp.globalMatch( text );

    while ( it.hasNext() )
    {
        QRegularExpressionMatch match = it.next();
        if ( currentBlockState() != 1 )
        {
            int start = match.capturedStart();
            int end   = match.capturedEnd();
            setFormat( start, end - start, format );
        }
    }
}

/** SyntaxHighlighter::formatBlock searches in text and formats all blocks which start with startExp and end with endExp with the given format.
 * Marks unfinished blocks with setCurrentBlockState(MULTILINE_COMMENT).
 */
void
SyntaxHighlighter::formatBlock( const QString& text, const QRegularExpression& startExp, const QRegularExpression& endExp, const QTextCharFormat& format )
{
    setCurrentBlockState( 0 );

    int startIndex = 0;
    if ( previousBlockState() != 1 )
    {
        startIndex = text.indexOf( startExp );
    }

    int commentLength;
    while ( startIndex >= 0 )
    {
        QRegularExpressionMatch endMatch;
        int                     endIndex = text.indexOf( endExp, startIndex, &endMatch );

        if ( endIndex == -1 )
        {
            setCurrentBlockState( MULTILINE_COMMENT );
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat( startIndex, commentLength, format );
        startIndex = text.indexOf( startExp, startIndex + commentLength );
    }
}
#else
void
SyntaxHighlighter::formatKeyword( const QString& text, const QRegExp& exp, const QTextCharFormat& format )
{
    REGULAR_EXPRESSION expression( exp );
    int                index = expression.indexIn( text );
    while ( index >= 0 )
    {
        int length = expression.matchedLength();
        if ( currentBlockState() != 1 )
        {
            setFormat( index, length, format );
        }
        index = expression.indexIn( text, index + length );
    }
}

void
SyntaxHighlighter::formatBlock( const QString& text, const QRegExp& startExp, const QRegExp& endExp, const QTextCharFormat& format )
{
    setCurrentBlockState( 0 );

    int startIndex = 0;
    if ( previousBlockState() != 1 )
    {
        startIndex = startExp.indexIn( text );
    }

    int commentLength;
    while ( startIndex >= 0 )
    {
        int endIndex = endExp.indexIn( text, startIndex );

        if ( endIndex == -1 )
        {
            setCurrentBlockState( MULTILINE_COMMENT );
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + endExp.matchedLength();
        }
        setFormat( startIndex, commentLength, format );
        startIndex = text.indexOf( startExp, startIndex + commentLength );
    }
}
#endif
