#include <QApplication>
#include <QString>
#include "main_window.h"
#include "player.h"
#include "serial_thread.h"
#include "audio_thread.h"
#include "fft_thread.h"
#include "animation.h"
#include "color.h"
#include "devices.h"

#define DELAY 10000
#define SAMPLE_SIZE 512  // connected to BASS_FFT_1024


//TODO: on destruction!
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    RGBParameters params(SAMPLE_SIZE / 4, 0, 0, 255, 127,
                         255. / 5000, 80, false);
    Player player(DELAY, SAMPLE_SIZE, params);

    Animation animation(nullptr, &player);
    Color color(nullptr, &player);
    Devices devices(nullptr, &player);
    SerialThread serial_thread(&player);
    AudioThread audio_thread(&player);
    FFTWThread fftw_thread(&player);
    MainWindow main_window(nullptr, &player);

    QObject::connect(&main_window, SIGNAL(open_colors()), &color, SLOT(showMaximized()));
    QObject::connect(&main_window, SIGNAL(open_animation()), &animation, SLOT(showMaximized()));
    QObject::connect(&main_window, SIGNAL(open_devices()), &devices, SLOT(showMaximized()));
    QObject::connect(&devices, SIGNAL(start_capture()), &audio_thread, SLOT(start()));
    QObject::connect(&devices, SIGNAL(start_capture()), &fftw_thread, SLOT(start()));
    QObject::connect(&devices, SIGNAL(start_port(const QSerialPortInfo &)), &serial_thread, SLOT(start_port(const QSerialPortInfo &)));
    QObject::connect(&fftw_thread, SIGNAL(new_data()), &color, SLOT(update()));
    QObject::connect(&fftw_thread, SIGNAL(new_data()), &main_window, SLOT(update()));
    main_window.showMaximized();
    return QApplication::exec();
}
