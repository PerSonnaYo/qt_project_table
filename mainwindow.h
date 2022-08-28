#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCompleter>
#include <QWidget>
#include <QStandardItemModel>
#include <QPalette>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QtSql>
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>
#include <QCheckBox>
#include <QGridLayout>
#include <QListWidget>
//#include <QHorizontalLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Создаем нужную нужные нам таблицы в базе
    void create_tables();
    //Получаем список монет для автообнавления
    void get_unique_names();
//    void add_in_write_off(QString name);
signals:
    void iop();
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton1_4_clicked();

    void on_pushButton1_3_clicked();
    //Активация кнопки отправки
    void readyToStart();
    void on_pushButton1_5_clicked();
    void on_pushButton_sale();
    void on_send_sale(QString name);
    void add_in_write_off(QString name);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_calendarWidget_clicked(const QDate &date);

    void on_dateEdit_dateChanged(const QDate &date);

private:
    Ui::MainWindow *ui;
    QSqlDatabase sdb;
    QSqlQuery query;
    QStringList keywords;
    QHash<int, QStringList> able_to_send;
    int volume;
    QVector<QCheckBox *> not_load_all;
    bool checked;
    QGridLayout *scrollLayout;
    QPushButton *form;
    QPushButton *take;
    QPushButton *s_photo;
    QHBoxLayout *second;
    QVBoxLayout *scrollLayout_sale;
    QPushButton *form_sale;
    QSet<QString> mass_closed_buttons;
};
#endif // MAINWINDOW_H


