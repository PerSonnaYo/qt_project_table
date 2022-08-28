#include "imagedelegate.h"
#include <QTextDocument>

ImageDelegate::ImageDelegate(QStandardItemModel *model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{

}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Вместо отрисовки иконки и текста будем отрисовывать только одно изображение
    // с небольшими отступами в 5 пикселей
    QRect optionRect = option.rect;
    if (m_model->data(index).toString().contains("C:"))
    {
        QPixmap pix(m_model->data(index).toString());
//        qDebug() << (m_model->data(index).toString());
//        QRect optionRect = option.rect;
        painter->drawPixmap(optionRect.x() + 5,
                            optionRect.y() + 5,
                            optionRect.width() - 10,
                            optionRect.height() - 10 ,
                            pix);
    }
    else
    {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);

        painter->save();

        QTextDocument doc;
        QFont font = doc.defaultFont();
        font.setPixelSize(8);
        doc.setDefaultFont(font);
        QTextOption opt(options.displayAlignment);
        if(index.column() == 0)
        {
            opt.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        }
        else
            opt.setAlignment(Qt::AlignCenter);
        opt.setWrapMode(QTextOption::WordWrap);
        options.text = m_model->data(index).toString();
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);
        doc.setDefaultTextOption(opt);
        painter->translate(options.rect.left(), options.rect.top());
        QRect clip(0, 0, options.rect.width(), options.rect.height());
        doc.setTextWidth(options.rect.width());
        doc.drawContents(painter, clip);
        painter->restore();
    }
//        painter->drawText(optionRect.x() + 5,
//                        optionRect.y() + 15,
//                      m_model->data(index).toString());
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        // Корректируем размеры области отображения объекта в списке
        QSize result = QStyledItemDelegate::sizeHint(option, index);
        result.setHeight(90);
        result.setWidth(100);
        return QSize(100, 90);
}

ImageDelegateColumn::ImageDelegateColumn(QStandardItemModel *model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{

}

void ImageDelegateColumn::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Вместо отрисовки иконки и текста будем отрисовывать только одно изображение
    // с небольшими отступами в 5 пикселей
    QRect optionRect = option.rect;
    if (m_model->data(index).toString().contains("C:"))
    {
        QPixmap pix(m_model->data(index).toString());
//        qDebug() << (m_model->data(index).toString());
//        QRect optionRect = option.rect;
        painter->drawPixmap(optionRect.x() + 5,
                            optionRect.y() + 5,
                            optionRect.width() - 10,
                            optionRect.height() - 10 ,
                            pix);
    }
    else
    {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);

        painter->save();

        QTextDocument doc;
        QFont font = doc.defaultFont();
        font.setPixelSize(8);
        doc.setDefaultFont(font);
        QTextOption opt(options.displayAlignment);
        if(index.column() == 0)
        {
            opt.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        }
        else
            opt.setAlignment(Qt::AlignCenter);
        opt.setWrapMode(QTextOption::WordWrap);
        options.text = m_model->data(index).toString();
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);
        doc.setDefaultTextOption(opt);
        painter->translate(options.rect.left(), options.rect.top());
        QRect clip(0, 0, options.rect.width(), options.rect.height());
        doc.setTextWidth(options.rect.width());
        doc.drawContents(painter, clip);
        painter->restore();
    }
//        painter->drawText(optionRect.x() + 5,
//                        optionRect.y() + 15,
//                      m_model->data(index).toString());
}

QSize ImageDelegateColumn::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        // Корректируем размеры области отображения объекта в списке
        QSize result = QStyledItemDelegate::sizeHint(option, index);
        result.setHeight(90);
        result.setWidth(100);
        return QSize(100, 90);
}
