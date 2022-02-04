/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef DERIVEDMETRICEDITOR_H
#define DERIVEDMETRICEDITOR_H
#include <QTextEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QKeyEvent>
#include <QStandardItemModel>

namespace metric_editor
{
class DerivedMetricEditor : public QTextEdit
{
    Q_OBJECT
public:
    DerivedMetricEditor();

    void
    setKeywords( const QStringList& keywords,
                 QHash<QString, QString>& labelHash  );

    QString
    tokenLeftToCursor() const;

protected:
    void
    keyPressEvent( QKeyEvent* e );

private slots:
    void
    insertCompletion( const QString& completion );

private:
    void
    updateCompletationKeywords( const QString& pre );
    void
    checkCompletion( bool showAlways = false );
    bool
    isVariableLeftToCursor() const;

    QStandardItemModel      completerModel;
    QCompleter*             comp;
    QStringList             keywords;
    QHash<QString, QString> keywordLabelHash;
    int                     colons;     // number of double colons in current token
    bool                    isVariable; // current token is a variable
};
}
#endif // DERIVEDMETRICEDITOR_H
