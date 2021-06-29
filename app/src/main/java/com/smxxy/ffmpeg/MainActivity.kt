package com.smxxy.ffmpeg

import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.SurfaceView
import android.view.View
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import java.io.File


class MainActivity : AppCompatActivity() {

    var surfaceView: SurfaceView? = null
    var wangyiPlayer: WanAVPlayer? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        window.setFlags(
            WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
            WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
        )
        surfaceView = findViewById(R.id.surfaceView)
        wangyiPlayer = WanAVPlayer()
        wangyiPlayer!!.setSurfaceView(surfaceView!!)
    }
    fun open(view: View?) {
        Log.e("====", "open: ${Environment.getExternalStorageDirectory()}")

        val file = File(Environment.getExternalStorageDirectory(), "input.mp4")
        wangyiPlayer?.start(file.absolutePath)
    }

    /**
     * 播放 ffplay -ar 44100 -ac 2 -f s16le -i output.pcm
     */
    fun openAudio(view: View) {
        val input = File(Environment.getExternalStorageDirectory(), "input.mp3").absolutePath
        val output = File(Environment.getExternalStorageDirectory(), "output.pcm").absolutePath
        wangyiPlayer?.startAudio(input, output)
    }
}