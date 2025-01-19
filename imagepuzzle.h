#ifndef IMAGEPUZZLE_H
#define IMAGEPUZZLE_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QPainter>
#include <QMouseEvent>
#include <QDrag>
#include <algorithm>
#include "tools.H"

class ImagePuzzle : public QMainWindow {
    Q_OBJECT

public:
    ImagePuzzle(QWidget *parent = nullptr);
    
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void openImage();
    void startPuzzle();

private:
    void clearPuzzle();
    void showPieces();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void updateRightArea(const QPixmap &pixmap, const QPoint &position);
    bool checkPlacement(const QPoint &expected, const QPoint &actual);
    bool checkCompletion();

    QWidget *leftArea;
    QGridLayout *leftLayout;
    QLabel *rightArea;
    QLineEdit *nInput;
    cv::Mat originalImage;
    std::vector<PieceOfImage> pieces;
    int correctPieces;  // 记录正确放置的拼图块数量
};

#endif // IMAGEPUZZLE_H
