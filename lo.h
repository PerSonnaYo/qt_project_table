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
#include <QTableView>

namespace Ui {
class lo;
}

class StringListModel1 : public QStandardItemModel
{
public:
    StringListModel1(QObject *s) : QStandardItemModel(s)
    {}

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        Qt::ItemFlags flags = QStandardItemModel::flags(index);

        if (index.isValid())
            flags &= (~Qt::ItemIsDropEnabled);

        return flags;
    }
//private:
//    void mousePressEvent(QMouseEvent * event);
    //void mouseMoveEvent(QMouseEvent * event);
    //void keyPressEvent(QKeyEvent *event);
//    void dropEvent(QDropEvent * event);

//    bool moveRows(const QModelIndex &srcParent, int srcRow, int count,
//    const QModelIndex &dstParent, int dstChild)
//    {
//    beginMoveRows(QModelIndex(), srcRow, srcRow + count - 1, QModelIndex(), dstChild);
//    for(int i = 0; i<count; ++i) {
//    m_data.insert(dstChild + i, m_data[srcRow]);
//    int removeIndex = dstChild > srcRow ? srcRow : srcRow+1;
//    m_data.removeAt(removeIndex);
//    }
};

class AdvancedTableW : public QTableView
{
    Q_OBJECT
public:
    AdvancedTableW(QWidget * parent = 0, StringListModel1 *imagesModel= nullptr)
    {
        this->parent=parent;
        setAcceptDrops(true);
        m_imagesModel = imagesModel;
    }
    StringListModel1* m_imagesModel;
signals:
    void srtItem(const QModelIndex &currentIndex, const QModelIndex &targetIdx, QVector<int> &spec);
private:
    QWidget * parent;
//    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    //void mouseMoveEvent(QMouseEvent * event);
    //void keyPressEvent(QKeyEvent *event);
//    void dropEvent(QDropEvent * event);
//    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    // Метод события отпускания объекта с данными
    virtual void dropEvent(QDropEvent *event) override;
    QList<QStandardItem *> first;
    QLabel *photo_show;
//    StringListModel1* m_imagesModel;
};

class python_worker1 : public QObject
{
    Q_OBJECT
public:
    explicit python_worker1(QObject* parent = nullptr);
signals:
    void change_progress(int prog);
    void unblock_button(QString &message);

public slots:
    void send_data(StringListModel1 *m_imagesModel, int p, QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que);
};

class lo : public QWidget
{
    Q_OBJECT

public:
    explicit lo(QWidget *parent = nullptr, QQueue<QString> params = QQueue<QString>());
    // Метод события перетаскивания
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    // Метод события отпускания объекта с данными
    virtual void dropEvent(QDropEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    ~lo();
signals:
    void p_finished();
    void start_operation(StringListModel1 *m_imagesModel, int p, QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que);
private:
    Ui::lo *ui;
    QGridLayout *scrollLayout;
    StringListModel1* m_imagesModel;
    QLabel *photo_show;
    QPushButton *delete_line;
    AdvancedTableW *field_drag;
    QList<QStandardItem *> first;
    QList<QStandardItem *> second;
    QList<QStandardItem *> therd;
    QPushButton *send_d;
    QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que;
    int p;
    python_worker1* worker;
    QThread *thread;
    QProgressBar *bar;

private slots:
//    void on_pushButton_2_clicked();
    void onImagesListViewClicked(const QModelIndex &index);
    void delete_photo();
    void pre_send();
    void set_prog(int prog);
    void unblock_button(QString &message);
    void change_items(const QModelIndex &currentIndex, const QModelIndex &targetIdx, QVector<int> &spec);
};
