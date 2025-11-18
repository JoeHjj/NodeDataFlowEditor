#include "MainWindow.hpp"
#include "PortTags.hpp"
#include "factory/NodeFactory.hpp"
#include "taggable/TagApplicator.hpp"
#include "view/GraphScene.hpp"
#include "view/GraphView.hpp"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Register multiple data tag types for the nodes.
    // Required for deserialization: this registers a tag name and its index,
    // allowing the system to identify the associated data type.
    TagApplicator::MultiTagRegistrar<
        data::ImageType,
        data::ValueWrapper<int>,
        data::ValueWrapper<double>,
        data::ValueWrapper<float>,
        data::ValueWrapper<std::string>>
        registerDataTags;

    // Create the main graph scene and view
    m_scene = new GraphScene(this);
    m_view = new GraphView(m_scene, this);

    setCentralWidget(m_view);

    // Example of a custom widget with a checkbox
    class CustomWidget : public QWidget
    {
    public:
        CustomWidget(QWidget* parent = nullptr)
            : QWidget(parent)
        {
            checkBox = new QCheckBox("Custom Option", this);
        }

        QCheckBox* checkBox = nullptr;
    };

    // Create a "Load Image" node
    auto nLoad = m_scene->getNodeFactory()->createNode(
        m_scene, "Load Image", QColor(45, 135, 245), QPointF(60, 60));

    // Add output port and tag
    m_scene->getNodeFactory()->addOutput(*nLoad, "image");
    m_scene->getNodeFactory()->addOutputTag<data::ImageType>(*nLoad, "image");

    {
        // Add parameters for path and a custom widget
        auto* path = new QLineEdit("/data/images/sample.png");
        auto* customWidg = new CustomWidget();
        m_scene->getNodeFactory()->addParameter(*nLoad, path, "path");
        m_scene->getNodeFactory()->addParameter(*nLoad, customWidg, "customWidg");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<QString>>(*nLoad, "path");

        // Add recursive option checkbox
        auto* recursive = new QCheckBox("recursive");
        m_scene->getNodeFactory()->addParameter(*nLoad, recursive, "recursive");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<bool>>(*nLoad, "recursive");

        // Connect checkbox state to a debug output
        connect(recursive, &QCheckBox::stateChanged, this, [path, recursive]() {
            qDebug() << "Load Image Node - Path:" << path->text()
            << ", Recursive:" << recursive->isChecked();
        });
    }

    // Create a "Resize" node
    auto nResize = m_scene->getNodeFactory()->createNode(
        m_scene, "Resize", QColor(70, 160, 230), QPointF(320, 60));

    // Connect input/output ports and tags
    m_scene->getNodeFactory()->addInput(*nResize, "image", "input image");
    m_scene->getNodeFactory()->addInputTag<data::ImageType>(*nResize, "image");
    m_scene->getNodeFactory()->addOutput(*nResize, "image", "output image");
    m_scene->getNodeFactory()->addOutputTag<data::ImageType>(*nResize, "image");

    {
        // Parameters: width, height, keep aspect ratio
        auto* w = new QSpinBox();
        w->setRange(16, 8192);
        w->setValue(640);
        auto* h = new QSpinBox();
        h->setRange(16, 8192);
        h->setValue(480);
        auto* keep = new QCheckBox("keep_aspect");
        keep->setChecked(true);

        m_scene->getNodeFactory()->addParameter(*nResize, w, "width", "kernel width");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<int>>(*nResize, "width");

        m_scene->getNodeFactory()->addParameter(*nResize, h, "height", "kernel height");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<int>>(*nResize, "height");

        m_scene->getNodeFactory()->addParameter(*nResize, keep, "keep_aspect", "keep scale");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<bool>>(*nResize, "keep_aspect");
    }

    // Create a "Normalize" node
    auto nNorm = m_scene->getNodeFactory()->createNode(
        m_scene, "Normalize", QColor(80, 180, 210), QPointF(560, 60));

    m_scene->getNodeFactory()->addInput(*nNorm, "image");
    m_scene->getNodeFactory()->addInputTag<data::ImageType>(*nNorm, "image");
    m_scene->getNodeFactory()->addOutput(*nNorm, "image");
    m_scene->getNodeFactory()->addOutputTag<data::ImageType>(*nNorm, "image");

    {
        // Parameters: mean and std deviation
        auto* mean = new QDoubleSpinBox();
        mean->setRange(-10.0, 10.0);
        mean->setDecimals(3);
        mean->setValue(0.485);
        auto* std = new QDoubleSpinBox();
        std->setRange(0.0, 10.0);
        std->setDecimals(3);
        std->setValue(0.229);

        m_scene->getNodeFactory()->addParameter(*nNorm, mean, "mean");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<float>>(*nNorm, "mean");

        m_scene->getNodeFactory()->addParameter(*nNorm, std, "std");
        m_scene->getNodeFactory()->addParamTag<data::ValueWrapper<float>>(*nNorm, "std");
    }

    // Create other nodes similarly: Canny, Extract Features, Join Metadata, KNN, Overlay, Save Image
    // Each node has inputs, outputs, and parameters with appropriate data tags
}

MainWindow::~MainWindow() = default;
