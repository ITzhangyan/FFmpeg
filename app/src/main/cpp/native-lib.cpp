#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <zconf.h>

extern "C"{
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}
extern "C" JNIEXPORT jstring  JNICALL
Java_com_smxxy_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smxxy_ffmpeg_WanAVPlayer_avPlayer(JNIEnv *env, jobject instance, jstring path_, jobject surface) {

   const char *path = env->GetStringUTFChars(path_,0);

    //对网络进行初始化
    avformat_network_init();
    //AVFormatContext是统领全局的基本结构体。主要用于处理封装和解封装，并可以从设备中采集数据。
    AVFormatContext * avFormatContext = avformat_alloc_context();

    //1.打开URL
    AVDictionary *opts = nullptr;
    //设置超时时间 3s
    av_dict_set(&opts,"timeout","3000000",0);
    //强制指定AVFormatContext中AVInputFormat的。这个参数一般情况下可以设置为NULL，这样FFmpeg可以自动检测AVInputFormat。
    //输入文件的封装格式
    // av_find_input_format("avi")
    // ret为零 表示成功
    int ret = avformat_open_input(&avFormatContext,path,nullptr,&opts);
    avformat_find_stream_info(avFormatContext,nullptr);
    //视频时长 （单位：微秒us，转换为秒需要除以1000000）
    int video_stream_idx = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_idx = i;
            break;
        }
    }
    AVCodecParameters *codecpar = avFormatContext->streams[video_stream_idx]->codecpar;

    //找到解码器
    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    //创建上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
    //根据提供的编解码器的值填充编解码器上下文
    avcodec_parameters_to_context(codecContext,codecpar);

    avcodec_open2(codecContext,dec,nullptr);

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









}