#ifndef CLOSED_H
#define CLOSED_H

#include <QWidget>
#include <QtSql>
#include <QCompleter>
#include <QMessageBox>
namespace Ui {
class closed;
}

class closed : public QWidget
{
    Q_OBJECT

public:
    explicit closed(QWidget *parent = nullptr, QString name = nullptr, QSqlDatabase* sdb = nullptr, QSqlQuery* query = nullptr);
    QStringList get_seria_names(QSqlDatabase *sdb, QSqlQuery* query);
    int choose_int_method(const QString &name);
    ~closed();

signals:
    void add_name(QString name);
private slots:
    void on_pushButton_clicked();

private:
    Ui::closed *ui;
    QStringList keywords;
    QString name1;
    QSqlDatabase *sdb1;
    QSqlQuery* query1;

};

#endif // CLOSED_H
