package com.smxxy.ffmpeg

import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

/**
 * @Description: java类作用描述
 * @Author: zhangyan
 * @CreateDate:  2021/3/24 16:08
 */
class WanAVPlayer : SurfaceHolder.Callback {
    //SurfaceHolder 一个关于Surface的监听器。提供访问和控制SurfaceView背后的Surface 相关的方法
    private var surfaceHolder: SurfaceHolder? = null

    companion object {
        // Used to load the 'wanavpalyer' library on application startup.
        init {
            System.loadLibrary("wanavpalyer")
        }
    }

    fun setSurfaceView(surfaceView: SurfaceView){
        if (null != surfaceHolder){
            this.surfaceHolder?.removeCallback(this)
        }
        surfaceHolder = surfaceView.holder
        surfaceHolder!!.addCallback(this);
    }

    /**
     * 开始播放
     */
    fun start(path:String){
        avPlayer(path,surfaceHolder!!.surface)
    }

    /**
     * 此方法与 native交互
     */
    private external fun avPlayer(path:String, surface:Surface)

    override fun surfaceCreated(p0: SurfaceHolder) {
    }

    override fun surfaceChanged(surfaceHolder: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        this.surfaceHolder = surfaceHolder
    }

    override fun surfaceDestroyed(p0: SurfaceHolder) {
    }


}