#include "lo.h"
#include "ui_lo.h"
#include "imagedelegate.h"
//#include <qDebug>

lo::lo(QWidget *parent, QQueue<QString> params) :
    QWidget(parent),
    ui(new Ui::lo)
{
    ui->setupUi(this);
    scrollLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    p = 1;
    while(!params.empty())
    {
        QString line = params.dequeue();
        QLabel *lab = new QLabel(this);
        QStringList box = line.split(" | ");
        lab->setText(box[0]);
        lab->setFixedWidth(35);
        box.clear();
        box = line.split(" [&] ");
        QLineEdit* self = new QLineEdit(this);
        self->setText(box[1]);
        self->setValidator(new QIntValidator);
        self->setFixedWidth(60);
        QString line2 = box[0].split(" | ")[1];
        QLineEdit* name = new QLineEdit(this);
        name->setText(line2);
        QQueue<QLineEdit*> all;
        all.enqueue(name);
        all.enqueue(self);
        QPair<QLabel*, QQueue<QLineEdit*> > pair;
        pair.first = lab;
        pair.second = all;
        name->setMinimumWidth(100);
//        QLineEdit* l = params.dequeue();
        scrollLayout->addWidget(lab, p, 0);
        scrollLayout->addWidget(name, p, 1);
        scrollLayout->addWidget(self, p, 2);
        que.enqueue(pair);//создаем буфер для дальнейшей отправки
        p++;
    }
    //Поле для фоток
    field_drag = new AdvancedTableW(this);
    field_drag->setObjectName("pole");
//    field_drag->setStyleSheet("QHeaderView::section { background-color:red }");
//    field_drag->setRowHeight();
//    field_drag->columnViewportPosition();
//    field_drag->setFixedWidth(540);
    scrollLayout->addWidget(field_drag, 1, 3, p + 1, 1);
    //Место для ведущей фотки
    photo_show = new QLabel(this);
    photo_show->setObjectName("photo");
//    field_drag->setFixedWidth(140);
    scrollLayout->addWidget(photo_show, p + 1, 1);
    //Кнопка удаления
    delete_line = new QPushButton(this);
    delete_line->setObjectName("photo1");
    delete_line->setText("Удалить");
//    field_drag->setFixedWidth(50);
    scrollLayout->addWidget(delete_line, p + 1, 0);
    //Кнопка отправки
    send_d = new QPushButton(this);
    send_d->setObjectName("photo2");
    send_d->setText("Отправить");
//    field_drag->setFixedWidth(60);
    scrollLayout->addWidget(send_d, p + 1, 2);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollAreaWidgetContents->showMaximized();
    ui->scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->scrollArea->setMinimumWidth(300);
    ui->scrollArea->setMinimumHeight(300);

    //  Активируем перенос фоток
    setAcceptDrops(true);
    m_imagesModel = new StringListModel1(field_drag);
    field_drag->setModel(m_imagesModel);
    field_drag->setItemDelegate(new ImageDelegate(m_imagesModel, field_drag));
    //Моды для переноса фоток
    field_drag->setDefaultDropAction(Qt::MoveAction);
    field_drag->setDragDropMode(field_drag->DragDrop);
    field_drag->setDragEnabled(true);
//    field_drag->set
    field_drag->setAcceptDrops(true);
//    field_drag->viewport()->setAcceptDrops(true);
    connect(field_drag, &AdvancedTableW::srtItem,
            this, &lo::change_items);
    m_imagesModel->setColumnCount(5);
    field_drag->setColumnWidth(0, 50);
//    field_drag->setColumnHidden(0, true);
    field_drag->setColumnWidth(1, 150);
    field_drag->setColumnWidth(2, 50);
    field_drag->setColumnWidth(3, 100);
    field_drag->setColumnWidth(4, 100);
    //первый столбец не редактируемый
    field_drag->setItemDelegateForColumn(0, new ImageDelegateColumn(m_imagesModel, field_drag));

//    field_drag->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    field_drag->setDragDropOverwriteMode(false);
    //Создаем три столбца в таблице первый название монеты два остальных фотки
//    m_imagesModel->appendColumn(first);
//    m_imagesModel->appendColumn(second);
//    m_imagesModel->appendColumn(therd);

    //  Блок кнопок
    connect(field_drag, &AdvancedTableW::clicked, this, &lo::onImagesListViewClicked);
    connect(delete_line, &QPushButton::clicked, this, &lo::delete_photo);
    connect(send_d, &QPushButton::clicked, this, &lo::pre_send);
    thread = new QThread();
    connect(this, SIGNAL(destroyed()), thread, SLOT(quit()));
    worker = new python_worker1();
    connect(this, &lo::start_operation,
            worker, &python_worker1::send_data);
    //Подключение прогрессбара
    connect(worker, &python_worker1::change_progress,
            this, &lo::set_prog);
    //Разблокировать кнопку в случае ошибки
    connect(worker, &python_worker1::unblock_button,
            this, &lo::unblock_button);
    worker->moveToThread(thread);
    thread->start();
//    thread->
}

python_worker1::python_worker1(QObject *parent) :
    QObject(parent)
    {}

lo::~lo()
{
    emit p_finished();
    delete ui;
}

void lo::dragEnterEvent(QDragEnterEvent *event)
{
    // Обязательно необходимо допустить событие переноса данных в область окна приложения
    event->accept();
    qDebug() << "de1e";
}

void lo::change_items(const QModelIndex &currentIndex, const QModelIndex &targetIdx, QVector<int> &spec)
{
    QMap<int, QVariant> sender = m_imagesModel->itemData(currentIndex);
    QMap<int, QVariant> reciver = m_imagesModel->itemData(targetIdx);
    m_imagesModel->setItemData(targetIdx, sender);
    int curr_row = spec[2];
    int curr_column = spec[3];
    int targ_row = spec[0];
    int targ_column = spec[1];
    QModelIndex firstIdx;
    QModelIndex secondIdx;
    QMap<int, QVariant> first;
    QMap<int, QVariant> second;

    if (curr_row > targ_row)//если сверху вниз
    {
        if (targ_column == 3)
        {
            secondIdx = m_imagesModel->index(targ_row, 4);
            second = m_imagesModel->itemData(secondIdx);
            m_imagesModel->setItemData(secondIdx, reciver);
        }
        else
            second = reciver;
        for(int i = targ_row + 1; i <= curr_row; i++)
        {
            for (int y = 3; y < 5; y++)
            {
                first.clear();
                firstIdx = m_imagesModel->index(i, y);
                first = m_imagesModel->itemData(firstIdx);
                m_imagesModel->setItemData(firstIdx, second);
                second.clear();
                second.swap(first);
                if (i == curr_row)
                {
                    if (curr_column == 3)
                        break;
                }
            }
        }
    }
    else if (curr_row < targ_row)//если с низу вверх
    {
        if (targ_column == 4)
        {
            secondIdx = m_imagesModel->index(targ_row, 3);
            second = m_imagesModel->itemData(secondIdx);
            m_imagesModel->setItemData(secondIdx, reciver);
        }
        else
            second = reciver;
        for(int i = targ_row - 1; i >= curr_row; i--)
        {
            for (int y = 4; y > 2; y--)
            {
                first.clear();
                //Свапаеем все ячейки
                firstIdx = m_imagesModel->index(i, y);
                first = m_imagesModel->itemData(firstIdx);
                m_imagesModel->setItemData(firstIdx, second);
                second.clear();
                second.swap(first);
                //Для то чтобы не заменять всю строку
                if (i == curr_row)
                {
                    if (curr_column == 4)
                        break;
                }
            }
        }
    }
    else if (curr_row == targ_row)
        m_imagesModel->setItemData(currentIndex, reciver);//если в одной строке
    qDebug() << m_imagesModel->rowCount() << spec;
}
void AdvancedTableW::dropEvent(QDropEvent *event)
{
//    m_imagesModel->setColumnCount(4);
    QModelIndex targetIdx = indexAt(event->position().toPoint());
    QVector<int> spec;
    spec.push_back(targetIdx.row());
    spec.push_back(targetIdx.column());
    QModelIndex currentIndex = selectionModel()->currentIndex();
//    QMap<int, QVariant> sender = m_imagesModel->itemData(currentIndex);
//    m_imagesModel->setItemData(targetIdx, sender);
    spec.push_back(currentIndex.row());
    spec.push_back(currentIndex.column());
    if (currentIndex.column() < 3)
    {
        QMessageBox::warning(this, "Error", "Нельзя переносить текст");
        return;
    }
    if (targetIdx.column() < 3)
    {
        QMessageBox::warning(this, "Error", "Ошибся дорогой");
        return;
    }
    QStandardItem *kio = m_imagesModel->itemFromIndex(currentIndex);
    emit srtItem(currentIndex, targetIdx, spec);
    qDebug() << "1111A";
}

void lo::dropEvent(QDropEvent *event)
{
    // Когда отпускаем файл в область приложения,
    // то забираем путь к файлу из MIME данных
//    first.clear();
//    second.clear();
//    event-
    if (event->mimeData()->urls().size() % 2 != 0)
    {
        QMessageBox::warning(this, "Error", "Только четное число фоток");
        return;
    }
    int count = m_imagesModel->rowCount();
    for (int i = 0; event->mimeData()->urls().size() > i; i++)
    {

        first.clear();
        QString filePath = event->mimeData()->urls()[i].toLocalFile();
        QPixmap pixmap(filePath);
        if (que.isEmpty())
        {
            QMessageBox::warning(this, "Error", "Список монет пуст");
            return;
        }
        QPair<QLabel*, QQueue<QLineEdit*> > pair = que.dequeue();
        QLineEdit *ji2 = pair.second.dequeue();
        QLineEdit *ji3 = pair.second.dequeue();
        QString name = ji2->text();
        QString self = ji3->text();
        QLabel *ji1 = pair.first;
        auto id = ji1->text();
        QStandardItem *got = new QStandardItem(id);
        got->setForeground(QBrush(QColor(180, 60, 220)));
        got->setFont(QFont("Times", 8, QFont::Black));
        first.push_back(got);
        QStandardItem *got1 = new QStandardItem(name);
        got1->setForeground(QBrush(QColor(180, 60, 220)));
        got1->setFont(QFont("Times", 8, QFont::Black));
        first.push_back(got1);
        QStandardItem *got2 = new QStandardItem(self);
        got2->setForeground(QBrush(QColor(180, 60, 220)));
        got2->setFont(QFont("Times", 8, QFont::Black));
        first.push_back(got2);
        photo_show->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
        first.push_back(new QStandardItem(QIcon(pixmap), filePath));
        QString filePath1 = event->mimeData()->urls()[++i].toLocalFile();
        QPixmap pixmap1(filePath1);
        first.push_back(new QStandardItem(QIcon(pixmap1), filePath1));
        m_imagesModel->appendRow(first);
        int count = m_imagesModel->rowCount();
        field_drag->setRowHeight(count - 1, 90);
//        field_drag->setColumnWidth(0, 300);
        field_drag->resizeRowsToContents() ;
//        field_drag->setColumnWidth(1, this->width()/3);
//        field_drag->setColumnWidth(2, this->width()/3);
        delete ji1;
        delete ji2;
        delete ji3;
    }
    qDebug() << "1111S";
}

void lo::onImagesListViewClicked(const QModelIndex &index)
{
    // Когда кликаем по элементу в списке, то забираем путь к файлу
    QPixmap pixmap(m_imagesModel->data(index).toString());
    qDebug() << m_imagesModel->data(index).toString() << "77777____";
    qDebug() << m_imagesModel->item(0)->text() << "8888____";
    qDebug() << m_imagesModel->invisibleRootItem()->rowCount() << "99999____";
    // И устанавливаем файл в область основного просмотра
    photo_show->setPixmap(pixmap.scaled(120, 100, Qt::KeepAspectRatio));
//    ui->label_2->resize(pixmap.size());
}

void lo::delete_photo()
{
    QModelIndex currentIndex = field_drag->selectionModel()->currentIndex();
    m_imagesModel->removeRow(currentIndex.row());
}

void lo::pre_send()
{
    emit start_operation(m_imagesModel, p, que);
    //Создаем прогрессбар
    bar = new QProgressBar(this);
    bar->setOrientation(Qt::Horizontal);
    bar->setRange(0, 100); // Let's say it goes from 0 to 100
    bar->setValue(0);
    scrollLayout->addWidget(bar, 0, 0, 1, 3);
    //одноразовая кнопка
    send_d->setEnabled(false);
}

void lo::set_prog(int prog)
{
    if (prog == 101)
    {
        QMessageBox::information(this, "Error", "Capcha");
        prog = 100;
    }
    bar->setValue(prog);
}

void lo::unblock_button(QString &message)
{
    QMessageBox::warning(this, "Error", message);
    send_d->setEnabled(true);
}

void python_worker1::send_data(StringListModel1 *m_imagesModel, int p, QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que)
{
//    p -= 1;
//    p *= 2;
//    if (p == 0 || m_imagesModel->invisibleRootItem()->rowCount() != p)
//    {
//        QString message1 = "Количество фото не соответсвует количеству названий";
//        emit unblock_button(message1);
//        return;
//    }
    QStringList params;
//    params << "11.py";
    QString message;
    auto row_count = m_imagesModel->rowCount();
//    auto column_count = m_imagesModel->columnCount();
    for (int i = 0; i < row_count; i++)
    {
        //Собираем элементы для прасинга
        auto id = m_imagesModel->item(i, 0)->text();
        auto name = m_imagesModel->item(i, 1)->text();
        auto self = m_imagesModel->item(i, 2)->text();
        auto foto1 = m_imagesModel->item(i, 3)->text();
        auto foto2 = m_imagesModel->item(i, 4)->text();
//        qDebug() << id << name << self << foto1 << foto2;
        if  (name.isEmpty())
        {
            message = "Введите название монеты " + id;
            emit unblock_button(message);
            return;
        }
        if  (self.isEmpty() || !self.toInt())
        {
            message = "Введите себестоимость " + id;
            emit unblock_button(message);
            return;
        }
        if (!name.contains(". "))
        {
            message = "Нет блоков " + id;
            emit unblock_button(message);
            return;
        }
        QStringList box = name.split(". ");
        if (box.size() != 8)
        {
            message = "Неверное количество аргументов " + id;
            emit unblock_button(message);
            return;
        }
        box.clear();
        if (!name.contains("Цена "))
        {
            message = "Нет цены1 " + id;
            emit unblock_button(message);
            return;
        }
        if (name.contains("  "))
        {
            message = "Указано много пробелов" + id;
            emit unblock_button(message);
            return;
        }
        box = name.split("Цена ");
        if (!name.contains(" руб"))
        {
            message = "Нет цены2 " + id;
            emit unblock_button(message);
            return;
        }
        QString price = box[1].split(" руб")[0];
        if (price.toInt() == 0)
        {
            message = "Нет цены3 " + id;
            emit unblock_button(message);
            return;
        }
        auto general_name = name.trimmed();
        auto name1 = box[0].trimmed();
        //Аргумент для передачи
        QString to_send = id;//Формируем данные для отправки в питон скрипт
        QString wall = "[$]";
        to_send.push_back(wall);
        to_send.push_back(name1);
        to_send.push_back(wall);
        to_send.push_back(price);
        to_send.push_back(wall);
        to_send.push_back(self);
        to_send.push_back(wall);
        to_send.push_back(general_name);
        to_send.push_back(wall);
        to_send.push_back(foto1);
        to_send.push_back(wall);
        to_send.push_back(foto2);
        params << to_send;
        qDebug()  << to_send;
    }
    if (params.isEmpty())
    {
            message = "Нет работы";
            emit unblock_button(message);
            return;
    }


    QString  command("vk_api.exe");
//    QString  command("11.exe");
    QProcess *process = new QProcess();
    process->start(command, params);
    auto y = 0;
    while (y != 99)
    {
        QDirIterator it(QCoreApplication::applicationDirPath(), QStringList() << "*.conf", QDir::Files);
        QString filename;
        while (it.hasNext())
        {
            filename = it.next();
//            qDebug() << filename;
        }
        QStringList op = filename.split('/');
        QString filename1 = op.takeLast().split('.')[0];
        if (filename1.contains("capcha"))
            y = 101;
        else
            y = filename1.toInt();
        emit change_progress(y);
    }
    emit change_progress(100);
//        data.append (process->readAll());
//    qDebug() << data;
//    process->waitForFinished(5000); //give the program at most 5 seconds (for now)
//    QByteArray result=process->readAll();
//    qDebug() << result;
//    emit change_progress(100);
//    process->waitForFinished();
//    QString p_stdout = process->readAllStandardOutput();
//    QString p_stdout1 = process->readAllStandardError();
//    qDebug()  << p_stdout << p_stdout1 << "99999____";

}


//Для разблокировки кнопки добавления
void lo::closeEvent(QCloseEvent *event)
{
    //Здесь код
    emit p_finished();
    event->accept();
}





