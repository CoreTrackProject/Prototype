#include "VideoUtils.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}

#include <boost/log/trivial.hpp>
#include <opencv2/imgproc.hpp>

int VideoUtils::GetVideoFrameCount(std::string filePath) {

    // open input file context
    AVFormatContext* inctx = nullptr;

    int ret = avformat_open_input(
        &inctx,
        filePath.c_str(),
        nullptr,
        nullptr);

    if (ret < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to avformat_open_input: \"" << filePath.c_str() << "\" Code:" << ret;
        return 0;
    }

    ret = avformat_find_stream_info(inctx, nullptr);
    if (ret < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to avformat_find_stream_info: Code:" << ret;
        return 0;
    }

    AVCodec* vcodec = nullptr;
    int bestStream = av_find_best_stream(inctx, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);

    if (bestStream < 0) {
        BOOST_LOG_TRIVIAL(error) << "Fail to av_find_best_stream: Code:" << bestStream;
        return 0;
    }

    // Select best stream
    AVStream* vstrm  = inctx->streams[bestStream];





    return vstrm->nb_frames;
}

GLuint VideoUtils::MatToGLuint(cv::Mat frame) {
    GLuint videoFrameGlTexture;

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(1, &videoFrameGlTexture);
    glBindTexture(GL_TEXTURE_2D, videoFrameGlTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
                 0,                // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,           // Internal colour format to convert to
                 frame.cols,       // Image width  i.e. 640 for Kinect in standard mode
                 frame.rows,       // Image height i.e. 480 for Kinect in standard mode
                 0,                // Border width in pixels (can either be 1 or 0)
                 GL_RGB,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE, // Image data type
                 frame.ptr()
                 );                // The actual image data itself

    return videoFrameGlTexture;
}

libmv::FloatImage VideoUtils::MatToFloatImage(cv::Mat &image) {

    libmv::FloatImage result(image.cols, image.rows);
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            result.Fill(image.at<unsigned char>(i, j));
        }
    }
    return result;



   /* cv::Mat dst(image.cols, image.rows, CV_32F);
    image.convertTo(dst, CV_32F);

    //std::vector<float> data;
    //dst.copyTo(data);

    std::vector<float> vec = image.isContinuous() ? dst : dst.clone();

    libmv::FloatImage result(
        vec.data(),
        image.cols,
        image.rows
        );

    return result;*/


}
