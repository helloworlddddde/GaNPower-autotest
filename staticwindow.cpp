#include "staticwindow.h"

QList<QSerialPortInfo> dialog_infos;
SerialButton * dialog_button = nullptr;
QProgressDialog * progress_dialog;
QPushButton * cancel_button;
QQueue<QString> str_queue;
ViSession default_rm_static;

void StaticWindow::onCompute(QString choice) {
    int idx = choice.split(":").at(0).toInt();
    serial_thread->setManualPort(dialog_button, dialog_infos[idx]);
    serial_thread->mode = SerialThread::Mode::MANUALCONNECT;
    serial_thread->start();
}

void StaticWindow::updateTable(int row, int col, QString val) {
    result_table->setItem(row, col, new QTableWidgetItem(val));
}
void StaticWindow::debug_slot(QString str) {
    str_queue.enqueue(str);
    if (str_queue.size() > 10) {
        str_queue.dequeue();
    }
    QString debug_output = "";
    for(int i = 0; i < str_queue.size(); i++) {
        debug_output += str_queue.at(i) + "\n";
    }
    debug_text->setText(debug_output);
    debug_text->verticalScrollBar()->setValue(debug_text->verticalScrollBar()->maximum());
}

void StaticWindow::updateProgress(int value) {
    progress_dialog->setValue(value);
}

void StaticWindow::reset_table(void) {
    for(int r = 1; r < result_table->rowCount(); r++) {
        for(int c = 0; c < result_table->columnCount(); c++) {
            result_table->setItem(r, c, new QTableWidgetItem(""));
        }
    }
}

void StaticWindow::save_table(void) {
    QString filename = QFileDialog::getSaveFileName(NULL, tr("Save Test Data"), "", tr("CSV file (*.csv)"));
    QString csv_data = "";
    QFile csv_file(filename);
    for (int i = 0; i < result_table->rowCount(); i++) {
        for (int j = 0; j < result_table->columnCount(); j++) {
            csv_data += result_table->item(i, j)->data(0).toString();
            csv_data += ", ";
        }
        csv_data += "\n";
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
    for(size_t i = 0; i < serial_buttons.size(); i++) {
        SerialButton * temp_button = serial_buttons[i];
        if (temp_button->connected && temp_button->configured) {
            temp_button->setStyleSheet("background-color: rgba(0, 0, 255, 0.4);");
        } else if (temp_button->connected) {
            temp_button->setStyleSheet("background-color: rgba(46, 204, 113, 0.4);");
        } else {
            temp_button->setStyleSheet("");
        }
    }
    for(size_t i = 0; i < test_buttons.size(); i++) {
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
        case SerialThread::Mode::MANUALCONNECT:
            dialog_infos.clear();
            dialog_button = nullptr;
            update_buttons();
            break;
        case SerialThread::Mode::AUTOCONNECT:
            for(size_t i = 0; i < serial_buttons.size(); i++) {
                serial_buttons[i]->configured = 0;
            }
            update_buttons();
            break;
        case SerialThread::Mode::CONFIGURE:
            update_buttons();
            break;
        case SerialThread::Mode::TEST:
            for(size_t i = 0; i < serial_buttons.size(); i++) {
                serial_buttons[i]->configured = 0;
            }
            progress_dialog->hide();
            delete(cancel_button);
            delete(progress_dialog);
            serial_thread->abort = 0;
            update_buttons();
            break;
    }
}

void StaticWindow::autoconnect(void) {
    if (serial_thread->isRunning()) {
        return;
    }
    serial_thread->test_set(test_buttons);
    serial_thread->mode = SerialThread::Mode::AUTOCONNECT;
    serial_thread->start();
}

void StaticWindow::configure(void) {
    if (serial_thread->isRunning()) {
        return;
    }
    serial_thread->test_set(test_buttons);
    serial_thread->configuration_set(configuration_data);
    serial_thread->step_set(step_data);
    serial_thread->vthstep_data = vthstep_data;
    serial_thread->mode = SerialThread::Mode::CONFIGURE;
    serial_thread->start();
}

void StaticWindow::test(void) {
    if (serial_thread->isRunning()) {
        return;
    }
    progress_dialog = new QProgressDialog(this);
    cancel_button = new QPushButton("Cancel", this);
    progress_dialog->setAutoClose(0);
    progress_dialog->setWindowTitle("Test Progress");
    progress_dialog->setCancelButton(cancel_button);
    connect(cancel_button, &QPushButton::released, serial_thread, &SerialThread::setAbort);
    progress_dialog->show();
    serial_thread->test_set(test_buttons);
    serial_thread->configuration_set(configuration_data);
    serial_thread->step_set(step_data);
    serial_thread->vthstep_data = vthstep_data;
    serial_thread->setMaxRow(row_combox->currentIndex() + 1);
    serial_thread->setMaxCol(col_combox->currentIndex() + 1);
    serial_thread->mode = SerialThread::Mode::TEST;
    serial_thread->start();
}

StaticWindow::StaticWindow(QWidget *parent)
    : QMainWindow{parent}
{
    viOpenDefaultRM (&default_rm_static);
    this->setFixedSize(QSize(1250, 350));
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

    select_vbox = new QVBoxLayout();
    hbox_0->addLayout(select_vbox);
    step_button = new QPushButton("bv step");
    select_vbox->addWidget(step_button);
    connect(step_button, &QPushButton::released, this, [this](void) {
        if (serial_thread->isRunning()) {
            return;
        }
        step_data.clear();
        step_label = "";
        QString filename = QFileDialog::getOpenFileName(NULL, tr("Get Step Data"), "", tr("CSV file (*.csv)"));
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        int toggle = 0;
        int idx = 0;
        while (!file.atEnd()) {
            QString pre_line = file.readLine();
            QList<QString> line = pre_line.split(',');
            if (toggle == 1) {
                step_data.push_back(std::vector<QString>());
                for(int c = 0; c < line.size(); c++) {
                    QString element = line[c].trimmed();
                    step_data[idx].push_back(element);
                }
                idx++;
            } else {
                QString trimmed = pre_line.trimmed().replace(",", " ");
                if (trimmed.compare("STEP (V) UPPR (mA) TIME (s) RISE (s) FALL (s)")) {
                    step_text->setText("");
                    return;
                }
                step_label = pre_line.trimmed().replace(",", " ");
                toggle++;
            }
        }
        QString pre_text = step_label;
        for(size_t i = 0; i < step_data.size(); i++) {
            pre_text += "\n" + vectorToString(step_data[i]);
        }
        step_text->setText(pre_text);
    });

    vthstep_button = new QPushButton("vth step");
    select_vbox->addWidget(vthstep_button);
    connect(vthstep_button, &QPushButton::released, this, [this](void) {
        if (serial_thread->isRunning()) {
            return;
        }
        vthstep_data.clear();
        vthstep_label = "";
        QString filename = QFileDialog::getOpenFileName(NULL, tr("Get Step Data"), "", tr("CSV file (*.csv)"));
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        int toggle = 0;
        int idx = 0;
        while (!file.atEnd()) {
            QString pre_line = file.readLine();
            QList<QString> line = pre_line.split(',');
            if (toggle == 1) {
                vthstep_data.push_back(std::vector<QString>());
                for(int c = 0; c < line.size(); c++) {
                    QString element = line[c].trimmed();
                    vthstep_data[idx].push_back(element);
                }
                idx++;
            } else {
                QString trimmed = pre_line.trimmed().replace(",", " ");
                if (trimmed.compare("STEP (V)")) {
                    vthstep_text->setText("");
                    return;
                }
                vthstep_label = pre_line.trimmed().replace(",", " ");
                toggle++;
            }
        }
        QString pre_text = vthstep_label;
        for(size_t i = 0; i < vthstep_data.size(); i++) {
            pre_text += "\n" + vectorToString(vthstep_data[i]);
        }
        vthstep_text->setText(pre_text);

    });

    gan_combox = new QComboBox(this);
    select_vbox->addWidget(gan_combox);
    select_button = new QPushButton("select");
    select_vbox->addWidget(select_button);
    step_vbox = new QVBoxLayout();
    hbox_0->addLayout(step_vbox);
    step_text = new QTextEdit("", this);
    step_vbox->addWidget(step_text);
    vthstep_text = new QTextEdit("", this);
    step_vbox->addWidget(vthstep_text);
    configuration_text = new QTextEdit("", this);
    step_vbox->addWidget(configuration_text);
    result_table = new QTableWidget(10001, 20, this);
    hbox_0->addWidget(result_table);
    QFile file(QCoreApplication::applicationDirPath() + "/GaNData.csv");
    file.open(QIODevice::ReadOnly);
    int idx = 0;
    int toggle = 0;
    while (!file.atEnd()) {
        QString pre_line = file.readLine();
        QList<QString> line = pre_line.split(',');
        if (toggle == 1) {
            gan_data.push_back(std::vector<QString>());
            for(int c = 0; c < line.size(); c++) {
                QString element = line[c].trimmed();
                gan_data[idx].push_back(element);
            }
            gan_combox->addItem(line[0].trimmed());
            idx++;
        } else {
            configuration_label = pre_line.trimmed().replace(",", " ");
            toggle++;
        }
    }
    connect(select_button, &QPushButton::released, this, [this](void) {
        if (serial_thread->isRunning()) {
            return;
        }
        configuration_data = gan_data[gan_combox->currentIndex()];
        configuration_text->setText(configuration_label + "\n" + vectorToString(configuration_data));
        for(size_t i = 0; i < serial_buttons.size(); i++) {
            serial_buttons[i]->configured = 0;
        }
        update_buttons();
    });
    configuration_data = gan_data[0];
    configuration_text->setText(configuration_label + "\n" + vectorToString(configuration_data));

    serial_buttons = std::vector<SerialButton *>();
    serial_buttons.push_back(new SerialButton("MCU"));
    serial_buttons[0]->setLabel(SerialButton::Equipment::MCU);
    serial_buttons[0]->setTarget("MCU\n");
    serial_buttons.push_back(new SerialButton("PSU"));
    serial_buttons[1]->setLabel(SerialButton::Equipment::PSU);
    serial_buttons[1]->setTarget("IT6302");
    serial_buttons.push_back(new SerialButton("HIPOT"));
    serial_buttons[2]->setLabel(SerialButton::Equipment::HIPOT);
    serial_buttons[2]->setTarget("TH9320");
    serial_buttons.push_back(new SerialButton("LCR"));
    serial_buttons[3]->setLabel(SerialButton::Equipment::LCR);
    serial_buttons[3]->setTarget("TH2827C");
    serial_buttons.push_back(new VisaButton("DMM"));
    serial_buttons.back()->setLabel(SerialButton::Equipment::DMM);
    serial_buttons.back()->setTarget("34465A");
    VisaButton * temp4 = (VisaButton *) serial_buttons.back();
    temp4->visa_port = ViString("USB0::0x2A8D::0x0101::MY60091049::INSTR");

    for(size_t i = 0; i < serial_buttons.size(); i++) {
        serial_buttons[i]->setPort(nullptr);
        hbox_1->addWidget(serial_buttons[i]);
        if (serial_buttons[i]->isVisa()) {
            connect(serial_buttons[i], &QPushButton::released, this, [this, i](void) {
                if (serial_thread->isRunning()) {
                    return;
                }
                serial_thread->setManualPort(serial_buttons[i], (QSerialPortInfo) NULL);
                serial_thread->mode = SerialThread::Mode::MANUALCONNECT;
                serial_thread->start();
            });
            continue;
        }
        connect(serial_buttons[i], &QPushButton::released, this, [this, i](void) {
            if (serial_thread->isRunning()) {
                return;
            }
            QList<QSerialPortInfo> serial_infos = QSerialPortInfo::availablePorts();
            QInputDialog serial_dialog;
            serial_dialog.setLabelText("select port");
            serial_dialog.setWindowTitle("Manual Connect");
            QStringList connections;
            for(int i = 0; i < serial_infos.size(); i++) {
                connections << QString::number(i) + ":" + QString(serial_infos[i].portName() + " " + serial_infos[i].description());
            }
            if (connections.size() == 0) {
                debug_slot("No port found");
                return;
            }
            serial_dialog.setComboBoxItems(connections);
            serial_dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
            dialog_infos = serial_infos;
            dialog_button = serial_buttons[i];
            connect(&serial_dialog, SIGNAL(textValueSelected(QString)), this, SLOT(onCompute(QString)));
            serial_dialog.exec();
        });
    }
    autoconnect_button = new QPushButton("autoconnect");
    hbox_1->addWidget(autoconnect_button);
    connect(autoconnect_button, &QPushButton::released, this, &StaticWindow::autoconnect);

    test_buttons = std::vector<TestButton *>();
    test_buttons.push_back(new TestButton("BV"));
    test_buttons[0]->setLabel(TestButton::Test::BV);
    test_buttons[0]->required.insert(test_buttons[0]->required.end(), {&serial_buttons[0], &serial_buttons[1], &serial_buttons[2]});
    test_buttons[0]->setupToolTip();
    test_buttons.push_back(new TestButton("VTH"));
    test_buttons[1]->setLabel(TestButton::Test::VTH);
    test_buttons[1]->required.insert(test_buttons[1]->required.end(), {&serial_buttons[0], &serial_buttons[1]});
    test_buttons[1]->setupToolTip();
    test_buttons.push_back(new TestButton("RDSON"));
    test_buttons[2]->setLabel(TestButton::Test::RDSON);
    test_buttons[2]->required.insert(test_buttons[2]->required.end(), {&serial_buttons[0], &serial_buttons[1], &serial_buttons[3]});
    test_buttons[2]->setupToolTip();
    test_buttons.push_back(new TestButton("BVSTEP"));
    test_buttons[3]->setLabel(TestButton::Test::BVSTEP);
    test_buttons[3]->required.insert(test_buttons[3]->required.end(), {&serial_buttons[2]});
    test_buttons[3]->setupToolTip();
    test_buttons.push_back(new TestButton("VTHSTEP"));
    test_buttons[4]->setLabel(TestButton::Test::VTHSTEP);
    test_buttons[4]->required.insert(test_buttons[4]->required.end(), {&serial_buttons[1], &serial_buttons[4]});
    test_buttons[4]->setupToolTip();
    test_buttons.push_back(new TestButton("VTHSTEP2"));
    test_buttons.back()->setLabel(TestButton::Test::VTHSTEP2);
    test_buttons.back()->required.insert(test_buttons.back()->required.end(), {&serial_buttons[0], &serial_buttons[1], &serial_buttons[4]});
    test_buttons.back()->setupToolTip();
    test_buttons.push_back(new TestButton("VTHSTEP3"));
    test_buttons.back()->setLabel(TestButton::Test::VTHSTEP3);
    test_buttons.back()->required.insert(test_buttons.back()->required.end(), {&serial_buttons[0], &serial_buttons[1], &serial_buttons[4]});
    test_buttons.back()->setupToolTip();
    for(size_t i = 0; i < test_buttons.size(); i++) {
        hbox_2->addWidget(test_buttons[i]);
        connect(test_buttons[i], &QPushButton::released, this, [this, i](void) {
            if (serial_thread->isRunning()) {
                return;
            }
            for (size_t j = 0; j < test_buttons.size(); j++) {
                test_buttons[j]->selected = 0;
            }
            for (size_t k = 0; k < serial_buttons.size(); k++) {
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
    connect(test_button, &QPushButton::released, this, &StaticWindow::test);
    save_button = new QPushButton("save");
    hbox_3->addWidget(save_button);
    connect(save_button, &QPushButton::released, this, &StaticWindow::save_table);

    serial_thread = new SerialThread(this);
    connect(serial_thread, &SerialThread::serialComplete, this, &StaticWindow::serialHandle);
    serial_thread->mode = SerialThread::Mode::NONE;
    serial_thread->timeout_msec = 200;
    serial_thread->buffer_max = 128;
    serial_thread->default_rm = &default_rm_static;

    central_vbox->setAlignment(Qt::AlignTop);
    hbox_0->setAlignment(Qt::AlignTop);
    hbox_1->setAlignment(Qt::AlignTop);
    hbox_2->setAlignment(Qt::AlignTop);
    // row_combox->setMaximumHeight(300);
    // col_combox->setMaximumHeight(300);
    // step_button->setMaximumHeight(300);
    // select_button->setMaximumHeight(300);
    // gan_combox->setMaximumHeight(300);
    // configuration_text->setMaximumHeight(300);
    configuration_text->setReadOnly(1);
    step_text->setReadOnly(1);
    vthstep_text->setReadOnly(1);
    result_table->setMaximumHeight(300);
    for(int r = 0; r < result_table->rowCount(); r++) {
        for(int c = 0; c < result_table->columnCount(); c++) {
            result_table->setItem(r, c, new QTableWidgetItem(""));
        }
    }
    result_table->setItem(0, 0, new QTableWidgetItem("Date"));
    result_table->setItem(0, 1, new QTableWidgetItem("Column"));
    result_table->setItem(0, 2, new QTableWidgetItem("Row"));
    result_table->setItem(0, 3, new QTableWidgetItem("Device"));
    result_table->setItem(0, 4, new QTableWidgetItem("BV (V)"));
    result_table->setItem(0, 5, new QTableWidgetItem("BV IDSS (uA)"));
    result_table->setItem(0, 6, new QTableWidgetItem("BV P/F"));
    result_table->setItem(0, 7, new QTableWidgetItem("VTH low (mA)"));
    result_table->setItem(0, 8, new QTableWidgetItem("VTH high (mA)"));
    result_table->setItem(0, 9, new QTableWidgetItem("VTH P/F"));
    result_table->setItem(0, 10, new QTableWidgetItem("RDSON (mΩ)"));
    result_table->setItem(0, 11, new QTableWidgetItem("RDSON P/F"));
    connect(serial_thread, &SerialThread::progressUpdate, this, &StaticWindow::updateProgress);
    debug_text = new QTextEdit("");
    step_vbox->addWidget(debug_text);
    debug_text->setReadOnly(1);
    connect(serial_thread, &SerialThread::debug, this, &StaticWindow::debug_slot);
    connect(serial_thread, &SerialThread::tableUpdate, this, &StaticWindow::updateTable);
}

void StaticWindow::closeEvent(QCloseEvent * event) {
    serial_thread->wait();
    delete(serial_thread);
    for(size_t i = 0; i < serial_buttons.size(); i++) {
        if (serial_buttons[i]->getPort() != nullptr) {
            serial_buttons[i]->getPort()->moveToThread(this->thread());
            serial_buttons[i]->getPort()->close();
            delete(serial_buttons[i]->getPort());
            serial_buttons[i]->setPort(nullptr);
        }
    }
    delete(central_widget);
    str_queue.clear();
    emit signal_1();
}

