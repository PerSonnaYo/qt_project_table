
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "imagedelegate.h"
#include <QProcess>
#include <QFileInfo>
#include "photo.h"
#include "ui_photo.h"
#include "closed.h"
#include "ui_closed.h"
#include "lo.h"
#include "ui_lo.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//    QFile::copy("vk_api.py", QCoreApplication::applicationFilePath());

    // Подключение к базе данных
    ui->setupUi(this);
    sdb = QSqlDatabase::addDatabase("QPSQL");
    sdb.setHostName("xx.xx.x.45");
    sdb.setDatabaseName("xxx");
    sdb.setUserName("xxxx");
    sdb.setPassword("xxxx");
    sdb.setPort(5432);
    if (!sdb.open()) {
          qDebug() << sdb.lastError().text();
          exit(1);
    }
    QSqlQuery query1;
    query.swap(query1);
    this->create_tables();
    this->get_unique_names();

//  Активируем перенос фоток
//    setAcceptDrops(true);
//    m_imagesModel = new StringListModel(ui->listView);
//    ui->listView->setModel(m_imagesModel);
//    ui->listView->setItemDelegate(new ImageDelegate(m_imagesModel, ui->listView));

//  Делаем автопродление
    QCompleter* completer = new QCompleter(keywords, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->lineEdit->setCompleter(completer);
    //Делаем вторую страницей с компановкой
    checked = true;
    form = new QPushButton();
    form->setObjectName("form");
    form->setText("Сформировать");
    form->setFixedWidth(250);
    take = new QPushButton();
    take->setObjectName("take");
    take->setText("Выделить все");
    s_photo = new QPushButton();
    s_photo->setObjectName("photo");
    s_photo->setText("Добавить фото");
    scrollLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    scrollLayout->addWidget(form, 0, 0);
    second = new QHBoxLayout();
    second->setObjectName("lay1");
    ui->scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->scrollArea->setMinimumWidth(500);
    ui->scrollArea->setMinimumHeight(200);
    //Третья страница
    form_sale = new QPushButton();
    form_sale->setObjectName("form_sale");
    form_sale->setText("Сформировать продажи");
    scrollLayout_sale = new QVBoxLayout(ui->tab_2);
    scrollLayout_sale->addWidget(form_sale);
//    scrollLayout->addWidget(take, 0, 1);
//    scrollLayout->addWidget(s_photo, 0, 2);
    //Для страницы с доп расходами
    ui->lineEdit_4->setValidator(new QIntValidator);
    ui->lineEdit_5->setValidator(new QIntValidator);

//  Блок кнопок
    connect(form, &QPushButton::clicked, this, &MainWindow::on_pushButton1_3_clicked);
    connect(take, &QPushButton::clicked, this, &MainWindow::on_pushButton1_5_clicked);
    connect(s_photo, &QPushButton::clicked, this, &MainWindow::on_pushButton1_4_clicked);
    connect(form_sale, &QPushButton::clicked, this, &MainWindow::on_pushButton_sale);

    ui->dateEdit->setCalendarWidget(ui->calendarWidget);
//    ui->dateEdit_2->
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
//    Добавляем монету в базу
    auto p1 = ui->lineEdit_2->text().toInt();
    auto p2 = ui->lineEdit_3->text().toInt();
    if (p1 == 0 || p2 == 0 || ui->lineEdit->text().isEmpty())
    {
        QMessageBox::information(this, "Внимание", "Введите число");
        return;
    }
//TODO: поменяь базу \\ засунуть в вайл для повторного открытия
    if (!sdb.isOpen())
        sdb.open();
    query.prepare("INSERT INTO main_base_coins(name \
                     , price \
                     , selfprice, status, owner) VALUES(:name \
                     , :price \
                     , :selfprice, :status, :owner); ");
    query.bindValue(":name", ui->lineEdit->text());
    query.bindValue(":price", p1);
    query.bindValue(":selfprice", p2);
    query.bindValue(":status", 1);
    query.bindValue(":owner", ui->comboBox->currentText());
    bool b = query.exec();
    if (!b) {
        qDebug() << "Кажется данные не вставляются, проверьте дверь, может она закрыта?";

    }
    QMessageBox::information(this, "Done", ui->lineEdit->text());
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}

void MainWindow::on_pushButton1_4_clicked()
{
    s_photo->setEnabled(false);
    QQueue<QString> not_load_check;
    for(int i = 0; i < not_load_all.size(); i++)
    {
        if (not_load_all[i]->isChecked())
            not_load_check.enqueue(not_load_all[i]->text());
    }
    lo* photo_window = new lo(nullptr, not_load_check);
//    photo* photo_window = new photo(nullptr, not_load_check);
    photo_window->show();
    connect(photo_window, SIGNAL(p_finished()), this, SLOT(readyToStart()));
}

void MainWindow::readyToStart()
{
    s_photo->setEnabled(true);
}


void MainWindow::on_pushButton1_5_clicked()
{
    if (checked)
        {
            for(int i = 0; i < not_load_all.size(); i++)
                not_load_all[i]->setChecked(true);
            checked = false;
        }
    else
        {
            for(int i = 0; i < not_load_all.size(); i++)
                not_load_all[i]->setChecked(false);
            checked = true;
        }
}

void MainWindow::on_send_sale(QString name)
{
//    mass_closed_buttons.insert(name);
    if (mass_closed_buttons.contains(name))
    {
        QMessageBox::warning(this, "Not", "Уже добавлен");
        return;
    }
    closed* closed_window = new closed(nullptr, name, &sdb, &query);
    //Добавляем в список выполненных чтобы не задублировать списание
    connect(closed_window, &closed::add_name,
            this, &MainWindow::add_in_write_off);
    closed_window->show();
//    connect(closed_window, SIGNAL(p_finished()), this, SLOT(readyAfterClosed()));
}

void MainWindow::add_in_write_off(QString name)
{
    mass_closed_buttons.insert(name);
}

void MainWindow::on_pushButton_6_clicked()
{
    if (!sdb.isOpen())
        sdb.open();
    query.prepare("INSERT INTO dop_credits(\"Расходы\") VALUES(:name);");
    query.bindValue(":name", ui->lineEdit_4->text().toInt());
    bool b = query.exec();
    if (!b) {
        qDebug() << "Кажется данные не вставляются _credits_, проверьте дверь, может она закрыта?";

    }
    QMessageBox::information(this, "Done", "Расходы учтены");
    ui->lineEdit_4->clear();
}


void MainWindow::on_pushButton_7_clicked()
{
    if (!sdb.isOpen())
        sdb.open();
    query.prepare("INSERT INTO dop_credits(\"Доходы\") VALUES(:name);");
    query.bindValue(":name", ui->lineEdit_5->text().toInt());
    bool b = query.exec();
    if (!b) {
        qDebug() << "Кажется данные не вставляются _credits_, проверьте дверь, может она закрыта?";

    }
    QMessageBox::information(this, "Done", "Доходы учтены");
    ui->lineEdit_5->clear();
}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    qDebug() << date;
    ui->dateEdit->setDate(date);
}


void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
    ui->dateEdit->setEnabled(false);
}

