package com.frca.shutdownandroid.Helpers;

/**
 * Created by KillerFrca on 6.12.13.
 */
public class LimitedExecutor {

    long lastCall;
    long delay;

    public LimitedExecutor(long delay) {
        this.delay = delay;
    }

    public void execute(Runnable runnable) {
        if (lastCall + delay > System.currentTimeMillis())
            return;

        lastCall = System.currentTimeMillis();
        runnable.run();
    }
}
