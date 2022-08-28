#ifndef IMAGEDELEGATE_H
#define IMAGEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QPixmap>
#include <QDebug>

class ImageDelegate : public QStyledItemDelegate
{
public:
    explicit ImageDelegate(QStandardItemModel *model, QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QStandardItemModel* m_model;
};

class ImageDelegateColumn : public QStyledItemDelegate
{
public:
    explicit ImageDelegateColumn(QStandardItemModel *model, QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        return 0;
    }
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QStandardItemModel* m_model;
};

#endif // IMAGEDELEGATE_H
