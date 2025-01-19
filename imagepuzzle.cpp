#include "imagepuzzle.h"
#include "tools.H"
#include "createalpha.h"
#include "pieceofimage.h"
#include <QRandomGenerator>
#include <cmath>

ImagePuzzle::ImagePuzzle(QWidget *parent) : QMainWindow(parent) {
    // 主窗口布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 工具栏
    QWidget *toolbar = new QWidget(this);
    toolbar->setStyleSheet(
        "QWidget {"
        "   border-bottom: 1px solid palette(dark);"
        "   background: palette(window);"
        "}"
    );
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setSpacing(15);
    toolbarLayout->setContentsMargins(15, 8, 15, 8);

    // 打开图片按钮
    QPushButton *openButton = new QPushButton("打开图片", this);
    openButton->setFixedSize(100, 35);
    openButton->setStyleSheet(
        "QPushButton {"
        "   padding: 5px;"
        "   background: palette(button);"
        "   border: 1px solid palette(dark);"
        "   border-radius: 3px;"
        "   color: palette(buttontext);"
        "}"
        "QPushButton:hover {"
        "   background: palette(light);"
        "}"
        "QPushButton:pressed {"
        "   background: palette(dark);"
        "}"
    );
    connect(openButton, &QPushButton::clicked, this, &ImagePuzzle::openImage);
    toolbarLayout->addWidget(openButton);

    // 分割数标签
    QLabel *nLabel = new QLabel("分割数:", this);
    nLabel->setFixedHeight(35);
    nLabel->setStyleSheet(
        "QLabel {"
        "   padding: 5px;"
        "   color: palette(text);"
        "}"
    );
    toolbarLayout->addWidget(nLabel);

    // 分割数输入框
    nInput = new QLineEdit(this);
    nInput->setFixedSize(60, 35);
    nInput->setAlignment(Qt::AlignCenter);
    nInput->setStyleSheet(
        "QLineEdit {"
        "   padding: 5px;"
        "   background: palette(base);"
        "   border: 1px solid palette(dark);"
        "   border-radius: 3px;"
        "   color: palette(text);"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid palette(highlight);"
        "}"
    );
    nInput->setValidator(new QIntValidator(2, 10, this));
    toolbarLayout->addWidget(nInput);

    // 开始按钮
    QPushButton *startButton = new QPushButton("开始", this);
    startButton->setFixedSize(100, 35);
    startButton->setStyleSheet(
        "QPushButton {"
        "   padding: 5px;"
        "   background: palette(button);"
        "   border: 1px solid palette(dark);"
        "   border-radius: 3px;"
        "   color: palette(buttontext);"
        "}"
        "QPushButton:hover {"
        "   background: palette(light);"
        "}"
        "QPushButton:pressed {"
        "   background: palette(dark);"
        "}"
    );
    connect(startButton, &QPushButton::clicked, this, &ImagePuzzle::startPuzzle);
    toolbarLayout->addWidget(startButton);

    // 添加伸缩空间
    toolbarLayout->addStretch();

    // 内容区域
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background: palette(window);");
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);

    // 左侧区域：显示分割后的图片
    leftArea = new QWidget(this);
    leftArea->setAcceptDrops(false);
    leftArea->setStyleSheet("background: palette(window);");
    leftLayout = new QGridLayout(leftArea);
    contentLayout->addWidget(leftArea, 1);

    // 右侧区域：用于重组图片
    rightArea = new QLabel(this);
    rightArea->setAlignment(Qt::AlignCenter);
    rightArea->setStyleSheet(
        "QLabel {"
        "   background: palette(base);"
        "   border: 2px dashed palette(dark);"
        "}"
    );
    rightArea->setAcceptDrops(true);
    contentLayout->addWidget(rightArea, 1);

    // 设置主布局
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(contentWidget, 1);  // 内容区域可拉伸
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setCentralWidget(centralWidget);
    setWindowTitle("图片拼图游戏");
    resize(1000, 700);  // 增大窗口尺寸
}

void ImagePuzzle::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
        event->acceptProposedAction();
    }
}

void ImagePuzzle::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
        QByteArray pieceData = event->mimeData()->data("application/x-puzzle-piece");
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QPoint position;
        dataStream >> pixmap >> position;

        if (checkPlacement(position, event->pos())) {
            updateRightArea(pixmap, position);
            event->acceptProposedAction();
        }
    }
}

void ImagePuzzle::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "打开图片", "", "图片文件 (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        originalImage = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
        if (originalImage.empty()) {
            QMessageBox::warning(this, "错误", "无法打开图片文件");
        }
    }
}

void ImagePuzzle::startPuzzle() {
    if (originalImage.empty()) {
        QMessageBox::warning(this, "错误", "请先选择图片");
        return;
    }

    // 清除之前的拼图
    clearPuzzle();

    // 获取切割数量
    int n = nInput->text().toInt();
    if (n <= 0) {
        QMessageBox::warning(this, "错误", "请输入有效的切割数量");
        return;
    }

    // 调整右侧区域大小，保持与原图相同的宽高比
    double aspectRatio = (double)originalImage.cols / originalImage.rows;
    int rightAreaHeight = rightArea->height();
    int rightAreaWidth = rightAreaHeight * aspectRatio;
    
    // 如果计算出的宽度超过了可用空间，则按宽度计算高度
    if (rightAreaWidth > rightArea->width()) {
        rightAreaWidth = rightArea->width();
        rightAreaHeight = rightAreaWidth / aspectRatio;
    }
    
    // 设置右侧区域的固定大小
    rightArea->setFixedSize(rightAreaWidth, rightAreaHeight);

    // 创建工具对象并切割图片
    Tools tools;
    pieces = tools.toPartition(originalImage, n, n);
    for(size_t i = 0; i < pieces.size(); i++) {
        tools.toShapeImage(pieces[i]);
    }
    
    // 显示拼图块
    showPieces();
    
    // 重置正确放置的计数
    correctPieces = 0;
}

void ImagePuzzle::clearPuzzle() {
    if (leftLayout) {
        QLayoutItem *child;
        while ((child = leftLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }
    }
    rightArea->clear();
    rightArea->setPixmap(QPixmap());
}

void ImagePuzzle::showPieces() {
    std::random_shuffle(pieces.begin(), pieces.end());

    // 计算每个piece的尺寸
    int pieceWidth = leftArea->width() / 3;
    int pieceHeight = leftArea->height() / 3;

    // 随机位置参数
    int padding = 20;
    int minX = padding;
    int maxX = leftArea->width() - pieceWidth - padding;
    int minY = padding;
    int maxY = leftArea->height() - pieceHeight - padding;

    for (const auto& piece : pieces) {
        QLabel *pieceLabel = new QLabel(leftArea);
        
        // 将OpenCV Mat转换为QImage
        cv::Mat rgbImage;
        cv::cvtColor(piece.shapedImage, rgbImage, cv::COLOR_BGRA2RGBA);
        QImage qImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGBA8888);
        
        // 创建QPixmap并缩放
        QPixmap pixmap = QPixmap::fromImage(qImage);
        pixmap = pixmap.scaled(pieceWidth, pieceHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // 设置随机位置
        int x = QRandomGenerator::global()->bounded(minX, maxX);
        int y = QRandomGenerator::global()->bounded(minY, maxY);
        
        pieceLabel->setPixmap(pixmap);
        pieceLabel->setGeometry(x, y, pieceWidth, pieceHeight);
        pieceLabel->setStyleSheet("background: transparent; border: none;");
        // 保存原始的行列位置
        pieceLabel->setProperty("originalRow", piece.location.row);
        pieceLabel->setProperty("originalCol", piece.location.col);

        pieceLabel->setAttribute(Qt::WA_DeleteOnClose);
        pieceLabel->installEventFilter(this);
        pieceLabel->show();
    }
}

bool ImagePuzzle::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel *child = static_cast<QLabel*>(obj);
        if (!child)
            return false;

        QPixmap pixmap = child->pixmap(Qt::ReturnByValue);
        // 获取原始的行列位置
        QPoint originalPos(child->property("originalCol").toInt(),
                         child->property("originalRow").toInt());

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixmap << originalPos;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-puzzle-piece", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        drag->setHotSpot(mouseEvent->pos());

        // 在开始拖动时立即隐藏原始图片
        child->hide();
        
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            child->close();
        } else {
            // 如果拖动取消，则重新显示原始图片
            child->show();
        }
        
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

bool ImagePuzzle::checkPlacement(const QPoint &expected, const QPoint &actual) {
    // 确保拖放位置在rightArea内
    QPoint localPos = rightArea->mapFrom(this, actual);
    if (!rightArea->rect().contains(localPos)) {
        return false;
    }

    // 使用nInput的值计算网格大小
    int gridSize = nInput->text().toInt();
    int cellWidth = rightArea->width() / gridSize;
    int cellHeight = rightArea->height() / gridSize;

    // 计算放置的网格位置
    int gridX = localPos.x() / cellWidth;
    int gridY = localPos.y() / cellHeight;

    // 确保在网格范围内
    if (gridX >= 0 && gridX < gridSize && gridY >= 0 && gridY < gridSize) {
        // 将网格位置存储在rightArea中
        rightArea->setProperty("dropGridX", gridX);
        rightArea->setProperty("dropGridY", gridY);
        return true;
    }
    return false;
}

void ImagePuzzle::updateRightArea(const QPixmap &pixmap, const QPoint &originalPos) {
    QPixmap currentPixmap = rightArea->pixmap(Qt::ReturnByValue);
    if (currentPixmap.isNull()) {
        // 创建与原图相同宽高比的pixmap
        currentPixmap = QPixmap(rightArea->width(), rightArea->height());
        currentPixmap.fill(Qt::transparent);
    }

    // 从rightArea获取实际的网格位置
    int dropX = rightArea->property("dropGridX").toInt();
    int dropY = rightArea->property("dropGridY").toInt();
    
    // 使用nInput的值计算网格大小
    int gridSize = nInput->text().toInt();
    int pieceWidth = rightArea->width() / gridSize;
    int pieceHeight = rightArea->height() / gridSize;

    // 使用网格位置计算像素坐标
    int x = dropX * pieceWidth;
    int y = dropY * pieceHeight;

    // 绘制图片
    QPainter painter(&currentPixmap);
    painter.drawPixmap(x, y, pixmap.scaled(pieceWidth, pieceHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 更新显示
    rightArea->setPixmap(currentPixmap);

    // 检查这个位置是否是正确的位置（使用原始的行列位置）
    if (dropX == originalPos.x() && dropY == originalPos.y()) {
        correctPieces++;
        if (correctPieces == gridSize * gridSize) {
            QMessageBox::information(this, "完成", "恭喜你完成了拼图！");
        }
    }
}

bool ImagePuzzle::checkCompletion() {
    // 由于我们在updateRightArea中已经处理了完成检查，这里可以简化
    return false;
}
