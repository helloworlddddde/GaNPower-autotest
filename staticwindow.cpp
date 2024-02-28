#include "staticwindow.h"

void StaticWindow::reset_table(void) {
    for(int r = 1; r < result_table->rowCount(); r++) {
        for(int c = 0; c < result_table->columnCount(); c++) {
            result_table->setItem(r, c, new QTableWidgetItem(""));
        }
    }
}

void StaticWindow::save_table(void) {
    QString filename = QFileDialog::getSaveFileName(NULL, tr("Save data"), "", tr("CSV file (*.csv)"));
    QFile csv_file(filename);
    int rows = table->rowCount();
    int columns = table->columnCount();

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            textData += table->item(i, j)->data(0).toString();
            textData += ", ";   // for .csv file format
        }
        textData += "\n";             // (optional: for new line segmentation)
    }
    if(csv_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&csv_file);
        out << csv_data;
        csv_file.close();
    }
}

QString StaticWindow::vectorToString(std::vector<QString> vector) {
    QString str;
    for(size_t i = 0; i < vector.size(); i++) {
        if (i == 0) {
            str.append(vector[i]);
        } else {
            str.append(" " + vector[i]);
        }
    }
    return str;
}

void StaticWindow::update_buttons(void) {
    for(int i = 0; i < button_max; i++) {
        SerialButton * temp_button = serial_buttons[i];
        if (temp_button->connected && temp_button->configured) {
            temp_button->setStyleSheet("background-color: rgba(0, 0, 255, 0.4);");
        } else if (temp_button->connected) {
            temp_button->setStyleSheet("background-color: rgba(46, 204, 113, 0.4);");
        } else {
            temp_button->setStyleSheet("");
        }
    }
    for(int i = 0; i < test_max; i++) {
        TestButton * temp_button = test_buttons[i];
        if (temp_button->selected) {
            temp_button->setStyleSheet("background-color: rgba(46, 204, 113, 0.4);");
        } else {
            temp_button->setStyleSheet("");
        }
    }
}

void StaticWindow::serialHandle(SerialThread::Mode mode) {
    switch(mode) {
        case SerialThread::Mode::NONE:
            break;
        case SerialThread::Mode::AUTOCONNECT:
            update_buttons();
            break;
        case SerialThread::Mode::CONFIGURE:
            update_buttons();
            break;
        case SerialThread::Mode::TEST:
            break;
    }
}

void StaticWindow::autoconnect(void) {
    if (serial_thread->isRunning()) {
        return;
    }
    serial_thread->serial_set(serial_buttons);
    serial_thread->mode = SerialThread::Mode::AUTOCONNECT;
    serial_thread->start();
}

void StaticWindow::configure(void) {
    if (serial_thread->isRunning()) {
        return;
    }
    serial_thread->serial_set(serial_buttons);
    serial_thread->test_set(test_buttons);
    serial_thread->configuration_set(configuration_data);
    serial_thread->mode = SerialThread::Mode::CONFIGURE;
    serial_thread->start();

}

void StaticWindow::indexChanged(int index)
{
    configuration_data = gan_data[index];
    configuration_text->setText(configuration_label + "\n" + vectorToString(configuration_data));
}


StaticWindow::StaticWindow(QWidget *parent)
    : QMainWindow{parent}
{
    this->setFixedSize(QSize(1000, 350));
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/Logo.ico"));
    this->setWindowTitle("Static Testing");
    central_widget = new QWidget(this);
    this->setCentralWidget(central_widget);
    central_vbox = new QVBoxLayout();
    central_widget->setLayout(central_vbox);
    hbox_0 = new QHBoxLayout();
    central_vbox->addLayout(hbox_0);
    hbox_1 = new QHBoxLayout();
    central_vbox->addLayout(hbox_1);
    hbox_2 = new QHBoxLayout();
    central_vbox->addLayout(hbox_2);
    hbox_3 = new QHBoxLayout();
    central_vbox->addLayout(hbox_3);

    row_combox = new QComboBox(this);
    for(int i = 0; i < max_row; i++) {
        row_combox->addItem(QString::number(i+1));
    }
    col_combox = new QComboBox(this);
    for(int i = 0; i < max_col; i++) {
        col_combox->addItem(QString::number(i+1));
    }
    hbox_0->addWidget(row_combox);
    hbox_0->addWidget(col_combox);

    gan_combox = new QComboBox(this);
    hbox_0->addWidget(gan_combox);
    configuration_text = new QTextEdit("", this);
    hbox_0->addWidget(configuration_text);
    result_table = new QTableWidget(26, 20, this);
    hbox_0->addWidget(result_table);
    gan_data = std::vector<std::vector<QString>>(max_gan, std::vector<QString>(max_spec, ""));
    QFile file(QCoreApplication::applicationDirPath() + "/GaNData.csv");
    file.open(QIODevice::ReadOnly);
    int idx = 0;
    int toggle = 0;
    while (!file.atEnd() && (idx < max_gan)) {
        QString pre_line = file.readLine();
        QList<QString> line = pre_line.split(',');
        if (toggle == 1) {
            for(int c = 0; c < max_spec; c++) {
                QString element = line[c].trimmed();
                gan_data[idx][c] = element;
            }
            gan_combox->addItem(line[0].trimmed());
            idx++;
        } else {
            configuration_label = pre_line.trimmed().replace(",", " ");
            toggle++;
        }
    }
    connect(gan_combox, SIGNAL(activated(int)), this, SLOT(indexChanged(int)));
    configuration_data = gan_data[0];
    configuration_text->setText(configuration_label + "\n" + vectorToString(configuration_data));

    serial_buttons = std::vector<SerialButton *>(button_max);
    serial_buttons[0] = new SerialButton("MCU");
    serial_buttons[0]->setLabel(SerialButton::Equipment::MCU);
    serial_buttons[0]->setTarget("MCU\n");
    serial_buttons[1] = new SerialButton("PSU");
    serial_buttons[1]->setLabel(SerialButton::Equipment::PSU);
    serial_buttons[1]->setTarget("ITECH Ltd., IT6302, 800071020777520196, 1.05-1.04\n");
    serial_buttons[2] = new SerialButton("HIPOT");
    serial_buttons[2]->setLabel(SerialButton::Equipment::HIPOT);
    serial_buttons[2]->setTarget("Tonghui,TH9320,Version:N1.4.7\n");
    serial_buttons[3] = new SerialButton("LCR");
    serial_buttons[3]->setLabel(SerialButton::Equipment::LCR);
    serial_buttons[3]->setTarget("Tonghui, TH2827C,Ver 1.0.1  , Hardware Ver C6.0\n");
    for(int i = 0; i < button_max; i++) {
        serial_buttons[i]->setPort(nullptr);
        hbox_1->addWidget(serial_buttons[i]);
    }
    autoconnect_button = new QPushButton("autoconnect");
    hbox_1->addWidget(autoconnect_button);
    connect(autoconnect_button, &QPushButton::released, this, &StaticWindow::autoconnect);

    test_buttons = std::vector<TestButton *>(test_max);
    test_buttons[0] = new TestButton("BV");
    test_buttons[0]->setLabel(TestButton::Test::BV);
    test_buttons[0]->required.insert(test_buttons[0]->required.end(), {SerialButton::Equipment::MCU, SerialButton::Equipment::PSU, SerialButton::Equipment::HIPOT});
    test_buttons[1] = new TestButton("VTH");
    test_buttons[1]->setLabel(TestButton::Test::VTH);
    test_buttons[1]->required.insert(test_buttons[1]->required.end(), {SerialButton::Equipment::MCU, SerialButton::Equipment::PSU});
    test_buttons[2] = new TestButton("RDSON");
    test_buttons[2]->setLabel(TestButton::Test::RDSON);
    test_buttons[2]->required.insert(test_buttons[2]->required.end(), {SerialButton::Equipment::MCU, SerialButton::Equipment::PSU, SerialButton::Equipment::LCR});
    test_buttons[3] = new TestButton("BVSTEP");
    test_buttons[3]->setLabel(TestButton::Test::BVSTEP);
    test_buttons[3]->required.insert(test_buttons[3]->required.end(), {SerialButton::Equipment::HIPOT});
    for(int i = 0; i < test_max; i++) {
        hbox_2->addWidget(test_buttons[i]);
        connect(test_buttons[i], &QPushButton::released, this, [this, i](void) {
            for (int j = 0; j < test_max; j++) {
                test_buttons[j]->selected = 0;
            }
            for (int k = 0; k < button_max; k++) {
                serial_buttons[k]->configured = 0;
            }
            test_buttons[i]->selected = 1;
            update_buttons();
        });
    }
    configure_button = new QPushButton("configure");
    hbox_2->addWidget(configure_button);
    connect(configure_button, &QPushButton::released, this, &StaticWindow::configure);

    new_button = new QPushButton("new");
    hbox_3->addWidget(new_button);
    connect(new_button, &QPushButton::released, this, &StaticWindow::reset_table);
    test_button = new QPushButton("test");
    hbox_3->addWidget(test_button);
    save_button = new QPushButton("save");
    hbox_3->addWidget(save_button);
    connect(save_Button, &QPushButton::released, this, &StaticWindow::save_table);

    serial_thread = new SerialThread(this);
    connect(serial_thread, &SerialThread::serialComplete, this, &StaticWindow::serialHandle);
    serial_thread->mode = SerialThread::Mode::NONE;
    serial_thread->timeout_msec = 200;
    serial_thread->buffer_max = 128;

    central_vbox->setAlignment(Qt::AlignTop);
    hbox_0->setAlignment(Qt::AlignTop);
    hbox_1->setAlignment(Qt::AlignTop);
    hbox_2->setAlignment(Qt::AlignTop);
    row_combox->setMaximumHeight(300);
    col_combox->setMaximumHeight(300);
    gan_combox->setMaximumHeight(300);
    configuration_text->setMaximumHeight(300);
    configuration_text->setReadOnly(1);
    result_table->setMaximumHeight(300);
    for(int r = 0; r < result_table->rowCount(); r++) {
        for(int c = 0; c < result_table->columnCount(); c++) {
            result_table->setItem(r, c, new QTableWidgetItem(""));
        }
    }
    result_table->setItem(0, 0, new QTableWidgetItem("Date"));
    result_table->setItem(0, 1, new QTableWidgetItem("Row"));
    result_table->setItem(0, 2, new QTableWidgetItem("Column"));
}

void StaticWindow::closeEvent(QCloseEvent * event) {
    emit signal_1();
}
