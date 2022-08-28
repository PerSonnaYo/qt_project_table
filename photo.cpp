#include "photo.h"
#include "ui_photo.h"
#include "imagedelegate.h"

photo::photo(QWidget *parent, QQueue<QString> params) :
    QWidget(parent),
    ui(new Ui::photo)
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
        lab->setFixedWidth(30);
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
    field_drag = new QListView(this);
    field_drag->setFixedWidth(140);
    scrollLayout->addWidget(field_drag, 1, 3, p + 1, 1);
    //Место для ведущей фотки
    photo_show = new QLabel(this);
//    field_drag->setFixedWidth(140);
    scrollLayout->addWidget(photo_show, p + 1, 1);
    //Кнопка удаления
    delete_line = new QPushButton(this);
    delete_line->setText("Удалить");
//    field_drag->setFixedWidth(50);
    scrollLayout->addWidget(delete_line, p + 1, 0);
    //Кнопка отправки
    send_d = new QPushButton(this);
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
    m_imagesModel = new StringListModel(field_drag);
    field_drag->setModel(m_imagesModel);
    field_drag->setItemDelegate(new ImageDelegate(m_imagesModel, field_drag));
    //Моды для переноса фоток
    field_drag->setDefaultDropAction(Qt::MoveAction);
    field_drag->setDragDropMode(field_drag->DragDrop);
    field_drag->setDragEnabled(true);

    //  Блок кнопок
    connect(field_drag, &QListView::clicked, this, &photo::onImagesListViewClicked);
    connect(delete_line, &QPushButton::clicked, this, &photo::delete_photo);
    connect(send_d, &QPushButton::clicked, this, &photo::pre_send);
    thread = new QThread();
    connect(this, SIGNAL(destroyed()), thread, SLOT(quit()));
    worker = new python_worker();
    connect(this, &photo::start_operation,
            worker, &python_worker::send_data);
    //Подключение прогрессбара
    connect(worker, &python_worker::change_progress,
            this, &photo::set_prog);
    //Разблокировать кнопку в случае ошибки
    connect(worker, &python_worker::unblock_button,
            this, &photo::unblock_button);
    worker->moveToThread(thread);
    thread->start();
//    thread->
}

python_worker::python_worker(QObject *parent) :
    QObject(parent)
    {}

photo::~photo()
{
    emit p_finished();
    delete ui;
}

void photo::dragEnterEvent(QDragEnterEvent *event)
{
    // Обязательно необходимо допустить событие переноса данных в область окна приложения
    event->accept();
}

void photo::dropEvent(QDropEvent *event)
{
    // Когда отпускаем файл в область приложения,
    // то забираем путь к файлу из MIME данных
    for (int i = 0; event->mimeData()->urls().size() > i; i++)
    {
        QString filePath = event->mimeData()->urls()[i].toLocalFile();
        QPixmap pixmap(filePath);
        photo_show->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
        m_imagesModel->appendRow(new QStandardItem(QIcon(pixmap), filePath));
    }
}

void photo::onImagesListViewClicked(const QModelIndex &index)
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

void photo::delete_photo()
{
    QModelIndex currentIndex = field_drag->selectionModel()->currentIndex();
    m_imagesModel->removeRow(currentIndex.row());
}

void photo::pre_send()
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

void photo::set_prog(int prog)
{
    bar->setValue(prog);
}

void photo::unblock_button(QString &message)
{
    QMessageBox::warning(this, "Error", message);
    send_d->setEnabled(true);
}

void python_worker::send_data(StringListModel *m_imagesModel, int p, QQueue<QPair<QLabel*, QQueue<QLineEdit*> > > que)
{
    p -= 1;
    p *= 2;
    if (p == 0 || m_imagesModel->invisibleRootItem()->rowCount() != p)
    {
        QString message1 = "Количество фото не соответсвует количеству названий";
        emit unblock_button(message1);
        return;
    }
    QStringList params;

    for(int i = 0; i < p; i++)
    {
        auto foto1 = m_imagesModel->item(i)->text();
        auto foto2 = m_imagesModel->item(++i)->text();
        QPair<QLabel*, QQueue<QLineEdit*> > pair = que.dequeue();
        QString name = pair.second.dequeue()->text();
        QString self = pair.second.dequeue()->text();
        auto id = pair.first->text();
        QString message;
        if  (name.isEmpty())
        {
            message = "Введите название монеты " + id;
            emit unblock_button(message);
            return;
        }
        if  (self.isEmpty())
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


    QString  command("vk_api.exe");
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
            qDebug() << filename;
        }
        QStringList op = filename.split('/');
        y = op.takeLast().split('.')[0].toInt();
        emit change_progress(y);
    }
    emit change_progress(100);
//    process->waitForFinished();
//    QString p_stdout = process->readAllStandardOutput();
//    QString p_stdout1 = process->readAllStandardError();
//    qDebug()  << p_stdout << p_stdout1 << "99999____";

}


//Для разблокировки кнопки добавления
void photo::closeEvent(QCloseEvent *event)
{
    //Здесь код
    emit p_finished();
    event->accept();
}

