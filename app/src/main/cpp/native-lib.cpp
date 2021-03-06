#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <zconf.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}
extern "C" JNIEXPORT jstring  JNICALL
Java_com_smxxy_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smxxy_ffmpeg_WanAVPlayer_avPlayer(JNIEnv *env, jobject instance, jstring path_,
                                           jobject surface) {
    //将Java的String转为C的字符串
    const char *path = env->GetStringUTFChars(path_, 0);

    //对网络进行初始化
    avformat_network_init();
    //AVFormatContext是统领全局的基本结构体。主要用于处理封装和解封装，并可以从设备中采集数据。
    AVFormatContext *avFormatContext = avformat_alloc_context();

    //1.打开URL
    AVDictionary *opts = nullptr;
    //设置超时时间 3s
    av_dict_set(&opts, "timeout", "3000000", 0);

    //输入文件的封装格式
    // av_find_input_format("avi")



    // 打开多媒体数据并且获得一些相关的信息 ret为零 表示成功
    //参数一：函数调用成功之后处理过的AVFormatContext结构体
    //参数二：打开的音视频流的URL
    //参数三：强制指定AVFormatContext中AVInputFormat的。这个参数一般情况下可以设置为NULL，这样FFmpeg可以自动检测AVInputFormat。
    //参数四：附加的一些选项，一般情况下可以设置为NULL。
    int ret = avformat_open_input(&avFormatContext, path, nullptr, &opts);

    //读取媒体文件的数据包以获得流信息
    avformat_find_stream_info(avFormatContext, nullptr);

    //视频时长 （单位：微秒us，转换为秒需要除以1000000）nb_streams表示视频中有几种流
    int video_stream_idx = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }
    if (-1 == video_stream_idx) {
        printf("Couldn't find a video stream\n");
        return;
    }

    AVCodecParameters *codecpar = avFormatContext->streams[video_stream_idx]->codecpar;

    //找到解码器
    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    //创建上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
    //根据提供的编解码器的值填充编解码器上下文
    avcodec_parameters_to_context(codecContext, codecpar);
    //打开解码器
    avcodec_open2(codecContext, dec, nullptr);

    //读取包 packet AVPacket：存储压缩数据（视频对应H.264等码流数据，音频对应AAC/MP3等码流数据）
    AVPacket *packet = av_packet_alloc();

    //处理像素数据
    //SwsContext结构体位于libswscale类库中, 该类库主要用于处理图片像素数据, 可以完成图片像素格式的转换, 图片的拉伸等工作.
/*
 *  @param srcW 		源图像的宽度
    @param srcH 		源图像的高度
    @param srcFormat    源图像的图像格式. 如:
		                AV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
		                AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    @param dstW 		目标图像的宽度
    @param dstH 		目标图像的高度
    @param dstFormat	目标图像的图像格式(如上的srcFormat)
    @param flags		标志值, 用于指定rescaling操作的算法和选项
    @param param		格外的参数.

 */
    SwsContext *sws_ctx = sws_getContext(
            codecContext->width, codecContext->height, codecContext->pix_fmt,
            codecContext->width, codecContext->height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, 0, 0, 0);

    //将从 Java 层传递的 Surface 对象转换成 ANativeWindow 结构体,ANativeWindow 就等价于Java层的Surface
    //1.java层将Surface传递给native层
    //2.获取ANativeWindow对象
    //3.将显示数据写到ANativeWindow的buffer中，注意需要将显示的数据格式转换成ANativeWindow设置的数据格式
    //4. 释放ANativeWindow

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    //视频缓冲区
    ANativeWindow_Buffer outBuffer;
    int frameCount = 0;

    //创建新的窗口用于视频显示
    ANativeWindow_setBuffersGeometry(nativeWindow, codecContext->width,
                                     codecContext->height,
                                     WINDOW_FORMAT_RGBA_8888);

    while (av_read_frame(avFormatContext, packet) >= 0) {
        avcodec_send_packet(codecContext, packet);
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);
        if (ret == AVERROR(EAGAIN)) {
            //需要更多数据
            continue;
        } else if (ret < 0) {
            break;
        }
        uint8_t *dst_data[0];
        int dst_linesize[0];
        av_image_alloc(dst_data, dst_linesize,
                       codecContext->width, codecContext->height, AV_PIX_FMT_RGBA, 1);

        if (packet->stream_index == video_stream_idx) {
            //非零   正在解码
            if (ret == 0) {
                //  绘制之前   配置一些信息  比如宽高   格式

                // 绘制
                ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
                // h 264   ----yuv   RGBA
                //转为指定的YUV420P
                sws_scale(sws_ctx,
                          reinterpret_cast<const uint8_t *const *>(frame->data), frame->linesize, 0,
                          frame->height,
                          dst_data, dst_linesize);
                //rgb_frame是有画面数据
                uint8_t *dst = (uint8_t *) outBuffer.bits;
                // 拿到一行有多少个字节 RGBA
                int destStride = outBuffer.stride * 4;
                uint8_t *src_data = dst_data[0];
                int src_linesize = dst_linesize[0];
                uint8_t *firstWindown = static_cast<uint8_t *>(outBuffer.bits);
                for (int i = 0; i < outBuffer.height; ++i) {
                    memcpy(firstWindown + i * destStride, src_data + i * src_linesize, destStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000 * 16);
                av_frame_free(&frame);
            }
        }
    }
    ANativeWindow_release(nativeWindow);
    avcodec_close(codecContext);
    avformat_free_context(avFormatContext);
    env->ReleaseStringUTFChars(path_, path);


}