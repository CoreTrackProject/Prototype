#include "ReadVideo.h"

#include <iostream>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <opencv2/imgproc.hpp>


ReadVideo::ReadVideo() {

}

ReadVideo::~ReadVideo() {
    this->result.reset();
}


void ReadVideo::Start() {
    this->status = TaskStatus::Running;

    this->result = std::make_shared<ReadVideoResult>();

    if(this->readVideo()) {
        this->status = TaskStatus::Finished;
        return;
    } else {
        this->status = TaskStatus::Errored;
    }
}

bool ReadVideo::readVideo() {

    // open input file context
    AVFormatContext* formatContext = nullptr;

    int ret = avformat_open_input(
        &formatContext,
        this->videoFilePath.c_str(),
        nullptr,
        nullptr);

    if (ret < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to avformat_open_input: \"" << this->videoFilePath.c_str() << "\" Code:" << ret;
        return false;
    }

    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to avformat_find_stream_info: Code:" << ret;
        return false;
    }

    AVCodec* vcodec = nullptr;
    int bestStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);

    if (bestStream < 0) {
        BOOST_LOG_TRIVIAL(error) << "Fail to av_find_best_stream: Code:" << bestStream;
        return false;
    }

    // Select best stream
    AVStream* vstrm  = formatContext->streams[bestStream];

    AVCodecContext *codecContext = avcodec_alloc_context3(vcodec);
    avcodec_parameters_to_context(codecContext, vstrm->codecpar);

    ret = avcodec_open2(codecContext, vcodec, nullptr);
    if (ret < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to avcodec_open2: Code:" << ret;
        return false;
    }


    BOOST_LOG_TRIVIAL(info)
        << "File       : " << this->videoFilePath << "\n"
        << "Format     : " << formatContext->iformat->name << "\n"
        << "Codec      : " << vcodec->name << "\n"
        << "Size       : " << codecContext->width << 'x' << codecContext->height << "\n"
        << "FPS        : " << av_q2d(codecContext->framerate) << " [fps]\n"
        << "Length     : " << av_rescale_q(vstrm->duration, vstrm->time_base, {1,1000}) / 1000. << " [sec]\n"
        << "Pix format : " << av_get_pix_fmt_name(codecContext->pix_fmt) << "\n"
        << "Framecount : " << vstrm->nb_frames << "\n";


    SwsContext*  sws_ctx =
        sws_getContext(   // [13]
            codecContext->width,
            codecContext->height,
            codecContext->pix_fmt,
            codecContext->width,
            codecContext->height,
            //AVPixelFormat::AV_PIX_FMT_BGR24,   // sws_scale destination color scheme
            AVPixelFormat::AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            nullptr,
            nullptr,
            nullptr
            );


    // Convert frames to opencv Mat
    AVPacket pkt;
    AVFrame* decFrame = av_frame_alloc();

    int idx = 0;
    while(av_read_frame(formatContext, &pkt)  >= 0) {


        if (pkt.stream_index != bestStream)
        {
            av_packet_unref(&pkt);
            continue;
        }

        int ret = avcodec_send_packet(codecContext, &pkt);
        if (ret < 0)
        {

            // could not send packet for decoding
            BOOST_LOG_TRIVIAL(error) << "Error sending packet for decoding";

            return false;
        }


        while(ret >= 0) {
            ret = avcodec_receive_frame(codecContext, decFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            {
                // EOF exit loop
                break;
            }
            else if (ret < 0)
            {
                // could not decode packet
                BOOST_LOG_TRIVIAL(error) << "Error while decoding.\n";

                // exit with error
                return false;
            }



            if(idx >= this->startFrame && idx <= this->endFrame) { // 0 - 99 resulted frame count should be 100 / [99; 99] -> 1 frame / [0; 1] -> 2 frames

                cv::Mat image = cv::Mat(decFrame->height, decFrame->width, CV_8UC3);
                int cvLinesizes[1];
                cvLinesizes[0] = image.step1();

                sws_scale(sws_ctx, decFrame->data, decFrame->linesize, 0, decFrame->height, &image.data, cvLinesizes);

                this->result->VideoFrameCollection.push_back(image.clone());
                idx++;

            } else {
                idx++;
                break;
            }




        }

        av_packet_unref(&pkt);
    }

    av_frame_free(&decFrame);
    sws_freeContext(sws_ctx);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);


    this->result->Successful = true;

    return true;
}

void ReadVideo::Stop() {
    this->status = TaskStatus::Stopped;
}


void ReadVideo::Pause() {
    this->status = TaskStatus::Paused;
}


TaskStatus ReadVideo::GetStatus() {
    return this->status;
}



std::shared_ptr<void> ReadVideo::GetResult() {
    return this->result;
}

void ReadVideo::SetVideoFilePath(std::string videoFilePath) {
    this->videoFilePath = videoFilePath;
}

void ReadVideo::SetReadRange(int startFrame, int endFrame) {

    this->startFrame = startFrame;
    this->endFrame   = endFrame;


}
