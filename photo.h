#ifndef PHOTO_H
#define PHOTO_H

#include <QWidget>
#include <QCompleter>
#include <QStandardItemModel>
#include <QPalette>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QtSql>
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QThread>
#include <QProgressBar>
#include <QGridLayout>

namespace Ui {
class photo;
}

class StringListModel : public QStandardItemModel
{
public:
    StringListModel(QObject *s) : QStandardItemModel(s)
    {}

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        Qt::ItemFlags flags = QStandardItemModel::flags(index);

        if (index.isValid())
            flags &= (~Qt::ItemIsDropEnabled);

        return flags;
    }
};

class python_worker : public QObject
{
    Q_OBJECT
public:
    explicit python_worker(QObject* parent = nullptr);
signals:
    void change_progress(int prog);
    void unblock_button(QString &message);

public slots:
    void send_data(StringListModel *m_imagesModel, int p, QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que);
};

class photo : public QWidget
{
    Q_OBJECT

public:
    explicit photo(QWidget *parent = nullptr, QQueue<QString> params = QQueue<QString>());
    // Метод события перетаскивания
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    // Метод события отпускания объекта с данными
    virtual void dropEvent(QDropEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    ~photo();
signals:
    void p_finished();
    void start_operation(StringListModel *m_imagesModel, int p, QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que);
private:
    Ui::photo *ui;
    QGridLayout *scrollLayout;
    StringListModel* m_imagesModel;
    QLabel *photo_show;
    QPushButton *delete_line;
    QListView *field_drag;
    QPushButton *send_d;
    QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que;
    int p;
    python_worker* worker;
    QThread *thread;
    QProgressBar *bar;
private slots:
//    void on_pushButton_2_clicked();
    void onImagesListViewClicked(const QModelIndex &index);
    void delete_photo();
    void pre_send();
    void set_prog(int prog);
    void unblock_button(QString &message);
};

#endif // PHOTO_H
