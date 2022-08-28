#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::create_tables()
{
    //    Создаем таблицу
    QString str = "CREATE TABLE IF NOT EXISTS uniq_base_coins ("
                "id serial, "
                "name VARCHAR(400) UNIQUE"
                ");";
    bool b = query.exec(str);
    if (!b) {
            qDebug() << "Вроде не удается создать таблицу uniq, провертье карманы!";
    }
    str = "CREATE TABLE IF NOT EXISTS described ("
                "id serial PRIMARY KEY, "
                "status text, "
                "owner_id integer, "
                "kind VARCHAR(250) UNIQUE, "
                "albums integer, "
                "about_albums text, "
                "method_sale VARCHAR(200)"
                ");";
    b = query.exec(str);
    if (!b) {
            qDebug() << "Вроде не удается создать таблицу described, провертье карманы!";
    }
    str = "CREATE TABLE IF NOT EXISTS main_base_coins ("
                "id serial, "
                "name VARCHAR(400), "
                "price integer, "
                "selfprice integer, "
                "dates timestamp DEFAULT current_timestamp, "
                "owner varchar(50) DEFAULT \"Рома\", "
                "photo_id1 integer, "
                "photo_id2 integer, "
                "comment_id1 integer, "
                "comment_id2 integer, "
                "photo_url1 text, "
                "photo_url2 text, "
                "status smallint DEFAULT 1, "
                "amount smallint, "
                "seria integer, "
                "profit integer, "
                "client varchar(300), "
                "data_contact text, "
                "method_sale smallint, "
                "date_sale date, "
                "FOREIGN KEY (status)  REFERENCES described (id), "
                "FOREIGN KEY (seria)  REFERENCES described (id), "
                "FOREIGN KEY (method_sale)  REFERENCES described (id)"
                ");";
    b = query.exec(str);
    if (!b) {
            qDebug() << "Вроде не удается создать таблицу main_base_coins, провертье карманы!";
    }
    str = "CREATE TABLE IF NOT EXISTS dop_credits ("
                "id serial, "
                "\"Расходы\" integer, "
                "\"Доходы\" integer, "
                "dates date DEFAULT CURRENT_DATE"
                ");";
    b = query.exec(str);
    if (!b) {
            qDebug() << "Вроде не удается создать таблицу dop_credits, провертье карманы!";
    }
}

void MainWindow::get_unique_names()
{
    if (!sdb.isOpen())
        sdb.open();
    auto pp = query.exec("SELECT name FROM uniq_base_coins");
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _get_unique_names_, провертье карманы!";
    }
//  Формируем список монет
    while (query.next()) {
        QString coin = query.value(0).toString();
        keywords << coin;
    //             QMessageBox::information(this, "ddd", country);
        }
}

void MainWindow::on_pushButton1_3_clicked()
{
    QLayoutItem *item;
    scrollLayout->removeItem(second);
    while((item = scrollLayout->takeAt(0)))
    {
        auto name = item->widget()->objectName();
//        qDebug() << name;
        if (name == "form" || name == "take" || name == "photo")
        {
            //Просто удаляем кнопки из слоя и переставляем на верх
            scrollLayout->removeItem(item);
            continue;
        }
        if(item->widget())
            delete item->widget();
        delete item;
    }
    while((item = second->takeAt(0)))
    {
        //Просто удаляем кнопки из слоя и переставляем на верх для второго слоя
        scrollLayout->removeItem(item);
    }
    second->addWidget(form);
    second->addWidget(take);
    scrollLayout->addLayout(second, 0, 0);
//    s_photo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
//    s_photo->setMinimumWidth(50);
    scrollLayout->addWidget(s_photo, 0, 2);
    not_load_all.clear();
    if (!sdb.isOpen())
        sdb.open();
    auto pp = query.exec("SELECT id, name, price, selfprice FROM main_base_coins where status = 1 order by dates");
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _СФОРМИРОВАТЬ_, провертье карманы!";
    }
//  Заполняем без фоточные монеты
    volume = 0;
    QListWidget *list = new QListWidget();
    list->setStyleSheet("QCheckBox{font-size: 16px;font-family: Arial;color: rgb(255, 255, 255);background-color: rgb(38,56,76);}");
    list->setAutoScroll(true);

    while (query.next()) {
        volume++;
        auto id = query.value(0).toString();
        QString name = query.value(1).toString();
        auto price = query.value(2).toString();
        QString selfprice = query.value(3).toString();
        QString all = id+" | "+name+" Цена "+price+" руб. По всем вопросам в Л.С. [&] "+selfprice;
        QCheckBox* line = new QCheckBox(this);//создаем линию для текста
        line->setText(all);
        not_load_all.push_back(line);
        QListWidgetItem *listWidgetItem = new QListWidgetItem();
        listWidgetItem->setSizeHint(line->sizeHint());
        list->addItem(listWidgetItem);
        list->setItemWidget(listWidgetItem, line);
        list->scrollToItem(listWidgetItem);
        }
    scrollLayout->addWidget(list, 1, 0, 3, 1);
    ui->scrollArea->setWidgetResizable(true);

}

void MainWindow::on_pushButton_sale()
{
    QLayoutItem *item;
    while((item = scrollLayout_sale->takeAt(0)))
    {
        auto name = item->widget()->objectName();
//        qDebug() << name;
        if (name == "form_sale")
        {
            //Просто удаляем кнопки из слоя и переставляем на верх
            scrollLayout_sale->removeItem(item);
            continue;
        }
        if(item->widget())
            delete item->widget();
        delete item;
    }
    scrollLayout_sale->addWidget(form_sale, 0);
//    not_load_all.clear();
    //Подчищаем список списанных монет
    mass_closed_buttons.clear();
    if (!sdb.isOpen())
        sdb.open();
    auto pp = query.exec("SELECT id, name, price, selfprice FROM main_base_coins where status = 3 order by dates");
    if (!pp) {
        qDebug() << "Вроде не удается сделать запрос _СФОРМИРОВАТЬ_, провертье карманы!";
    }
//  Заполняем без фоточные монеты
    QListWidget *list = new QListWidget();
    list->setStyleSheet("QPushButton{font-size: 16px;font-family: Arial;color: rgb(255, 255, 255);background-color: rgb(38,56,76);}");
    list->setAutoScroll(true);

    while (query.next()) {
        auto id = query.value(0).toString();
        QString name = query.value(1).toString();
        auto price = query.value(2).toString();
        QString selfprice = query.value(3).toString();
        QString all = name+" Цена: "+price+" руб.Себестоиомсть: "+selfprice;
        QPushButton* line = new QPushButton(this);//создаем линию для текста
        line->setText(all);
        line->setObjectName(id);
        connect(line, &QPushButton::clicked,[=](){
            on_send_sale(line->objectName());
//            emit iop();
});
        QListWidgetItem *listWidgetItem = new QListWidgetItem();
        listWidgetItem->setSizeHint(line->sizeHint());
        list->addItem(listWidgetItem);
        list->setItemWidget(listWidgetItem, line);
        list->scrollToItem(listWidgetItem);
        }
    scrollLayout_sale->addWidget(list, 1);
    ui->scrollArea->setWidgetResizable(true);
}


