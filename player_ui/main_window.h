#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <player.h>
#include "polygon.h"
#include <deque>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, Player *player = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *) override;

signals:
    void open_colors();
    void open_animation();
    void open_devices();

private slots:
    void on_colors_clicked();
    void on_devices_clicked();
    void on_animation_clicked();

private:
    Ui::MainWindow *ui;
    Player *player;
    std::deque<Polygon *> polygons;
    double *transformation_matrix;
};


#endif // MAINWINDOW_H
