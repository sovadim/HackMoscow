#include "devices.h"
#include "ui_devices.h"
#include <QSerialPortInfo>
#include <pulse_device/list_devices.h>

#define POSSIBLE_AMOUNT_OF_DEVICES 32

Devices::Devices(QWidget *parent, Player *player) :
    QWidget(parent),
    ui(new Ui::devices),
    player(player),
    sound_card_list(new QStringListModel(this)),
    ports_list(new QStringListModel(this)) {

    ui->setupUi(this);
    load_cards();
    load_ports();
}

Devices::~Devices() {
    delete ui;
    delete sound_card_list;
    delete ports_list;
}

void Devices::on_select_card_clicked() {
    player->capture_device.isSet = false;
    player->capture_device.set_device(ui->soundCardList->currentIndex().data().toString().toUtf8().data());
    emit start_capture();
}

void Devices::on_select_port_clicked() {
    if (ui->portsList->currentIndex().data().toString() != "") {
        emit start_port(QSerialPortInfo::availablePorts()[ui->soundCardList->currentIndex().data().toInt()]);
    }
}

void Devices::load_ports() {
    QStringList portStringList;
    for (int i = 0; i < QSerialPortInfo::availablePorts().length(); ++i) {
        portStringList << QSerialPortInfo::availablePorts()[i].portName();
    }
    ports_list->setStringList(portStringList);
    ui->portsList->setModel(ports_list);
}

void Devices::load_cards() {
    QStringList deviceStringList;

    auto pa_input_devicelist = (pa_devicelist_t *) calloc(POSSIBLE_AMOUNT_OF_DEVICES, sizeof(pa_devicelist_t));
    auto pa_output_devicelist = (pa_devicelist_t *) calloc(POSSIBLE_AMOUNT_OF_DEVICES, sizeof(pa_devicelist_t));
    if (pa_get_devicelist(pa_input_devicelist, pa_output_devicelist) < 0) {
        return;
    }
    for (int32_t i = 0; i < POSSIBLE_AMOUNT_OF_DEVICES; i++) {
        if (!pa_output_devicelist[i].initialized) {
            break;
        }
        deviceStringList << pa_output_devicelist[i].name;
    }
    for (int32_t i = 0; i < POSSIBLE_AMOUNT_OF_DEVICES; i++) {
        if (!pa_input_devicelist[i].initialized) {
            break;
        }
        deviceStringList << pa_input_devicelist[i].name;
    }
    free(pa_input_devicelist);
    free(pa_output_devicelist);
    sound_card_list->setStringList(deviceStringList);
    ui->soundCardList->setModel(sound_card_list);
}
