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
    // 自定义的QWidget类，用于处理左侧区域的拖放
    class DropWidget : public QWidget {
    public:
        DropWidget(QWidget* parent = nullptr) : QWidget(parent) {
            setAcceptDrops(true);
        }
        
    protected:
        void dragEnterEvent(QDragEnterEvent* event) override {
            if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
                event->acceptProposedAction();
            }
        }
        
        void dropEvent(QDropEvent* event) override {
            if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
                QByteArray pieceData = event->mimeData()->data("application/x-puzzle-piece");
                QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
                QPixmap pixmap;
                QPoint originalPos;
                dataStream >> pixmap >> originalPos;

                // 获取放置位置
                QPoint dropPos = event->position().toPoint();
                
                // 创建新的标签显示拼图块
                QLabel *pieceLabel = new QLabel(this);
                pieceLabel->setPixmap(pixmap);
                pieceLabel->setStyleSheet("background: transparent; border: none;");
                
                // 保存原始位置信息
                pieceLabel->setProperty("originalCol", originalPos.x());
                pieceLabel->setProperty("originalRow", originalPos.y());

                // 设置标签大小为图片大小
                pieceLabel->setFixedSize(pixmap.size());
                
                // 确保图片完整显示
                pieceLabel->setScaledContents(true);

                // 移动到放置位置，考虑图片大小
                pieceLabel->move(dropPos - QPoint(pixmap.width()/2, pixmap.height()/2));
                
                pieceLabel->setAttribute(Qt::WA_DeleteOnClose);
                pieceLabel->installEventFilter(window());
                pieceLabel->show();

                event->acceptProposedAction();
            }
        }
    };

    // 自定义的QLabel类，用于处理右侧区域的拖放
    class DropLabel : public QLabel {
    public:
        DropLabel(QWidget* parent = nullptr) : QLabel(parent) {
            setAcceptDrops(true);
        }
        
    protected:
        void dragEnterEvent(QDragEnterEvent* event) override {
            if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
                event->acceptProposedAction();
            }
        }
        
        void paintEvent(QPaintEvent *event) override {
            QLabel::paintEvent(event);
            
            // 绘制网格线
            QPainter painter(this);
            painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
            
            ImagePuzzle* puzzle = static_cast<ImagePuzzle*>(window());
            int n = puzzle->nInput->text().toInt();
            
            // 绘制竖线
            for (int i = 1; i < n; ++i) {
                int x = i * puzzle->gridCellWidth;
                painter.drawLine(x, 0, x, height());
            }
            
            // 绘制横线
            for (int i = 1; i < n; ++i) {
                int y = i * puzzle->gridCellHeight;
                painter.drawLine(0, y, width(), y);
            }
        }

        void dropEvent(QDropEvent* event) override {
            if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
                QByteArray pieceData = event->mimeData()->data("application/x-puzzle-piece");
                QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
                QPixmap pixmap;
                QPoint originalPos;
                dataStream >> pixmap >> originalPos;

                // 获取放置位置
                QPoint dropPos = event->position().toPoint();
                
                // 创建新的标签显示拼图块
                QLabel *pieceLabel = new QLabel(this);
                
                // 缩放图片以适应网格大小，但保持原始宽高比
                ImagePuzzle* puzzle = static_cast<ImagePuzzle*>(window());
                QPixmap scaledPixmap = pixmap.scaled(puzzle->gridCellWidth, puzzle->gridCellHeight, 
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation);
                
                pieceLabel->setPixmap(scaledPixmap);
                pieceLabel->setStyleSheet("background: transparent; border: none;");
                pieceLabel->setFixedSize(scaledPixmap.size());
                
                // 保存原始位置信息
                pieceLabel->setProperty("originalCol", originalPos.x());
                pieceLabel->setProperty("originalRow", originalPos.y());
                
                // 保存原始大小的图片，用于拖动时恢复
                pieceLabel->setProperty("originalPixmap", pixmap);

                // 直接使用鼠标放置位置，不强制对齐到网格
                pieceLabel->move(dropPos - QPoint(scaledPixmap.width()/2, scaledPixmap.height()/2));
                
                pieceLabel->setAttribute(Qt::WA_DeleteOnClose);
                pieceLabel->installEventFilter(window());
                pieceLabel->show();

                event->acceptProposedAction();

                // 检查是否完成拼图
                puzzle->checkCompletion();
            }
        }
    };

    void clearPuzzle();
    void showPieces();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void updateRightArea(const QPixmap &pixmap, const QPoint &position);
    bool checkPlacement(const QPoint &actual);
    bool checkCompletion();

    QWidget *leftArea;
    DropLabel *rightArea;
    QGridLayout *leftLayout;
    QLineEdit *nInput;
    cv::Mat originalImage;
    std::vector<PieceOfImage> pieces;
    int correctPieces;  // 记录正确放置的拼图块数量
    int gridCellWidth;  // 网格单元格宽度
    int gridCellHeight; // 网格单元格高度
};

#endif // IMAGEPUZZLE_H
