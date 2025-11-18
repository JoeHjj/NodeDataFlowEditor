#pragma once

#include <QMainWindow>

class GraphScene;
class GraphView;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    GraphScene* m_scene;
    GraphView* m_view;
};
