package com.smxxy.ffmpeg

import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

/**
 * @Description: java类作用描述
 * @Author: zhangyan
 * @CreateDate:  2021/3/24 16:08
 */
class WanAVPlayer : SurfaceHolder.Callback, Runnable {
    private var surfaceHolder: SurfaceHolder? = null

    companion object {
        // Used to load the 'wanavpalyer' library on application startup.
        init {
            System.loadLibrary("wanavpalyer")
        }
    }

    fun setSurfaceView(surfaceView: SurfaceView){
        if (null != surfaceView){
            surfaceView.removeCallbacks(this)
        }
        surfaceHolder = surfaceView.holder
        surfaceHolder!!.addCallback(this);
    }

    fun start(path:String){
        avPlayer(path,surfaceHolder!!.surface)
    }

    private external fun avPlayer(path:String, surface:Surface)

    override fun surfaceCreated(p0: SurfaceHolder) {
    }

    override fun surfaceChanged(surfaceHolder: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        this.surfaceHolder = surfaceHolder
    }

    override fun surfaceDestroyed(p0: SurfaceHolder) {
    }

    override fun run() {

    }
}