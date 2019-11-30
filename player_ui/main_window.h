#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <player.h>
#include "polygon.h"
#include "fixed_queue.h"
#define POLYGON_AMOUNT 30

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, Player *player = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void open_colors();
    void open_animation();
    void open_devices();

public slots:
    void animation_changed(int verteces, const fPoint *vectors, float radius, int mode);
    void rotation_changed(float rot);

private slots:
    void on_colors_clicked();
    void on_devices_clicked();
    void on_animation_clicked();

private:
    Ui::MainWindow *ui;
    Player *player;
    Polygon *base_polygon;
    fixed_queue<Polygon *, POLYGON_AMOUNT> *polygons;
    fPoint *transformation_matrix;
};


#endif // MAINWINDOW_H
