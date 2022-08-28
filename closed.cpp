#include "closed.h"
#include "ui_closed.h"
#include "mainwindow.h"

closed::closed(QWidget *parent, QString name, QSqlDatabase *sdb, QSqlQuery* query) :
    QWidget(parent),
    ui(new Ui::closed)
{
    ui->setupUi(this);
    //создаем спиоск серий
    keywords = get_seria_names(sdb, query);
    //включаем автодополнение
    QCompleter* completer = new QCompleter(keywords, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->lineEdit_3->setCompleter(completer);
    ui->lineEdit->setValidator(new QIntValidator);
    //возвращаем цену
    query->prepare("select price, name from main_base_coins where id = :id");
    query->bindValue(":id", name.toInt());
    if (!sdb->isOpen())
        sdb->open();
    bool pp = query->exec();
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _get_seria_names_, провертье карманы!";
    }
    while (query->next()) {
        QString coin = query->value(0).toString();
        ui->lineEdit->setText(coin);
        QString coin1 = query->value(1).toString();
        ui->label_4->setText(coin1);
        }
    sdb1 = sdb;
    name1 = name;
    query1 = query;

//    connect(ui->pushButton, &QPushButton::clicked, this, SLOT(on_pushButton_clicked(QSqlDatabase *sdb, QSqlQuery* query, QString name)));
}

closed::~closed()
{
    delete ui;
}

QStringList closed::get_seria_names(QSqlDatabase *sdb, QSqlQuery* query)
{
    auto pp = query->exec("SELECT kind FROM described");
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _get_seria_names_, провертье карманы!";
    }
//  Формируем список серий
    QStringList words;
    while (query->next()) {
        QString coin = query->value(0).toString();
        words << coin;
    //             QMessageBox::information(this, "ddd", country);
        }
    return words;
}
int closed::choose_int_method(const QString &name)
{
    if (name == "Мешок")
        return 1;
    else if (name == "Авито")
        return 2;
    else if (name == "NewAuction")
        return 3;
    else if (name == "Самара")
        return 4;
    else if (name == "Личная встреча")
        return 5;
    else if (name == "vk.com")
        return 6;
    else if (name == "Youla")
        return 7;
    else if (name == "Ok.ru")
        return 8;
    else if (name == "Невские клады")
        return 9;
    else if (name == "5monetok")
        return 10;
    else if (name == "Anumis")
        return 11;
    else if (name == "New Molotok")
        return 12;
    else if (name == "Конрос")
        return 13;
    else if (name == "Магазин Альберт")
        return 14;
    else if (name == "Молоток")
        return 15;
    else if (name == "Перийные ряды")
        return 16;
    else if (name == "Сбербанк")
        return 17;
    else if (name == "Соберу")
        return 18;
    else if (name == "Троицкий стандарт")
        return 19;
    return 0;
}

void closed::on_pushButton_clicked()
{
    auto id = name1.toInt();
    //Вставляем новую серию
    query1->prepare("INSERT INTO described(kind) VALUES(:name) ON CONFLICT (kind) DO NOTHING");
    query1->bindValue(":name", ui->lineEdit_3->text().trimmed());
    if (!sdb1->isOpen())
        sdb1->open();
    bool pp = query1->exec();
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _seria_, провертье карманы!";
    }
    //Получаем номер метода продажи
    auto method = choose_int_method(ui->comboBox->currentText());
    query1->prepare("UPDATE main_base_coins SET amount = :amount, \
                        price = :price, client = :client, \
                        data_contact = :contact, seria = (select id from described where kind = :seria), \
                        method_sale = :method, date_sale = current_date, \
                        status = 4, profit = price - selfprice \
                    WHERE id = :uid;");
    query1->bindValue(":amount", ui->spinBox->text().toInt());
    query1->bindValue(":price", ui->lineEdit->text().toInt());
    query1->bindValue(":client", ui->lineEdit_2->text().trimmed());
    query1->bindValue(":contact", ui->lineEdit_4->text().trimmed());
    query1->bindValue(":seria", ui->lineEdit_3->text().trimmed());
    query1->bindValue(":method", method);
    query1->bindValue(":uid", id);
    pp = query1->exec();
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _add_sale_, провертье карманы!";
    }
    QMessageBox::information(this, "Ok", "Лот закрыт");
    emit add_name(name1);
    this->close();
//    qDebug() << "Вроде не удается сделать запрос _add_sale_, провертье карманы!";
}

